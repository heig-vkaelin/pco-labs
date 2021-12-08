/**
  \file route.cpp
  \author Valentin Kaelin & Lazar Pavicevic
  \date 12.12.2021
  \brief Implémentation de la classe Route représentant le parcours d'une locomotive.
  Les points de contacts utilisés pour le tronçon partagé sont calculés à partir
  des points du parcours de la locomotive et ceux de la section partagée.
*/

#include <algorithm>
#include "ctrain_handler.h"
#include "route.h"

using namespace std;

Route::Route(const vector<int>& route, const vector<int>& shared,
             const vector<RailwaySwitch>& railwaySwitches)
    : railwaySwitches(railwaySwitches), inversed(false)
{
    if (!route.size() || !shared.size())
        throw runtime_error("Le parcours et la section partagée ne peuvent pas être vides!");

    // Fin d'un tour
    contactEndTurn = route.at(route.size() - 1);

    auto sectionStart = find(route.begin(), route.end(), shared.at(0));
    auto sectionEnd = find(route.begin(), route.end(), shared.at(shared.size() - 1));

    // Problème: la section partagée n'est pas dans le parcours de la loco
    if (sectionStart == route.end() || sectionEnd == route.end())
        throw runtime_error("Le parcours ne contient pas la section partagée!");

    // Sens normal
    contactRequestShared = *prev(sectionStart, 3);
    contactStartShared = *prev(sectionStart, 2);
    contactEndShared = *next(sectionEnd, 1);

    // Sens inversé
    contactRequestSharedInversed = *next(sectionEnd, 3);
    contactStartSharedInversed = *next(sectionEnd, 2);
    contactEndSharedInversed = *prev(sectionStart, 1);
}

int Route::getSectionRequest() {
    return inversed ? contactRequestSharedInversed : contactRequestShared;
}

int Route::getSectionStart() {
    return inversed ? contactStartSharedInversed : contactStartShared;
}

int Route::getSectionEnd() {
    return inversed ? contactEndSharedInversed : contactEndShared;
}

int Route::getTurnEnd() {
    return contactEndTurn;
}

void Route::inverse() {
    inversed = !inversed;
}

bool Route::isInversed() {
    return inversed;
}

void Route::applyRailwaySwitches() {
    for (RailwaySwitch railwaySwitch : railwaySwitches) {
        diriger_aiguillage(railwaySwitch.first, railwaySwitch.second, 0);
    }
}
