#include "RouteData.h"

RouteData::RouteData()
{
    distance = UNDEFINED;
    airlineList = new std::vector<std::string>();
}

RouteData::~RouteData()
{
    distance = UNDEFINED;
    delete airlineList;
}

void RouteData::swap(RouteData& r) throw()
{
    std::swap(this->distance, r.getDist());
    std::swap(this->airlineList, r.getAirlineList());
}

RouteData::RouteData(const RouteData& r)
{
    if(this != &r)
    {
        RouteData(r).swap(*this);
    }
}

RouteData& RouteData::operator=(const RouteData& r)
{
    if(this != &r)
    {
        RouteData(r).swap(*this);
    }

    return *this;
}

void RouteData::setDist(double d)
{
    distance = d;
}

double& RouteData::getDist()
{
    return distance;
}

void RouteData::addAirlineCode(std::string s)
{
    airlineList->push_back(s);
}

std::vector<std::string>*& RouteData::getAirlineList()
{
    return airlineList;
}
