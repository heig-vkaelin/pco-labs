#include <iostream>

#include <QMessageBox>
#include <QThread>

#include "pslideinterface.h"
#include "display.h"

using namespace std;


bool PSlideInterface::sm_didInitialize   = false;
MainWindow *PSlideInterface::mainWindow  = nullptr;
unsigned int PSlideInterface::sm_nbSteps = 0;

PSlideInterface::PSlideInterface()
{
    if (!sm_didInitialize) {
        cout << "Vous devez appeler PSlideInterface::initialize()" << endl;
        QMessageBox::warning(nullptr,"Erreur","Vous devez appeler "
                             "PSlideInterface::initialize() avant de créer un "
                             "objet PSlideInterface");
        exit(-1);
    }

    if (!QObject::connect(this,
                          SIGNAL(sig_consoleAppendText(unsigned int,QString)),
                          mainWindow,
                          SLOT(consoleAppendText(unsigned int,QString)),Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }
    if (!QObject::connect(this,
                          SIGNAL(sig_travel(unsigned int,unsigned int,unsigned int)),
                          mainWindow,
                          SLOT(travel(unsigned int,unsigned int,unsigned int)),Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }
    if (!QObject::connect(this,
                          SIGNAL(sig_goToPlayground(uint,uint)),
                          mainWindow,
                          SLOT(goToPlayground(unsigned int,unsigned int)),Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }
}


unsigned int PSlideInterface::nbSteps()
{
    return sm_nbSteps;
}

unsigned int PSlideInterface::nbPlaces()
{
    return sm_nbSteps + 1;
}

#include <QTest>

void PSlideInterface::travel(unsigned int KidId, unsigned int step2,
                             unsigned int ms)
{
    emit sig_travel(KidId,step2,ms);
    getKid(KidId)->sem.acquire();
       //QTest::qSleep(ms);
}

void PSlideInterface::goToPlayground(unsigned int KidId, unsigned int ms)
{
    emit sig_goToPlayground(KidId,ms);
    getKid(KidId)->sem.acquire();
    //QTest::qSleep(ms);
}


void PSlideInterface::consoleAppendText(unsigned int consoleId,QString text) {
    emit sig_consoleAppendText(consoleId,std::move(text));
}


void PSlideInterface::setInitKid(unsigned int step, unsigned int KidId) {
    mainWindow->setKid(step, KidId);
}

void PSlideInterface::initialize(unsigned int nbConsoles, unsigned int nbStep, unsigned nbKids)
{
    if (sm_didInitialize) {
        cout << "Vous devez ne devriez appeler PSlideInterface::initialize()"
             << " qu'une seule fois" << endl;
        QMessageBox::warning(nullptr,"Erreur","Vous ne devriez appeler "
                             "PSlideInterface::initialize() "
                             "qu'une seule fois");
        return;
    }

    sm_nbSteps = nbStep;

    mainWindow= new MainWindow(nbConsoles, nbStep, nbKids, nullptr);
    mainWindow->show();
    sm_didInitialize=true;
}
