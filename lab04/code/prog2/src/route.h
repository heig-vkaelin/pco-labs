#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

using RailwaySwitch = std::pair<int, int>;

class Route
{
public:
    Route(std::vector<int> route,
          int contactRequestShared, int contactStartShared, int contactEndShared,
          int contactRequestSharedInversed, int contactStartSharedInversed, int contactEndSharedInversed,
          std::vector<RailwaySwitch>& railwaySwitches);

    int getContact(int index);

    int getSectionRequest();

    int getSectionStart();

    int getSectionEnd();

    int getTurnEnd();

    void inverse();

    bool isInversed();

    void applyRailwaySwitches();

private:
    std::vector<int> route;
    std::vector<std::pair<int, int>> railwaySwitches;

    int contactRequestShared, contactStartShared, contactEndShared;

    int contactRequestSharedInversed, contactStartSharedInversed, contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
