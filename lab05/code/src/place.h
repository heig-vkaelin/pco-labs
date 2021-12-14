#ifndef PLACE_H
#define PLACE_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>


class Place
{
public:
    ///
    /// \brief Constructeur
    /// \param nbMaxPeople Nombre maximal de personnes sur cet emplacement
    ///
    Place(unsigned int nbMaxPeople) : nbMaxPeople(nbMaxPeople), nbWaiting(0),
        nbCurrentPeople(0), mutex(), cond()
    {
        // TODO
    }

    ///
    /// \brief Fonction permettant de gérer l'accès à l'emplacement.
    ///
    void access()
    {
        // TODO
        mutex.lock();
        //nbWaiting++;
        while (nbCurrentPeople == nbMaxPeople) {
            cond.wait(&mutex);
        }
        //nbWaiting--;
        nbCurrentPeople++;

        mutex.unlock();
    }

    ///
    /// \brief Fonction permettant de quitter la marche proprement
    ///
    void leave()
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
