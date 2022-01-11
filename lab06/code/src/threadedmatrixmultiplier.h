#ifndef THREADEDMATRIXMULTIPLIER_H
#define THREADEDMATRIXMULTIPLIER_H

#include <vector>

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

    /// As a suggestion, a buffer class that could be used to communicate between
    /// the workers and the main thread...
    ///
    /// Here we only wrote two potential methods, but there could be more at the end...
    ///
    class Buffer
    {
    public:
        void sendJob(/* Maybe some parameters */) {}

        void /* Maybe not void... */ getJob() {}
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
        : m_nbThreads(nbThreads), m_nbBlocksPerRow(nbBlocksPerRow)
    {
        // TODO
    }

    ///
    /// In this destructor we should ask for the termination of the computations. They could be aborted without
    /// ending into completion.
    /// All threads have to be
    ///
    ~ThreadedMatrixMultiplier()
    {
        // TODO
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
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < A.size(); j++) {
                for (int k = 0; k < A.size(); k++) {
                    C->setElement(i, j, C->element(i, j) + A.element(k, j) * B.element(i, k));
                }
            }
        }
    }

protected:
    int m_nbThreads;
    int m_nbBlocksPerRow;
};




#endif // THREADEDMATRIXMULTIPLIER_H
