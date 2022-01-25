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
    // Structure d'un Job
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
        QMap<int, int> nbJobsFinished;
        QMap<int, QSharedPointer<PcoConditionVariable>> waitingMasters;
    public:
        Buffer() : jobs(), nbJobsFinished(), waitingMasters() {}

        ~Buffer()
        {
            if (!waitingMasters.isEmpty()) {
                waitingMasters.clear(); // Supprime directement les allocations créées
            }
        }

        /**
         * Ajoute un Job dans la liste des jobs à réaliser
         * @param job : Job à réaliser
         */
        void sendJob(Job job  /* Maybe some parameters */) {
            mutex.lock();
            jobs.append(job);
            if (!nbJobsFinished.contains(job.id))
                nbJobsFinished.insert(job.id, 0);

            if (!waitingMasters.contains(job.id))
                waitingMasters.insert(job.id, (QSharedPointer<PcoConditionVariable>)new PcoConditionVariable());
            cond.notifyOne();
            mutex.unlock();
        }

        /**
         * Retourne le Job le plus ancien et le supprime de la liste des jobs
         * à réaliser. Si aucun Job n'est disponible, la fonction bloque en
         * attendant un nouveau Job.
         * @return le Job à réaliser
         */
        Job/* Maybe not void... */ getJob() {
            Job job;
            mutex.lock();
            while (jobs.empty()) {
                if (PcoThread::thisThread()->stopRequested()) {
                    break;
                }
                cond.wait(&mutex);
            }
            if (jobs.size() >= 1) {
                job = jobs.first();
                jobs.removeFirst();
            }
            mutex.unlock();
            return job;
        }

        /**
         * Vérifie que tous les jobs d'une multiplication de matrice
         * sont terminés.
         * @param id du calcul de la matrice à vérifier
         * @return true si le calcul est terminé, false sinon
         */
        void waitJobsFinished(int id, int nbTotalJobs) {
            mutex.lock();
            while (nbTotalJobs != nbJobsFinished[id]) {
                waitingMasters[id]->wait(&mutex);
            }
            mutex.unlock();
        }

        void finishedJob(int id) {
            mutex.lock();
            nbJobsFinished[id]++;
            waitingMasters[id]->notifyOne();
            mutex.unlock();
        }

        int getNbJobsFinished(int id) {
            int nb;
            mutex.lock();
            nb = nbJobsFinished[id];
            mutex.unlock();
            return nb;
        }

        /**
         * Libère tous les threads bloqués en attente d'un Job
         */
        void freeAllThreads() {
            cond.notifyAll();
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
        : m_nbThreads(nbThreads), m_nbBlocksPerRow(nbBlocksPerRow), counter(0), threads()
    {
        for (int i = 0; i < nbThreads;++i) {
            threads.append((QSharedPointer<PcoThread>)new PcoThread(&ThreadedMatrixMultiplier::threadRun, this));
        }
    }

    void threadRun() {
        while (1) {
            Job job = buffer.getJob();
            // Si le thread doit être arrêté, il sort de la boucle
            if (PcoThread::thisThread()->stopRequested())
                return;

            // Multiplication des index attribués au Job
            for (int i = 0; i < job.size; i++) {
                for (int j = 0; j < job.size; j++) {
                    for (int k = 0; k < job.A->size(); k++) {
                        job.C->setElement(j + job.colIndex, i + job.rowIndex,
                                          job.C->element(j + job.colIndex, i + job.rowIndex) +
                                          job.A->element(k, job.rowIndex + i) *
                                          job.B->element(job.colIndex + j, k));
                    }
                }
            }
            buffer.finishedJob(job.id);
        }
    }

    ///
    /// In this destructor we should ask for the termination of the computations. They could be aborted without
    /// ending into completion.
    /// All threads have to be
    ///
    ~ThreadedMatrixMultiplier()
    {
        for (QSharedPointer<PcoThread> &thread : threads) {
            thread->requestStop();
        }
        buffer.freeAllThreads();
        for (QSharedPointer<PcoThread> &thread : threads) {
            thread->join();
        }

        if (!threads.isEmpty()){
            threads.clear(); // Supprime directement les allocations créées
        }
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
        // Permet de savoir à quelle matrice le Job fait référence
        int id;
        mutex.lock();
        id = ++counter;
        mutex.unlock();

        int size = A.getSizeX() / nbBlocksPerRow;
        int nbJobs = nbBlocksPerRow * nbBlocksPerRow;

        // Crée les différents jobs
        for (int i = 0; i < nbBlocksPerRow; ++i) {
            for (int j = 0; j < nbBlocksPerRow; ++j) {
                Job job = {
                    .id = id,
                    .finished = false,
                    .size = size,
                    .rowIndex = i * size,
                    .colIndex = j * size,
                    .A = &A,
                    .B = &B,
                    .C = C,
                };

                buffer.sendJob(job);
            }
        }
        // Attend que le calcul de la matrice soit terminé par les différents threads
        buffer.waitJobsFinished(id, nbJobs);

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
