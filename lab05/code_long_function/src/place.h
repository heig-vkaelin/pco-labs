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
    Place(unsigned int nbMaxPeople): nbMaxPeople(nbMaxPeople), nbWaiting(0),
        nbCurrentPeople(0), mutex(), cond()
    {

    }

    ///
    /// \brief Fonction permettant de gérer l'accès à l'emplacement.
    /// \param kid un enfant qui doit pouvoir attendre et repartir
    ///
    /// Attention, avant de bloquer le thread il faut appeler kid.startWaiting(),
    /// et après être reparti il faut appeler kid.endWaiting()
    ///
    void access(Kid &kid)
    {
        // TODO
        mutex.lock();
        //nbWaiting++;
        while (nbCurrentPeople == nbMaxPeople) {
            kid.startWaiting();
            cond.wait(&mutex);
            kid.endWaiting();
        }
        //nbWaiting--;
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
        // TODO
        mutex.lock();
        //if (nbWaiting > 0) {

        if (nbCurrentPeople > 0) {
            cond.notifyOne();
            nbCurrentPeople--;
        }


        //} else {
        //    nbCurrentPeople--;
        //}


        mutex.unlock();
    }

private:
    // TODO
    unsigned nbMaxPeople;
    unsigned nbWaiting;
    unsigned nbCurrentPeople;
    PcoMutex mutex;
    PcoConditionVariable cond;
};

#endif // PLACE_H
