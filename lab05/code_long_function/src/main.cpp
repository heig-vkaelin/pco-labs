#include <iostream>

#include <QApplication>
#include <QCommandLineParser>


#include "pslideinterface.h"
#include "kid.h"
#include "place.h"
#include "playground.h"


constexpr int NBSTEPS = 4;
constexpr int NBKIDS = 6;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption stepNumber(QStringList() << "s" << "steps",
                                  QCoreApplication::translate("main",
                                       "Number or steps on toboggan, "
                                       "the slider is considered as a step, minimum 2"),
                                  QCoreApplication::translate("main", "steps"));

    QCommandLineOption kidNumber(QStringList() << "k" << "kids",
                                 QCoreApplication::translate("main",
                                                             "Number of kids on the toboggan, minimum 1"),
                                 QCoreApplication::translate("main", "kids"));

    parser.addOption(stepNumber);
    parser.addOption(kidNumber);
    parser.process(a);

    int nbSteps = parser.value(stepNumber).toInt();
    int nbKids = parser.value(kidNumber).toInt();

     std::cout << "Number of steps from cmd line: " << nbSteps << std::endl;
     std::cout << "Number of kids from cmd line: " << nbKids << std::endl;

    if (nbKids < 1) {
        nbKids = NBKIDS;
        std::cout << "Number of kids passed in commande line argument invalid."
                     " Using default value:"<< nbKids << std::endl;
    }

    if (nbSteps < 2) {
        nbSteps = NBSTEPS;
        std::cout << "Number of steps passed in commande line argument invalid."
                     " Using default value:"<< nbSteps << std::endl;
    }

     /* Max number not checked */


    // Initialisation de la partie graphique de l'application
    PSlideInterface::initialize(nbKids,nbSteps, nbKids);
    // Création de l'interface pour les commandes à la partie graphique
    auto gui_interface = new PSlideInterface();

   // Création de steps
    std::vector<Place*> places(nbSteps + 1);

    for (int s=0; s < nbSteps + 1; s++) {
        // Les steps pairs accueillent une personne et les
        // steps impairs en accueillent deux
        places[s] = new Place((s % 2) + 1);
    }

    Playground playingPlace(nbKids / 2);

    std::vector<std::unique_ptr<PcoThread> > threads;

    // Création des enfants
    std::vector<Kid*> kids(nbKids);
    for (int t = 0; t < nbKids; t++) {
        // std::cout << "Created kid " << t << std::endl;
        kids[t] = new Kid(t, gui_interface, places, playingPlace);
        threads.emplace_back(std::make_unique<PcoThread>(&Kid::run, kids[t]));
    }


    // Attention, il est obligatoire d'exécuter l'instruction suivante.
    // C'est elle qui permet la gestion de la boucle des évévements de
    // l'application graphique.
    return a.exec();
}


