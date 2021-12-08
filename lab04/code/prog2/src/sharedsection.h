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
    SharedSection() : mutex(1), waiting(0), occupied(false),
        nbWaiting(0), locoARequest(false), locoBRequest(false) {
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
        if (locoId == LocoId::LA) {
            locoARequest = true;
            locoAEntry = entryPoint;
        } else if (locoId == LocoId::LB) {
            locoBRequest = true;
            locoBEntry = entryPoint;
        }
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
        mutex.acquire();
        if (!canAccess(locoId)) {
            nbWaiting++;
            loco.arreter();
            mutex.release();
            afficher_message(qPrintable(QString("The engine no. %1 is waiting for the shared section.").arg(loco.numero())));
            waiting.acquire();
            mutex.acquire();
            loco.demarrer();
        } else {
            occupied = true;
        }

        if (locoId == LocoId::LA) {
            locoARequest = false;
        } else if (locoId == LocoId::LB) {
            locoBRequest = false;
        }

        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
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

        if (nbWaiting == 0) {
            occupied = false;
        } else {
            nbWaiting--;
            locoARequest = false;
            locoBRequest = false;
            waiting.release();
        }
        mutex.release();
    }

private:
    PcoSemaphore mutex, waiting;
    bool occupied;
    int nbWaiting;

    EntryPoint locoAEntry, locoBEntry;
    bool locoARequest, locoBRequest;

    bool canAccess(LocoId locoId) {
        if (occupied) return false;

        if (!locoARequest || !locoBRequest) return true;

        return locoId == LocoId::LA ? locoAEntry == locoBEntry : locoAEntry != locoBEntry;
    }
};

#endif // SHAREDSECTION_H
