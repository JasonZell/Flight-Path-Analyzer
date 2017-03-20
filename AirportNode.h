#ifndef AIRPORTNODE_H
#define AIRPORTNODE_H

#include <string>

using std::string;

class AirportNode
{
public:
    AirportNode();
    AirportNode(const AirportNode&);
    ~AirportNode();
    AirportNode& operator=(const AirportNode&);

    void setPortIATA(string& );
    void setPortICAO(string&);
    void setPortCity(string&);
    void setPortCountry(string&);

    string& getPortIATA();
    string& getPortICAO();
    string& getPortCity();
    string& getPortCountry();


private:
    void copy( AirportNode&);
    void swap(AirportNode&) throw();

    string portIATA;
    string portICAO;
    string portCity;
    string portCountry;

};

inline bool operator<(const AirportNode &n1, const AirportNode &n2)
{
    return n1.getPortIATA() < n2.getPortIATA();
}

#endif // AIRPORTNODE_H
