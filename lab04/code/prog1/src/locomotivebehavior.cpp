//    ___  _________    ___  ___  ___   __ //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  | / / //
//  / ___/ /__/ /_/ / / __// // / __/ / /  //
// /_/   \___/\____/ /____/\___/____//_/   //
//

#include "locomotivebehavior.h"
#include "ctrain_handler.h"
#include "sharedsection.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */

    size_t i = 0;

    // Attend que la loco passe sur les differents contacts de son parcours.
    while (true) {
        size_t pointToCheck = (i + 2) % parcours.size();

        attendre_contact(parcours[i]);
        // Entrée section partagée
        if (parcours.at(pointToCheck) == shared.at(0)) {
            diriger_aiguillage(aiguillages[0].first, aiguillages[0].second, 0);
            sharedSection->access(loco);
        }
        // Sortie section partagée
        else if(parcours.at(i) == shared.at(shared.size() - 1)) {
            // TODO: la rouge se remet pas bien à la sortie de la section
            // Marche que une fois
            diriger_aiguillage(aiguillages[1].first, aiguillages[1].second, 0);
            sharedSection->leave(loco);
        }
        afficher_message(qPrintable(QString("Loco %1 a atteint le contact %2.").arg(loco.numero()).arg(parcours.at(i))));
        loco.afficherMessage(QString("Atteint contact %1.").arg(parcours.at(i)));

        // Les deux s'arrêtent en même temps...
        /* if (parcours[parcours.size() - 1]) {
            loco.arreter();
            loco.afficherMessage(QString("Arret au dernier point [%1] du parcours .").arg(parcours.at(i)));
        } */

        i++;
        if (i == parcours.size())
            i = 0;
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
