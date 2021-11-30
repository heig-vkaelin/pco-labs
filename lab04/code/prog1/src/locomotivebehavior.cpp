//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//

#include "locomotivebehavior.h"
#include "ctrain_handler.h"
#include "sharedsection.h"

void LocomotiveBehavior::run() {
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    int nbTurns = NB_TURNS;
    int contactIndex = 0;

    while(true) {
        int contact = route.getContact(contactIndex);
        attendre_contact(contact);

        contactIndex++;

        if (contact == route.getSectionStart()) {
            sharedSection->access(loco);
            route.applyRailwaySwitches();
            loco.afficherMessage("Entrée trançon partagé!");
        } else if (contact == route.getSectionEnd()) {
            sharedSection->leave(loco);
            loco.afficherMessage("Sortie trançon partagé!");
        } else if (contact == route.getTurnEnd()) {
            nbTurns--;
            if (!nbTurns) {
                inverse();
                nbTurns = NB_TURNS;
            }
            contactIndex = 0;
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
