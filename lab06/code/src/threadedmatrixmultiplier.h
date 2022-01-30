#ifndef THREADEDMATRIXMULTIPLIER_H
#define THREADEDMATRIXMULTIPLIER_H

/*
Auteurs: Alexandre Jaquier, Valentin Kaelin
Date: 29.01.2022
Description: Version multi-thread de la multiplication matricielle
*/

#include <QList>
#include <QMap>
#include <QSharedPointer>

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
        int size; // taille du bloc à traiter
        int nbTotalJobs; // nombre de jobs pour la multiplication
        int rowIndex, colIndex;
        SquareMatrix<T> *A, *B, *C;
    };

    /**
     * Classe servant à communiquer entre le thread principal et les différents workers.
     * Réalisé à l'aide de moniteurs de Mesa afin de pouvoir réveiller facilement tous
     * threads en attente d'un Job.
     */
    class Buffer
    {
    private:
        // Mutex gérant la gestion de l'envoi et la réception de job
        PcoMutex mutex;
        // Condition gérant la gestion de l'envoi et la réception de job
        PcoConditionVariable cond;
        QList<Job> jobs;
        // Map stockant pour chaque multiplication le nombre de jobs fini
        QMap<int, int> nbJobsFinished;
        // Map stockant pour chaque multiplication la condition permettant de stopper le thread principal
        QMap<int, QSharedPointer<PcoConditionVariable>> waitingMasters;
    public:
        Buffer() : jobs(), nbJobsFinished(), waitingMasters() {}

        ~Buffer() {
            // Supprime les allocations créées
            if (!waitingMasters.isEmpty())
                waitingMasters.clear();
        }

        /**
         * Ajoute un Job dans la liste des jobs à réaliser
         * @param job : Job à réaliser
         */
        void sendJob(Job job) {
            mutex.lock();
            jobs.append(job);
            // Annonce qu'un nouveau job est disponible
            cond.notifyOne();
            mutex.unlock();
        }

        /**
         * Retourne le Job le plus ancien et le supprime de la liste des jobs
         * à réaliser. Si aucun Job n'est disponible, la fonction bloque en
         * attendant un nouveau Job.
         * @return le Job à réaliser
         */
        Job getJob() {
            Job job;
            mutex.lock();
            while (jobs.empty()) {
                if (PcoThread::thisThread()->stopRequested())
                    break;

                cond.wait(&mutex);
            }
            if (jobs.size() > 0) {
                job = jobs.first();
                jobs.removeFirst();
            }
            mutex.unlock();
            return job;
        }

        /**
         * Initie un nouveau calcul en mettant un état initial aux différentes
         * structures utilisées
         * @param id du calcul de la matrice à initialiser
         */
        void initNewMultiplication(int id) {
            mutex.lock();
            nbJobsFinished.insert(id, 0);
            waitingMasters.insert(id, (QSharedPointer<PcoConditionVariable>)new PcoConditionVariable());
            mutex.unlock();
        }

        /**
         * Attend que tous les jobs d'une multiplication de matrice
         * soient terminés via un moniteur de Mesa.
         * @param id du calcul de la matrice à vérifier
         * @param nbTotalJobs nombre de Jobs total à réaliser pour la multiplication de matrices
         */
        void waitJobsFinished(int id, int nbTotalJobs) {
            mutex.lock();
            while (nbTotalJobs != nbJobsFinished[id]) {
                waitingMasters[id]->wait(&mutex);
            }
            // Suppression des informations concernant la multiplication terminée
            nbJobsFinished.remove(id);
            waitingMasters.remove(id); // Va également supprimer le QSharedPointer
            mutex.unlock();
        }

        /**
         * Annonce au buffer qu'un thread a terminé un Job
         * @param id du calcul de la matrice
         * @param nbTotalJobs nombre de Jobs total à réaliser pour la multiplication de matrices
         */
        void finishedJob(int id, int nbTotalJobs) {
            mutex.lock();
            // Si tous les jobs sont terminés, le thread principal est notifié
            if (++nbJobsFinished[id] == nbTotalJobs)
                waitingMasters[id]->notifyOne();
            mutex.unlock();
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
        // Crée le nombre de threads demandé
        for (int i = 0; i < nbThreads; ++i)
            threads.append(
                        (QSharedPointer<PcoThread>)new PcoThread(&ThreadedMatrixMultiplier::threadRun, this)
                        );
    }

    /**
     * Fonction exécutée par les différents threads
     * S'occupe de récupérer un Job, puis de réaliser les calculs.
     * Annonce au buffer une fois ce Job terminé.
     */
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

            // Annonce que le Job est terminé
            buffer.finishedJob(job.id, job.nbTotalJobs);
        }
    }

    ///
    /// In this destructor we should ask for the termination of the computations. They could be aborted without
    /// ending into completion.
    /// All threads have to be
    ///
    ~ThreadedMatrixMultiplier()
    {
        // Annonce aux threads qu'ils doivent se terminer
        for (QSharedPointer<PcoThread> &thread : threads)
            thread->requestStop();

        // Notifie tous les threads qui attendent pour avoir un Job
        buffer.freeAllThreads();

        // Attend que les threads se terminent sans erreur
        for (QSharedPointer<PcoThread> &thread : threads)
            thread->join();

        // Supprime les allocations créées
        if (!threads.isEmpty())
            threads.clear();
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
        // Permet de savoir à quel calcul matriciel le Job fait référence
        int id;
        counterMutex.lock();
        id = counter++;
        // Annonce au buffer qu'un nouveau calcul se prépare
        buffer.initNewMultiplication(id);
        counterMutex.unlock();

        int size = A.getSizeX() / nbBlocksPerRow;
        int nbTotalJobs = nbBlocksPerRow * nbBlocksPerRow;

        Job job = {
            .id = id,
            .size = size,
            .nbTotalJobs = nbTotalJobs,
            .rowIndex = 0,
            .colIndex = 0,
            .A = &A,
            .B = &B,
            .C = C,
        };

        // Crée les différents jobs
        for (int i = 0; i < nbBlocksPerRow; ++i) {
            for (int j = 0; j < nbBlocksPerRow; ++j) {
                job.rowIndex = i * size;
                job.colIndex = j * size;
                buffer.sendJob(job);
            }
        }

        // Attend que le calcul de la matrice soit terminé par les différents threads
        buffer.waitJobsFinished(id, nbTotalJobs);
    }

protected:
    int m_nbThreads;
    int m_nbBlocksPerRow;
    int counter; // permet de générer un identifiant unique pour chaque calcul matriciel
    Buffer buffer;
    PcoMutex counterMutex; // permet de protéger la variable counter
    QList<QSharedPointer<PcoThread>> threads;
};




#endif // THREADEDMATRIXMULTIPLIER_H
