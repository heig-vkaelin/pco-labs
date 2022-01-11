#ifndef SIMPLEMATRIXMULTIPLIER_H
#define SIMPLEMATRIXMULTIPLIER_H

#include "abstractmatrixmultiplier.h"

/**
 * A simple implementation of the matrix multiplication.
 */
template<class T>
class SimpleMatrixMultiplier : public AbstractMatrixMultiplier<T>
{
public:
    void multiply(SquareMatrix<T>& A, SquareMatrix<T>& B, SquareMatrix<T>* C)
    {
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < A.size(); j++) {
                for (int k = 0; k < A.size(); k++) {
                    C->setElement(i, j, C->element(i, j) + A.element(k, j) * B.element(i, k));
                }
            }
        }
    }
};


#endif // SIMPLEMATRIXMULTIPLIER_H
