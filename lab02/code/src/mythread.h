#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QString>
#include <QCryptographicHash>
#include <QVector>
#include <QDebug>

#include <pcosynchro/pcothread.h>
#include "threadmanager.h"

/**
 * @brief runComputation tâche qui s'occupe de trouver le hash md5 sur une partie des hashs totaux
 * @param charset QString tous les caractères possibles composant le mot de passe
 * @param salt QString sel qui permet de modifier dynamiquement le hash
 * @param hash QString hash à reverser
 * @param nbChars taille du mot de passe
 * @param nbToCompute nombre de hashs à tester pour ce thread spécifique
 * @param totalToCompute nombre de hashs à tester entre tous les threads
 * @param startingIndex index de départ de la recherche
 * @param finished valeur signifiant si le hash a été trouvé ou pas par un thread
 * @param result le hash trouvé par le thread, ou la chaîne inchangée autrement
 * @param manager instance de la classe appellant la fonction runComputation
 *
 * La fonction communique avec le code appellant en modifiant les valeurs des
 * paramètres nbCOmputed et result qui sont passés via des pointeurs.
 */
void runComputation(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        long long unsigned nbToCompute,
        long long unsigned totalToCompute,
        long long unsigned startingIndex,
        bool* finished,
        QString* result,
        ThreadManager* manager
);

#endif // MYTHREAD_H
