#ifndef MULTIPLIERTESTER_H
#define MULTIPLIERTESTER_H

/*
Auteurs: Alexandre Jaquier, Valentin Kaelin
Date: 29.01.2022
Description: Ajout d'une fonction permettant de limiter les valeurs
             générées dans les matrices
*/

#include <chrono>
#include <iostream>

#include "matrix.h"
#include "simplematrixmultiplier.h"
#include "threadedmatrixmultiplier.h"



/**
 * This class implements a tester for the multiplier. It calls a simple
 * implementation of multiply and the multi-threaded one, compares the
 * result, and the time spent by both implementation.
 */
template<class ThreadedMultiplierType>
class MultiplierTester
{
public:
    MultiplierTester() {}

    /**
     * Fonction de test déjà présente, n'a pas de limite de valeur maximale
     */
    void test(int matrixSize, int nbThreads, int nbBlocksPerRow) {
        return test_values_limited(matrixSize, nbThreads, nbBlocksPerRow, RAND_MAX);
    }

    /**
     * Fonction de test permettant de donner une limite maximale aux
     * valeurs aléatoires de la matrice.
     *
     * @param matrixSize
     * @param nbThreads
     * @param nbBlocksPerRow
     * @param maxValue : valeur maximale des valeurs générées dans la matrice
     */
    void test_values_limited(int matrixSize, int nbThreads, int nbBlocksPerRow, int maxValue)
    {
        using T = decltype(ThreadedMultiplierType::getElementType());

        SquareMatrix<T> A(matrixSize);
        SquareMatrix<T> B(matrixSize);
        SquareMatrix<T> C(matrixSize);
        SquareMatrix<T> C_ref(matrixSize);

        for (int i = 0; i < matrixSize; i++) {
            for (int j = 0; j < matrixSize; j++) {
                A.setElement(i, j, rand() % maxValue);
                B.setElement(i, j, rand() % maxValue);
                C.setElement(i, j, 0);
                C_ref.setElement(i, j, 0);
            }
        }

        SimpleMatrixMultiplier<T> multiplier;
        auto start = std::chrono::steady_clock::now();
        multiplier.multiply(A, B, &C_ref);
        auto end = std::chrono::steady_clock::now();
        int64_t timeSimple = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        ThreadedMultiplierType threadedMultiplier(nbThreads, nbBlocksPerRow);
        start = std::chrono::steady_clock::now();
        threadedMultiplier.multiply(A, B, &C);
        end = std::chrono::steady_clock::now();
        int64_t timeThreaded = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        C.compare(C_ref);

        if (timeThreaded == 0) {
            std::cout << "Time too short, try with a bigger matrix size" << std::endl;
        }
        else {
            double gain = static_cast<double>(timeSimple) / static_cast<double>(timeThreaded) * 100.0 - 100.0;
            std::cout << "Time gain: " << gain << " % " << std::endl;
        }
    }
};

#endif // MULTIPLIERTESTER_H
