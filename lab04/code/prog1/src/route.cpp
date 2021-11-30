#include <algorithm>
#include "ctrain_handler.h"
#include "route.h"

using namespace std;

Route::Route(vector<int> route, int contactStartShared, int contactEndShared,
             int contactStartSharedInversed, int contactEndSharedInversed,
             vector<RailwaySwitch>& railwaySwitches)
    : route(route), railwaySwitches(railwaySwitches),
      contactStartShared(contactStartShared), contactEndShared(contactEndShared),
      contactStartSharedInversed(contactStartSharedInversed),
      contactEndSharedInversed(contactEndSharedInversed), inversed(false)
{
    size_t size = route.size();
    if (size < 0)
        throw runtime_error("Le parcours ne peut pas être vide!");

    contactEndTurn = route.at(size - 1);
}

int Route::getContact(int index) {
    return route.at(index);
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
    std::reverse(route.begin(), route.end() - 1);
}

void Route::applyRailwaySwitches() {
    for(auto railwaySwitch : railwaySwitches) {
        diriger_aiguillage(railwaySwitch.first, railwaySwitch.second, 0);
    }
}
