#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

using RailwaySwitch = std::pair<int, int>;

class Route
{
public:
    Route(const std::vector<int>& route, const std::vector<int>& shared,
          const std::vector<RailwaySwitch>& railwaySwitches);

    int getSectionStart();

    int getSectionEnd();

    int getTurnEnd();

    void inverse();

    void applyRailwaySwitches();

private:
    std::vector<std::pair<int, int>> railwaySwitches;

    int contactStartShared;
    int contactEndShared;

    int contactStartSharedInversed;
    int contactEndSharedInversed;

    int contactEndTurn;

    bool inversed;
};

#endif // ROUTE_H
