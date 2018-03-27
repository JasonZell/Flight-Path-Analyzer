#include "AirportNode.h"

AirportNode::AirportNode()
{
    portIATA ="";
    portICAO ="";
    portCity = "";
    portCountry ="";

}
AirportNode::AirportNode(const AirportNode& a)
{
    if(this != &a)
    {
        AirportNode(a).swap(*this);
    }
}


AirportNode::~AirportNode()
{
    this->portCity = "";
    this->portCountry = "";
    this->portIATA= "";
    this->portICAO ="";
}

AirportNode& AirportNode::operator=(const AirportNode& a)
{
    //https://en.wikibooks.org/wiki/More_C++_Idioms/Copy-and-swap
    if(this != &a)
    {
        AirportNode(a).swap(*this);
    }

    return *this;
}

void AirportNode::copy( AirportNode& a)
{
    this->portCity = a.getPortCity();
    this->portCountry = a.getPortCountry();
    this->portIATA = a.getPortIATA();
    this->portICAO = a.getPortICAO();
}

void AirportNode::swap(AirportNode& a) throw()
{
    std::swap(this->portCity, a.getPortCity());
    std::swap(this->portCountry, a.getPortCountry());
    std::swap(this->portIATA, a.getPortIATA());
    std::swap(this->portICAO, a.getPortICAO());
}

void AirportNode::setPortIATA(string& s)
{
    this->portIATA = s;
}

void AirportNode::setPortICAO(string& s)
{
    this->portICAO = s;
}

void AirportNode::setPortCity(string& s )
{
    this->portCity = s;
}

void AirportNode::setPortCountry(string& s)
{
    this->portCountry = s;
}

string& AirportNode::getPortIATA() const
{
    return portIATA;
}

string& AirportNode::getPortICAO() const
{
    return this->portICAO;
}

string& AirportNode::getPortCity() const
{
    return this->portCity;
}

string& AirportNode::getPortCountry() const
{
    return this->portCountry;
}
