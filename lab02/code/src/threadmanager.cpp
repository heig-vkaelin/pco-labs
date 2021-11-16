#include <QCryptographicHash>
#include <QVector>
#include <QDebug>

#include <pcosynchro/pcothread.h>
#include "mythread.h"
#include "threadmanager.h"

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
        long long unsigned int number,
        long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}


void ThreadManager::incrementPercentComputed(double percentComputed)
{
    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned nbChars,
        unsigned nbThreads
)
{
    // Variable à true si le hash a été trouvé par un des threads
    bool finished = false;

    // Nombre total de hashs à tester
    long long unsigned nbToCompute = intPow(charset.length(), nbChars);

    // Index de départ des différents threads
    long long unsigned startingIndex = 0;

    // Vecteur contenant des pointeurs sur les différents threads lancés
    QVector<PcoThread*> threads(nbThreads);

    // Résultat final modifié par un des différents threads
    QString result;

    // Vecteur contenant le nombre exact de hashs à tester par thread
    QVector<long long unsigned> nbToComputePerThread(nbThreads);

    // Split des nombres à compute entre les threads
    if(nbToCompute % nbThreads == 0) {
        nbToComputePerThread.fill(nbToCompute / nbThreads);
    } else {
        long long unsigned toDistribute = nbThreads - (nbToCompute % nbThreads);
        long long unsigned meanValue = nbToCompute / nbThreads;
        for (unsigned i = 0; i < nbThreads; ++i) {
            if(i >= toDistribute) {
                nbToComputePerThread[i] = meanValue + 1;
            } else {
                nbToComputePerThread[i] = meanValue;
            }
        }
    }
    qInfo() << "nbToComputePerThread: " << nbToComputePerThread;

    // Création des threads
    for (unsigned i = 0; i < nbThreads; ++i) {
        PcoThread* thread = new PcoThread(
                    runComputation,
                    charset,
                    salt,
                    hash,
                    nbChars,
                    nbToComputePerThread[i],
                    nbToCompute,
                    startingIndex,
                    &finished,
                    &result,
                    this);
        threads[i] = thread;
        startingIndex += nbToComputePerThread[i];
    }

    // Attente des threads
    for (unsigned i = 0; i < nbThreads; ++i) {
        threads[i]->join();
    }

    // Suppression des différents pointeurs sur les threads
    for (unsigned i = 0; i < nbThreads; ++i) {
        delete threads[i];
    }

    return result;
}
