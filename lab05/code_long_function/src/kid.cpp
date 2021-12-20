#include "kid.h"

#include "place.h"
#include "playground.h"


void Kid::run() {
    unsigned int t = id;
    qsrand(t);

    //Affichage d'un message
    gui_interface->consoleAppendText(t,"Salut");

    while(true) {
        {
            for(unsigned int step = 0; step < gui_interface->nbPlaces(); step++) {
                // Déplacement d'un enfant
                m_listSteps[step]->access(*this);
                if (step != 0) {
                    m_listSteps[step - 1]->leave(*this);
                }
                gui_interface->travel(t,           // ID de l'enfant
                                      step,        // place d'arrivée
                                      t + 1 * 1000);   // Temps en millisecondes
                PcoThread::usleep((rand() % 30 ) * 100000 % 1000000);
            }
        }
        m_listSteps[gui_interface->nbPlaces() - 1]->leave(*this);

        // Va à la place de jeu
        gui_interface->goToPlayground(t, (t + 1) * 1000);

        m_playingPlace.play(*this);

        // Est prêt à repartir sur le toboggan
    }
}


void Kid::startWaiting()
{
    gui_interface->consoleAppendText(id, QString("Kid %1 waits").arg(id));
}

void Kid::endWaiting()
{
    gui_interface->consoleAppendText(id, QString("Kid %1 end waiting").arg(id));
}
