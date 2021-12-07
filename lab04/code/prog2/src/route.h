#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

class Route
{
    using RailwaySwitch = std::pair<int, int>;
public:
    Route(const std::vector<int>& route, const std::vector<int>& shared,
          const std::vector<RailwaySwitch>& railwaySwitches);

    int getSectionRequest();

    int getSectionStart();

    int getSectionEnd();

    int getTurnEnd();

    void inverse();

    bool isInversed();

    void applyRailwaySwitches();

private:
    std::vector<RailwaySwitch> railwaySwitches;

    int contactRequestShared, contactStartShared, contactEndShared;

    int contactRequestSharedInversed, contactStartSharedInversed, contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
