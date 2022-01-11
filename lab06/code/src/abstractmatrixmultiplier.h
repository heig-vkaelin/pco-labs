#ifndef ABSTRACTMATRIXMULTIPLIER_H
#define ABSTRACTMATRIXMULTIPLIER_H

#include "matrix.h"

/**
 * The abstract matrix multiplier, only supplying a method for the
 * multiplication.
 */
template<class T>
class AbstractMatrixMultiplier
{
public:
    /**
     * C = A * B
     */
    virtual void multiply(SquareMatrix<T>& A, SquareMatrix<T>& B, SquareMatrix<T>* C) = 0;

    //! Empty virtual destructor, needed for correct polymorphism
    virtual ~AbstractMatrixMultiplier() {}

    static auto getElementType()
    {
        return T{};
    }
};

#endif // ABSTRACTMATRIXMULTIPLIER_H
