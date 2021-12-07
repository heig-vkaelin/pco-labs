//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//                                         //


#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run() {
    // Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */
    EntryPoint entryPoint;
    int nbTurns = NB_TURNS;

    while(true) {
        entryPoint = route.isInversed() ? EntryPoint::EB : EntryPoint::EA;

        // Request section partagée
        attendre_contact(route.getSectionRequest());
        sharedSection->request(loco, locoId, entryPoint);
        loco.afficherMessage("Demande d'entrée en trançon partagé!");

        // Accès section partagée
        attendre_contact(route.getSectionStart());
        sharedSection->getAccess(loco, locoId);
        route.applyRailwaySwitches();
        loco.afficherMessage("Entrée trançon partagé!");

        // Sortie section partagée
        attendre_contact(route.getSectionEnd());
        sharedSection->leave(loco, locoId);
        loco.afficherMessage("Sortie trançon partagé!");

        // Fin du tour
        attendre_contact(route.getTurnEnd());
        nbTurns--;
        if (!nbTurns) {
            inverse();
            nbTurns = NB_TURNS;
        }
    }

}

void LocomotiveBehavior::printStartMessage() {
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage() {
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}

void LocomotiveBehavior::inverse() {
    loco.arreter();
    loco.inverserSens();
    route.inverse();
    loco.demarrer();
    qDebug() << "[INVERSE] La loco" << loco.numero() << "a changé de sens";
    loco.afficherMessage("J'ai changé de sens!");
}
