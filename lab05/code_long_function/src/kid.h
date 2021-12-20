/******************************************************************************
  \file main.cpp
  \author Yann Thoma
  \date 16.02.2018

  Ce fichier propose un squelette pour l'application de modélisation de toboggan.
  Il est évident qu'il doit être grandement modifié pour respecter la donnée,
  mais vous y trouvez des exemples d'appels de fonctions de l'interface.
  ****************************************************************************/



#ifndef KID_H
#define KID_H

#include <pcosynchro/pcothread.h>

#include "pslideinterface.h"

class Place;
class Playground;

/**
  Tâche illustrant les différents appels pouvant être faits à l'interface
  graphique.
  */
class Kid
{
public:
    Kid(unsigned int id, PSlideInterface *interface, std::vector<Place *> &liststeps, Playground &playingPlace)
        : id(id), gui_interface(interface), m_listSteps(liststeps), m_playingPlace(playingPlace) {}

    void run();

    void startWaiting();
    void endWaiting();


private:
    unsigned int id;
    PSlideInterface* gui_interface;
    std::vector<Place *> m_listSteps;
    Playground& m_playingPlace;
};


#endif // KID_H


