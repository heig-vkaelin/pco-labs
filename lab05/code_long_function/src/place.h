// Auteurs: Valentin Kaelin & Alexandre Jaquier

#ifndef PLACE_H
#define PLACE_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>

#include "kid.h"

class Place
{
public:
    ///
    /// \brief Constructeur
    /// \param nbMaxPeople Nombre maximal de personnes sur cet emplacement
    ///
    Place(unsigned int nbMaxPeople): nbMaxPeople(nbMaxPeople),
        nbCurrentPeople(0), mutex(), cond() {}

    ///
    /// \brief Fonction permettant de gérer l'accès à l'emplacement.
    /// \param kid un enfant qui doit pouvoir attendre et repartir
    ///
    /// Attention, avant de bloquer le thread il faut appeler kid.startWaiting(),
    /// et après être reparti il faut appeler kid.endWaiting()
    ///
    void access(Kid &kid)
    {
        // Permet de savoir si l'enfant doit appeller la fonction startWaiting ou non
        bool hasAlreadyWait = false;

        mutex.lock();
        while (nbCurrentPeople == nbMaxPeople) {
            if (!hasAlreadyWait) {
                mutex.unlock();
                kid.startWaiting();
                mutex.lock();
            }

            // On revérifie la condition
            if (nbCurrentPeople == nbMaxPeople) {
                cond.wait(&mutex);
            }

            hasAlreadyWait = true;
        }

        if (hasAlreadyWait) {
            mutex.unlock();
            kid.endWaiting();
            mutex.lock();
        }

        nbCurrentPeople++;

        mutex.unlock();
    }

    ///
    /// \brief Fonction permettant de quitter la marche proprement
    ///
    /// A priori il n'y aura pas besoin d'utiliser kid
    ///
    void leave(Kid &kid)
    {
        mutex.lock();
        if (nbCurrentPeople > 0) {
            cond.notifyOne();
            nbCurrentPeople--;
        }
        mutex.unlock();
    }

private:
    unsigned nbMaxPeople;
    unsigned nbCurrentPeople;
    PcoMutex mutex;
    PcoConditionVariable cond;
};

#endif // PLACE_H
