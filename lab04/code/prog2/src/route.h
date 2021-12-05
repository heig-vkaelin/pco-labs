#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

class Route
{
    using RailwaySwitch = std::pair<int, int>;
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
    std::vector<RailwaySwitch> railwaySwitches;

    int contactRequestShared, contactStartShared, contactEndShared;

    int contactRequestSharedInversed, contactStartSharedInversed, contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
