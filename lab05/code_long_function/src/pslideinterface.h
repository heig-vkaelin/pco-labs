/**
  \file bikinginterface.h
  \author Yann Thoma
  \date 05.05.2011

  Ce fichier contient la définition de la classe permettant d'interfacer
  l'application graphique avec les différents threads de l'application.
  */

#ifndef BIKINGINTERFACE_H
#define BIKINGINTERFACE_H

#include <QObject>

#include "mainwindow.h"

/**
  \brief Classe permettant aux threads d'interagir avec la partie graphique.

  Cette classe permet d'interagir avec la partie graphique de l'application.
  Elle utilise le concept de signaux et slots afin d'envoyer les commandes
  à l'interface graphique, et peut donc être appelée par des threads, ce qui
  ne serait pas possible sinon.

  Les commandes permettent de:
  \li afficher un message dans un parmi plusieurs consoles
  \li définir le nombre d'enfants présents sur le toboggan
  \li faire se déplacer un enfant entre deux marches
  \li A savoir que la rampe du tobogan est considérée comme une marche.
  */
class PSlideInterface : public QObject
{
    Q_OBJECT

public:

    /**
      \brief Constructeur simple.

      Une seule interface peut être partagée par plusieurs threads.
      */
    PSlideInterface();

    /**
      \brief Initialisation à exécuter en début d'application.

      Fonction statique devant être appelée avant toute construction d'objet
      de type PSlideInterface.
      \param nbConsoles Nombre de consoles d'affichage
      \param nbStep Nombre de marches où peuvent se situer un enfant
      \param nbKids Nombre d'enfants
      */
    static void initialize(unsigned int nbConsoles, unsigned int nbStep, unsigned nbKids);

    /**
      \brief Fonction permettant d'afficher du texte dans une console.
      \param consoleId Identifiant de la console. Attention, doit être compris
             entre 0 et nombre_de_consoles-1.
      \param text Texte à ajouter à la console.
      */
    void consoleAppendText(unsigned int consoleId,QString text);


    /**
      \brief Place un enfant sur une marche

      Fonction permettant de définir la position d'un enfant.
      Ne doit être appelé que depuis le main() avant le lancement de la
      boucle de gestion des événements. Elle ne doit pas être appelée
      de nouveau.
      \param step Identifiant de la marche. Attention, doit être compris entre
             0 et sm_nbSteps.
      \param KidId Identifiant dde l'enfant.
      */
    void setInitKid(unsigned int step, unsigned int KidId);

    /**
      \brief Déplace un enfant d'une marche à une autre.

      Fonction permettant de visualiser le déplacement d'un enfant d'une marche à
      l'autre. Le déplacement prend un certain nombre de millisecondes, et la
      fonction retourne lorsque le déplacement est terminé.
      \param KidId Identifiant de l'enfant empruntant la marche
      \param step2 Identifiant de la marche d'arrivée. Attention, doit être compris
             entre 0 et sm_nbSteps.
      \param ms Nombre de millisecondes de l'animation.
      */
    void travel(unsigned int KidId, unsigned int step2, unsigned int ms);

    /**
      \brief Déplace un enfant d'une marche à l'autre.

      Fonction permettant de visualiser le déplacement d'un enfant d'une marche à
      l'autre. Le déplacement prend un certain nombre de millisecondes, et la
      fonction retourne lorsque le déplacement est terminé.
      \param ms Nombre de millisecondes de l'animation.
      */
    void goToPlayground(unsigned int KidId,unsigned int ms);

    //! Retourne le nombre de marches du toboggan
    unsigned int nbSteps();

    //! Retourne le nombre de marches du toboggan + 1, ce qui inclue la rampe du toboggan
    //! qui est considérée comme un emplacement à protéger
    unsigned int nbPlaces();

private:

    //! Indique si la fonction d'initialisation a déjà été appelée
    static bool sm_didInitialize;
    //! Fenêtre principale de l'application
    static MainWindow *mainWindow;

    //! Nombre de marches du toboggan
    static unsigned int sm_nbSteps;

signals:
    /**
      Signal envoyé à la fenêtre principale pour l'ajout d'un message
      \param consoleId Identifiant de la console. Attention, doit être compris
             entre 0 et nombre_de_consoles-1.
      \param text Texte à ajouter à la console.
      */
    void sig_consoleAppendText(unsigned int consoleId,QString text);

    /**
      Signal envoyé à la fenêtre principale pour déplacer un enfant d'une marche à
      l'autre.
      \param kidId Identifiant de l'enfant empruntant la marche
      \param step2 Identifiant de la marche d'arrivée. Attention, doit être compris
             entre 0 et nombre_de_marche. Le step2 d'identifiant nombre_de_sites ??????
             correspond au local de maintenance.
      \param ms Nombre de millisecondes de l'animation.
      */
    void sig_travel(unsigned int kidId,unsigned int step2,unsigned int ms);

    /**
      Signal envoyé à la fenêtre principale pour déplacer un enfant d'une marche à
      l'autre.
      \param kidId Identifiant de l'enfant empruntant la marche
      \param ms Nombre de millisecondes de l'animation.
      */
    void sig_goToPlayground(unsigned int kidId,unsigned int ms);

};

#endif // BIKINGINTERFACE_H
