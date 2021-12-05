//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//                                         //


#ifndef SHAREDSECTION_H
#define SHAREDSECTION_H

#include <QDebug>
#include <vector>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "sharedsectioninterface.h"

/**
 * @brief La classe SharedSection implémente l'interface SharedSectionInterface qui
 * propose les méthodes liées à la section partagée.
 */
class SharedSection final : public SharedSectionInterface
{
public:

    /**
     * @brief SharedSection Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    SharedSection() : mutex(1), waiting(0), occupied(false), nbWaiting(0), hasBeenRequested(false), priorityAccessAsked(false), priorities(2) {
        priorities[(int)LocoId::LA] = Priority::NONE;
        priorities[(int)LocoId::LB] = Priority::NONE;
    }

    /**
     * @brief request Méthode a appeler pour indiquer que la locomotive désire accéder à la
     * section partagée (deux contacts avant la section partagée).
     * @param loco La locomotive qui désire accéder
     * @param locoId L'identidiant de la locomotive qui fait l'appel
     * @param entryPoint Le point d'entree de la locomotive qui fait l'appel
     */
    void request(Locomotive& loco, LocoId locoId, EntryPoint entryPoint) override {
        afficher_message(qPrintable(QString("The engine no. %1 requested the shared section.").arg(loco.numero())));
        mutex.acquire();

        if (!hasBeenRequested) {
            hasBeenRequested = true;
            requested = {locoId, entryPoint};
            mutex.release();
            return;
        }

        // Même point d'entrée
        if (requested.second == entryPoint) {
            // priorities[(int)locoId] = locoId == LocoId::LA ? 1 : 0;
            priorities[(int)LocoId::LA] = Priority::HIGH;
            priorities[(int)LocoId::LB] = Priority::LOW;

            //if(locoId == LocoId::LA)
             //   priorityAccessAsked = true;

        }
        // Point d'entrée différent
        else {
            // priorities[(int)locoId] = locoId == LocoId::LB ? 1 : 0;
            priorities[(int)LocoId::LA] = Priority::LOW;
            priorities[(int)LocoId::LB] = Priority::HIGH;

            //if(locoId == LocoId::LB)
             //   priorityAccessAsked = true;
        }
        priorityAccessAsked = true;
        //requested = {locoId, entryPoint};
        hasBeenRequested = false;
        mutex.release();
    }

    /**
     * @brief getAccess Méthode à appeler pour accéder à la section partagée, doit arrêter la
     * locomotive et mettre son thread en attente si la section est occupée ou va être occupée
     * par une locomotive de plus haute priorité. Si la locomotive et son thread ont été mis en
     * attente, le thread doit être reveillé lorsque la section partagée est à nouveau libre et
     * la locomotive redémarée. (méthode à appeler un contact avant la section partagée).
     * @param loco La locomotive qui essaie accéder à la section partagée
     * @param locoId L'identidiant de la locomotive qui fait l'appel
     */
    void getAccess(Locomotive &loco, LocoId locoId) override {
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
        mutex.acquire();
        afficher_message(qPrintable(QString("occupied %1").arg(occupied)));
        afficher_message(qPrintable(QString("priority acces asked %1").arg(priorityAccessAsked)));
        if (occupied || (priorityAccessAsked && priorities[(int)locoId] != Priority::HIGH)) {
            nbWaiting++;
            loco.arreter();
            mutex.release();
            afficher_message(qPrintable(QString("The engine no. %1 is waiting for the shared section.").arg(loco.numero())));
            waiting.acquire();
            mutex.acquire();
            occupied = true;
            loco.demarrer();
        } else {
            occupied = true;
        }

        //if (priorities[(int)locoId] == Priority::HIGH)
        hasBeenRequested = false;
        priorityAccessAsked = false;
        afficher_message(qPrintable(QString("Fin get access loco %1").arg(loco.numero())));
        mutex.release();
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section
     * partagée. (reveille les threads des locomotives potentiellement en attente).
     * @param loco La locomotive qui quitte la section partagée
     * @param locoId L'identidiant de la locomotive qui fait l'appel
     */
    void leave(Locomotive& loco, LocoId locoId) override {
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
        mutex.acquire();

        // priorities[(int)locoId] = Priority::NONE;

        if (nbWaiting == 0) {
            occupied = false;
            mutex.release();
        } else {
            nbWaiting--;
            mutex.release();
            waiting.release();
        }
    }

    /* A vous d'ajouter ce qu'il vous faut */

private:
    // Méthodes privées ...
    // Attributes privés ...
    PcoSemaphore mutex, waiting;
    bool occupied;
    int nbWaiting;

    enum class Priority {
        NONE,
        LOW,
        HIGH
    };

    bool hasBeenRequested;
    bool priorityAccessAsked;
    std::vector<Priority> priorities;
    std::pair<LocoId, EntryPoint> requested;
};


#endif // SHAREDSECTION_H
