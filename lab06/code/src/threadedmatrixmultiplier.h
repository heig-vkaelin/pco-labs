#ifndef THREADEDMATRIXMULTIPLIER_H
#define THREADEDMATRIXMULTIPLIER_H

#include <vector>
#include <QList>
#include <QDebug>

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

#include "abstractmatrixmultiplier.h"
#include "matrix.h"

///
/// A multi-threaded multiplicator. multiply() should at least be reentrant.
/// It is up to you to offer a very good parallelism.
///
template<class T>
class ThreadedMatrixMultiplier : public AbstractMatrixMultiplier<T>
{
    // Job structure
    struct Job {
        int id; // id de la multiplication
        bool finished;
        int size;
        int rowIndex;
        int colIndex;
        SquareMatrix<T>* A;
        SquareMatrix<T>* B;
        SquareMatrix<T>* C;
    };

    /// As a suggestion, a buffer class that could be used to communicate between
    /// the workers and the main thread...
    ///
    /// Here we only wrote two potential methods, but there could be more at the end...
    ///
    ///
    class Buffer
    {
    private:
        PcoMutex mutex;
        PcoConditionVariable cond;
        QList<Job> jobs;
    public:
        Buffer() : jobs() {}

        void sendJob(Job job  /* Maybe some parameters */) {
            mutex.lock();
            jobs.append(job);
            cond.notifyOne();
            mutex.unlock();
        }

        Job/* Maybe not void... */ getJob() {
            Job job;
            mutex.lock();
            while (jobs.empty()) {
                if(PcoThread::thisThread()->stopRequested()){
                    break;
                }
                cond.wait(&mutex);
            }
            if(jobs.size() >= 1){
                job = jobs.first();
                jobs.removeFirst();
            }
            mutex.unlock();
            return job;

            // faire opérations sur result

            // demander protection matrix C
            // appliquer modif dans C

        }

        bool finished(int id) {
            bool result = true;
            mutex.lock();
            for(Job& j : jobs)
                if (j.id == id) {
                    result = false;
                    break;
                }
            mutex.unlock();
            return result;
        }
        void freeAllThreads() {
            cond.notifyAll();
        }

        void writeResult(SquareMatrix<T> result, Job &job) {
            // Apply result in C matrix
            for (int i = 0; i < job.size; i++) {
                for (int j = 0; j < job.size; j++) {
                    job.C->setElement(job.rowIndex + i, job.colIndex + j, result.element(i,j));
                }
            }
        }

    };

public:
    ///
    /// \brief ThreadedMatrixMultiplier
    /// \param nbThreads Number of threads to start
    /// \param nbBlocksPerRow Default number of blocks per row, for compatibility with SimpleMatrixMultiplier
    ///
    /// The threads shall be started from the constructor
    ///
    ThreadedMatrixMultiplier(int nbThreads, int nbBlocksPerRow = 0)
        : m_nbThreads(nbThreads), m_nbBlocksPerRow(nbBlocksPerRow), counter(0),threads()
    {
        for(int i = 0; i < nbThreads;++i){
            threads.append((QSharedPointer<PcoThread>)new PcoThread(&ThreadedMatrixMultiplier::threadRun,this));
        }
    }

    void threadRun(){
        while(1) {
            Job job = buffer.getJob();
            if(PcoThread::thisThread()->stopRequested())
                return;
            job.C->print();
            // TODO: bouger ça de cette méthode
            for (int i = 0; i < job.size; i++) {
                for (int j = 0; j < job.size; j++) {
                    for (int k = 0; k < job.A->size(); k++) {
                        job.C->setElement(i+job.rowIndex, j+job.colIndex, job.C->element(i+job.rowIndex, j+job.colIndex) + job.A->element(job.rowIndex + i,job.colIndex +  k) * job.B->element(job.rowIndex + k, job.colIndex + j));
                        qDebug() << i + job.rowIndex << " / " << j+job.colIndex << " -/- " <<job.rowIndex + i << job.colIndex + k << " / " << job.rowIndex + k<< job.colIndex + j << " / " << job.id;

                    }
                    qDebug() << "-----------------------------------------------";

                }
                //C->setElement(i, j, C->element(i, j) + A.element(k, j) * B.element(i, k));

            }

            /*for (int i = 0; i < job.size; i++) {
                for (int j = 0; j < job.size; j++) {
                    for (int k = 0; k < job.A.size(); k++) {
                        job.C->setElement(i+job.rowIndex, j+job.colIndex, job.C->element(i+job.rowIndex, j+job.colIndex) + A.element(k, j) * B.element(i, k));
                    }
                }
            }*/
            job.C->print();

        }
    }

    ///
    /// In this destructor we should ask for the termination of the computations. They could be aborted without
    /// ending into completion.
    /// All threads have to be
    ///
    ~ThreadedMatrixMultiplier()
    {
        // TODO DELETE CORRECTEMENT LES THREADS
        /*for(QSharedPointer<PcoThread> thread : threads){
            thread->requestStop();
            thread->join();
        }
        if(!threads.isEmpty()){
            threads.clear();
        }*/

    }

    ///
    /// \brief multiply
    /// \param A First matrix
    /// \param B Second matrix
    /// \param C Result of AxB
    ///
    /// For compatibility reason with SimpleMatrixMultiplier
    void multiply(SquareMatrix<T>& A, SquareMatrix<T>& B, SquareMatrix<T>* C)
    {
        multiply(A, B, C, m_nbBlocksPerRow);
    }

    ///
    /// \brief multiply
    /// \param A First matrix
    /// \param B Second matrix
    /// \param C Result of AxB
    ///
    /// Executes the multithreaded computation, by decomposing the matrices into blocks.
    void multiply(SquareMatrix<T>& A, SquareMatrix<T>& B, SquareMatrix<T>* C, int nbBlocksPerRow)
    {
        // OK, computation is done correctly, but... Is it really multithreaded?!?
        // TODO : Get rid of the next lines and do something meaningful

        // Permet de savoir à quelle matrice le job fait référence
        int id;
        mutex.lock();
        id = ++counter;
        mutex.unlock();

        int size = A.getSizeX() / nbBlocksPerRow;

        // Appeller les sendJobs
        for (int i = 0; i < nbBlocksPerRow; ++i) {
            for (int j = 0; j < nbBlocksPerRow; ++j) {
                Job job = {
                    .id = id,
                    .finished = false,
                    .size = size,
                    .rowIndex = i*size,
                    .colIndex = j*size,
                    .A = &A,
                    .B = &B,
                    .C = C,
                };

                buffer.sendJob(job);
            }
        }
        while (!buffer.finished(id)){}

        for(QSharedPointer<PcoThread> &thread : threads){
            thread->requestStop();
        }
        buffer.freeAllThreads();
        for(QSharedPointer<PcoThread> &thread : threads){
            thread->join();
        }




        /*for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < A.size(); j++) {
                for (int k = 0; k < A.size(); k++) {
                    C->setElement(i, j, C->element(i, j) + A.element(k, j) * B.element(i, k));
                }
            }
        }*/
    }

protected:
    int m_nbThreads;
    int m_nbBlocksPerRow;
    int counter;
    Buffer buffer;
    PcoMutex mutex;
    QList<QSharedPointer<PcoThread>> threads;
};




#endif // THREADEDMATRIXMULTIPLIER_H
