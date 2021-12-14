#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <pcosynchro/pcohoaremonitor.h>

class Playground : PcoHoareMonitor
{
public:
    ///
    /// \brief Constructeur
    /// \param nbKidsToLeave Nombre d'enfants permettant de relâcher la moitié d'entre eux
    ///
    Playground(unsigned int nbKidsToLeave) : initialNbKidsToLeave(nbKidsToLeave),
        nbKidsToLeave(nbKidsToLeave)
    {
        // TODO
    }

    ///
    /// \brief Fonction bloquante jusqu'à ce que le bon nombre d'enfants soit atteint
    ///
    /// Cette fonction doit bloquer les enfants jusqu'à ce que le bon nombre d'entre
    /// eux soient présents. A ce moment-là la moitié des enfants présents doivent
    /// repartir.
    ///
    void play()
    {
        // TODO
        monitorIn();
        nbKidsToLeave--;
        if (nbKidsToLeave > 0) {
            wait(cond);
        }
        signal(cond);

        // On le fait pour chaque kid...
        nbKidsToLeave = initialNbKidsToLeave;

        monitorOut();
    }

private:
    unsigned initialNbKidsToLeave;
    unsigned nbKidsToLeave;
    Condition cond;
};

#endif // PLAYGROUND_H
