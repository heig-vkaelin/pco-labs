//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//

#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */

    // Vous pouvez appeler les méthodes de la section partagée comme ceci :
    //sharedSection->request(loco);
    //sharedSection->getAccess(loco);
    //sharedSection->leave(loco);

    // Attend que la loco passe sur les differents contacts de son parcours.
    for (size_t i = 0; i < parcours.size(); ++i) {
        attendre_contact(parcours[i]);
        afficher_message(qPrintable(QString("Loco %1 a atteint le contact %2.").arg(loco.numero()).arg(parcours.at(i))));
        loco.afficherMessage(QString("Atteint contact %1.").arg(parcours.at(i)));

        // Les deux s'arrêtent en même temps...
        /* if (parcours[parcours.size() - 1]) {
            loco.arreter();
            loco.afficherMessage(QString("Arret au dernier point [%1] du parcours .").arg(parcours.at(i)));
        } */
    }

    // while (1) {}
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
