// Auteurs: Valentin Kaelin & Alexandre Jaquier

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
    Place(unsigned int nbMaxPeople) : nbMaxPeople(nbMaxPeople),
        nbCurrentPeople(0), mutex(), cond() {}

    ///
    /// \brief Fonction permettant de gérer l'accès à l'emplacement.
    ///
    void access()
    {
        mutex.lock();
        while (nbCurrentPeople == nbMaxPeople) {
            cond.wait(&mutex);
        }
        nbCurrentPeople++;
        mutex.unlock();
    }

    ///
    /// \brief Fonction permettant de quitter la marche proprement
    ///
    void leave()
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
