/*
Auteurs: Alexandre Jaquier, Valentin Kaelin
Date: 29.01.2022
Description: Ajouts de quelques tests
*/

#include <future>

#include <gtest/gtest.h>
#include <pcosynchro/pcotest.h>

#include "multipliertester.h"
#include "multiplierthreadedtester.h"
#include "src/threadedmatrixmultiplier.h"

#define ThreadedMultiplierType ThreadedMatrixMultiplier<int>

// Decommenting the next line allows to check for interlocking
#define CHECK_DURATION

// Test sur une petite matrice avec des valeurs dans les matrices très petites
TEST(Multiplier, TestSmallValues){

#ifdef CHECK_DURATION
        ASSERT_DURATION_LE(30, ({
#endif // CHECK_DURATION
                               constexpr int MATRIXSIZE = 4;
                               constexpr int NBTHREADS = 1;
                               constexpr int NBBLOCKSPERROW = 2;
                               constexpr int MAX_VALUE = 10;

                               MultiplierTester<ThreadedMultiplierType> tester;

                               tester.test_values_limited(MATRIXSIZE, NBTHREADS, NBBLOCKSPERROW, MAX_VALUE);

#ifdef CHECK_DURATION
                           }))
#endif // CHECK_DURATION

}

TEST(Multiplier, SingleThread){

#ifdef CHECK_DURATION
        ASSERT_DURATION_LE(30, ({
#endif // CHECK_DURATION
                               constexpr int MATRIXSIZE = 500;
                               constexpr int NBTHREADS = 1;
                               constexpr int NBBLOCKSPERROW = 5;

                               MultiplierTester<ThreadedMultiplierType> tester;

                               tester.test(MATRIXSIZE, NBTHREADS, NBBLOCKSPERROW);

#ifdef CHECK_DURATION
                           }))
#endif // CHECK_DURATION

}


TEST(Multiplier, Simple){

#ifdef CHECK_DURATION
        ASSERT_DURATION_LE(30, ({
#endif // CHECK_DURATION
                               constexpr int MATRIXSIZE = 500;
                               constexpr int NBTHREADS = 4;
                               constexpr int NBBLOCKSPERROW = 5;

                               MultiplierTester<ThreadedMultiplierType> tester;

                               tester.test(MATRIXSIZE, NBTHREADS, NBBLOCKSPERROW);

#ifdef CHECK_DURATION
                           }))
#endif // CHECK_DURATION

}


TEST(Multiplier, Reentering)
{

#ifdef CHECK_DURATION
    ASSERT_DURATION_LE(30, ({
#endif // CHECK_DURATION
                           constexpr int MATRIXSIZE = 500;
                           constexpr int NBTHREADS = 4;
                           constexpr int NBBLOCKSPERROW = 5;

                           MultiplierThreadedTester<ThreadedMultiplierType> tester(2);

                           tester.test(MATRIXSIZE, NBTHREADS, NBBLOCKSPERROW);

#ifdef CHECK_DURATION
                       }))
#endif // CHECK_DURATION
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
