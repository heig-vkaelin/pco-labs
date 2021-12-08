/**
  \file route.h
  \author Valentin Kaelin & Lazar Pavicevic
  \date 12.12.2021
  \brief Déclaration de la classe Route.
  Cette classe s'occupe des différents événements arrivant au cours d'un parcours
  d'une locomotive:
    * Savoir quels points de contact utiliser pour faire une requête, un accès ou
      quitter le tronçon partagé.
    * Connaître le sens actuel de la locomotive
    * Savoir quels aiguillages modifiés
*/

#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

class Route
{
    using RailwaySwitch = std::pair<int, int>;
public:
    /**
     * Constructeur.
     * Initialise le parcours en précisant son itinéraire, son tronçon partagé et ses aiguillages
     * @param route points de contact du parcours
     * @param shared points de contact du tronçon partagé
     * @param railwaySwitches numéros des aiguillages et positions à appliquer
     */
    Route(const std::vector<int>& route, const std::vector<int>& shared,
          const std::vector<RailwaySwitch>& railwaySwitches);

    /**
     * Retourne le point de contact où réaliser la requête d'accès au tronçon partagé
     * @return le numéro du point de contact
     */
    int getSectionRequest();

    /**
     * Retourne le point de contact où réaliser l'accès au tronçon partagé
     * @return le numéro du point de contact
     */
    int getSectionStart();

    /**
     * Retourne le point de contact de sortie du tronçon partagé
     * @return le numéro du point de contact
     */
    int getSectionEnd();

    /**
     * Retourne le point de contact de fin d'un tour de parcours
     * @return le numéro du point de contact
     */
    int getTurnEnd();

    /**
     * Informe que la locomotive a changé de sens
     */
    void inverse();

    /**
     * Vérifie si la locomotive se déplace dans son sens initial ou inverse
     * @return true si la locomotive a changé de sens, false sinon
     */
    bool isInversed();

    /**
     * Applique les positions souhaitées des différents aiguillages pour le bon
     * parcours du tronçon partagé
     */
    void applyRailwaySwitches();

private:
    std::vector<RailwaySwitch> railwaySwitches;

    int contactRequestShared, contactStartShared, contactEndShared;

    int contactRequestSharedInversed, contactStartSharedInversed, contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
