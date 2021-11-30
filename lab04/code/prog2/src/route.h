#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

using RailwaySwitch = std::pair<int, int>;

class Route
{
public:
    Route(std::vector<int> route, int contactStartShared, int contactEndShared,
          int contactStartSharedInversed, int contactEndSharedInversed,
          std::vector<RailwaySwitch>& railwaySwitches);

    int getContact(int index);

    int getSectionStart();

    int getSectionEnd();

    int getTurnEnd();

    void inverse();

    void applyRailwaySwitches();

private:
    std::vector<int> route;
    std::vector<std::pair<int, int>> railwaySwitches;

    int contactStartShared;
    int contactEndShared;

    int contactStartSharedInversed;
    int contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
