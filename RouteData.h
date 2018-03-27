#ifndef ROUTEDATA_H
#define ROUTEDATA_H

#include <vector>
#include <string>
class RouteData
{
    const double UNDEFINED = -2;

    public:
        RouteData();
        ~RouteData();
        RouteData(const RouteData&);
        RouteData& operator=(const RouteData&);

        void setDist(double);
        double& getDist();

        void addAirlineCode(std::string s);
        std::vector<std::string>*& getAirlineList();

    private:

        double distance;
        std::vector<std::string> *airlineList; //2 letter airline IATA code

        void swap(RouteData&) throw();
};

#endif // ROUTEDATA_H
