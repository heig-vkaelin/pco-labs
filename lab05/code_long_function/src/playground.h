#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcohoaremonitor.h>

#include "kid.h"

class Playground : PcoHoareMonitor
{
public:
    ///
    /// \brief Constructeur
    /// \param nbKidsToLeave Nombre d'enfants permettant de relâcher la moitié d'entre eux
    ///
    Playground(unsigned int nbKidsToLeave) : nbKidsWaiting(0),
        nbKidsToLeave(nbKidsToLeave), nbKidsToFree(nbKidsToLeave / 2) {}

    ///
    /// \brief Fonction bloquante jusqu'à ce que le bon nombre d'enfants soit atteint
    /// \param kid un enfant qui doit pouvoir attendre et repartir
    ///
    /// Cette fonction doit bloquer les enfants jusqu'à ce que le bon nombre d'entre
    /// eux soient présents. A ce moment-là la moitié des enfants présents doivent
    /// repartir.
    /// Attention, avant de bloquer le thread il faut appeler kid.startWaiting(),
    /// et après être reparti il faut appeler kid.endWaiting()
    ///
    void play(Kid &kid)
    {
        monitorIn();
        nbKidsWaiting++;
        if (nbKidsWaiting == nbKidsToLeave) {
            nbKidsWaiting -= nbKidsToFree;
            for (unsigned i = 0; i < nbKidsToFree; ++i)
                signal(cond);
        }

        monitorOut();
        kid.startWaiting();
        monitorIn();
        wait(cond);
        monitorOut();
        kid.endWaiting();
    }

private:
    unsigned nbKidsWaiting;
    unsigned nbKidsToLeave;
    unsigned nbKidsToFree;
    Condition cond;
};

#endif // PLAYGROUND_H
