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
    Place(unsigned int nbMaxPeople)
    {
        // TODO
    }

    ///
    /// \brief Fonction permettant de gérer l'accès à l'emplacement.
    ///
    void access()
    {
        // TODO
    }

    ///
    /// \brief Fonction permettant de quitter la marche proprement
    ///
    void leave()
    {
        // TODO
    }

private:
    // TODO
};

#endif // PLACE_H
