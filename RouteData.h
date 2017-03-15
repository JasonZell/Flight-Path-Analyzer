#ifndef ROUTEDATA_H
#define ROUTEDATA_H

#include <vector>
#include <string>
class RouteData
{
    public:
        RouteData();

    private:

        double distance;
        std::vector<string> airlineList;
};

#endif // ROUTEDATA_H
