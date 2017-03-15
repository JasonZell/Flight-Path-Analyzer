/* Flight Path Analyzer
 *
 * The purpose of this program is to create XML data file from OpenFlight data files.

 * Contributor: Chang Zhang, Ryan Waer
 */

/* This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */


 /*
 * NOTE: Airport IATA/ICAO code is NOT the same as Airline IATA/ICAO!!!
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <QtCore>
#include <QString>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <sstream>
#include <ctime>


using namespace std;

struct AIRPORTDATA
{
    QString OpenFlightAirportID;
    QString AirportName;
    QString CityName;
    QString CountryName;
    QString AirportCode[2]; // first element is IATA for airport(3 letter code), then ICAO (4 letter code);
    QString Latitude;
    QString Longitude;
};

struct AIRLINEDATA
{
    QString OpenFlightAirlineID;
    QString AirlineName;
    QString AirlineCode[2]; //first element is IATA for airline (2 letter code), then ICAO(3 letter code);
    QString AirlineCountry;
};

struct ROUTES
{
    QString AirCode; // 2 letter IATA or 3 letter ICAO;
    QString OpenFlightAirlineID;
    QString SourceAirportCode; //3 letter IATA or 4 letter ICAO
    QString SourceOpenFlightPortID;
    QString DestAirportCode; //3 letter IATA or 4 letter ICAO
    QString DestOpenFlightPortID;
};

void airport_parse(vector<AIRPORTDATA>&);
void airline_parse(vector<AIRLINEDATA>&);
void route_parse(vector<ROUTES>&);
void format();
QString sanitize(string str);
void construct_XML_V2(vector<AIRPORTDATA>& portdata, vector<AIRLINEDATA>& airlinedata,vector<ROUTES> &routedata);
void construct_XML(vector<AIRPORTDATA>& portdata, vector<AIRLINEDATA>& airlinedata,vector<ROUTES> &routedata);
QString distances(QString& src_lat, QString& src_long, QString &des_lat, QString& des_long);


int main()
{
    vector<AIRPORTDATA> port_data;
    vector<AIRLINEDATA> airline_data;
    vector<ROUTES> route_data;

    int clocks;

    clocks = clock();
    airport_parse(port_data);
    airline_parse(airline_data);
    route_parse(route_data);
    //construct_XML(port_data,airline_data,route_data);
    construct_XML_V2(port_data,airline_data,route_data);


    cout<<"DONE!\n";
    cout<<"TIME:"<<(float)(clock()-clocks)/CLOCKS_PER_SEC<<endl;
    return 0;
}

void airport_parse(vector<AIRPORTDATA>& data)
{
    ifstream in;
    string temp;
    size_t pos=0,pos2=0;
    QString arr[13]={0};
    AIRPORTDATA tempdata;
    int count=0;

    in.open("airports.dat");

    while(getline(in,temp))
    {

        while((pos = temp.find(",",pos2)) != string::npos)
        {
            arr[count++] = sanitize(temp.substr(pos2,pos-pos2));
            pos2 = pos+1;
        }
        arr[count] = sanitize(temp.substr(pos2));
        if(!(arr[4] == "\\N" && arr[5] == "\\N")) // if neither airport code is available, disregard it
        {
            tempdata.OpenFlightAirportID = arr[0];
            tempdata.AirportName = arr[1];
            tempdata.CityName = arr[2];
            tempdata.CountryName = arr[3];
            tempdata.AirportCode[0] = arr[4];
            tempdata.AirportCode[1] = arr[5];
            tempdata.Latitude = arr[6];
            tempdata.Longitude = arr[7];
            data.push_back(tempdata);
        }
        pos2=0;
        count = 0;
    }

    in.close();

}

void airline_parse(vector<AIRLINEDATA>& data)
{
    ifstream in;
    string temp;
    size_t pos=0,pos2=0;
    QString arr[12]={0};
    AIRLINEDATA tempdata;
    int count=0;

    in.open("airlines.dat");

    while(getline(in,temp))
    {
        while((pos = temp.find(",",pos2)) != string::npos)
        {
            arr[count++] = sanitize(temp.substr(pos2,pos-pos2));
            pos2 = pos+1;
        }
        arr[count] = sanitize(temp.substr(pos2));
        tempdata.OpenFlightAirlineID = arr[0];
        tempdata.AirlineName = arr[1];
        tempdata.AirlineCode[0] = arr[3]; // 2 digit airline IATA
        tempdata.AirlineCode[1] = arr[4]; // 3 digit airline ICAO
        tempdata.AirlineCountry = arr[6]; //NEW
        data.push_back(tempdata);
        pos2=0;
        count = 0;
    }
    in.close();
}

void route_parse(vector<ROUTES>&data)
{
    ifstream in;
    string temp;
    size_t pos=0,pos2=0;
    QString arr[12]={0};
    ROUTES tempdata;
    int count=0;

    in.open("routes.dat");

    while(getline(in,temp))
    {
        while((pos = temp.find(",",pos2)) != string::npos)
        {
            arr[count++] = sanitize(temp.substr(pos2,pos-pos2));
            pos2 = pos+1;
        }
        arr[count] = sanitize(temp.substr(pos2));
        tempdata.AirCode = arr[0]; //2 letter or 3-letter ICAO code for airline.
        tempdata.OpenFlightAirlineID = arr[1];
        tempdata.SourceAirportCode = arr[2];
        tempdata.SourceOpenFlightPortID = arr[3];
        tempdata.DestAirportCode = arr[4];
        tempdata.DestOpenFlightPortID = arr[5];
        data.push_back(tempdata);
        pos2=0;
        count = 0;
    }
    in.close();
}

QString sanitize(string str)
{
    QString qstr;

    if(str == "\"\"") // if we have "" as the token, change it to \N to indicate NULL.
        str = "\\N";
    else if(str !="\"\"") // not ""
        if(str[0]== '\"') // if starts with ", means it also ends with "
            str = str.substr(1,str.size()-2);

    return (qstr = QString::fromLatin1(str.data(), str.size()));
}

void construct_XML_V2(vector<AIRPORTDATA>& portdata, vector<AIRLINEDATA>& airlinedata,vector<ROUTES> &routedata)
{
    int portsize = portdata.size(),routesize = routedata.size(), linesize = airlinedata.size();
    //QString portcode,
    QString airportID,qtemp;
    QString filename = "dataTestV3.xml";
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    bool found = false,hasroute= false;

    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0"); // XML version 1.0

    writer.writeStartElement("Document");

    //write the number of airports in data
    writer.writeStartElement("AirportSize");
    writer.writeTextElement("PortSize",QString::number(portsize));
    writer.writeEndElement(); // </AriportSize>

    //writing out all airport names
    //include IATA, use ICAO if IATA does not exist
    writer.writeStartElement("AirportNames");
    for(int i = 0; i < portsize;++i)
    {
        if(portdata[i].AirportCode[0]!="\\N")
        {
            writer.writeStartElement("PortName");
            writer.writeAttribute("CityName",portdata[i].CityName);
            writer.writeAttribute("CountryName",portdata[i].CountryName);
            if(portdata[i].AirportCode[1] !="\\N") // write the ICAO name for this IATA
                writer.writeAttribute("PortICAO",portdata[i].AirportCode[1]);
            writer.writeCharacters(portdata[i].AirportCode[0]);
            writer.writeEndElement();
            //writer.writeTextElement("PortName",portdata[i].AirportCode[0]);
        }
        else
        {
           // std::cout<<"USE ICAO FOR AIRPORT NAME...\n";
            //writer.writeTextElement("PortName",portdata[i].AirportCode[1]);
            writer.writeStartElement("PortName");
            writer.writeAttribute("CityName",portdata[i].CityName);
            writer.writeAttribute("CountryName",portdata[i].CountryName);
            writer.writeCharacters(portdata[i].AirportCode[1]);
            writer.writeEndElement();
        }
    }
    writer.writeEndElement(); // </AirportNames>

    //write the airline and its full name, attribute would be its country
    writer.writeStartElement("AirlineDatas");
    for(int i = 0; i < linesize; ++i)
    {
        if(airlinedata[i].AirlineCode[0] != "\\N")
        {
            writer.writeStartElement("AirlineCode");
            writer.writeAttribute("AirlineName",airlinedata[i].AirlineName);
            writer.writeAttribute("AirlineCountry",airlinedata[i].AirlineCountry);
            writer.writeCharacters(airlinedata[i].AirlineCode[0]);
            writer.writeEndElement();

        }
        else
        {
            writer.writeStartElement("AirlineCode");
            writer.writeAttribute("AirlineName",airlinedata[i].AirlineName);
            writer.writeAttribute("AirlineCountry",airlinedata[i].AirlineCountry);
            writer.writeCharacters(airlinedata[i].AirlineCode[1]);
            writer.writeEndElement();
        }
    }
    writer.writeEndElement(); // </AirlineDatas>


    //write out edges from route data
    writer.writeStartElement("RouteDatas");
    for(int i = 0; i < portsize; ++i)
    {
        writer.writeStartElement("Airport");
        //writer.writeTextElement("AirportID",portdata[i].OpenFlightAirportID); //inconsistent, hence not used.
        writer.writeTextElement("Port_IATA",portdata[i].AirportCode[0]);
        //writer.writeTextElement("Port_ICAO",portdata[i].AirportCode[1]); // no point for this since route only uses airport IATA codes.
        //writer.writeTextElement("City",portdata[i].CityName);
        //writer.writeTextElement("Country",portdata[i].CountryName);

        //BEGIN EDGE WRITING

        //portcode =  (portdata[i].AirportCode[0] == "\\N" ? portdata[i].AirportCode[1] : portdata[i].AirportCode[0]);
        airportID = portdata[i].OpenFlightAirportID;
        writer.writeStartElement("Routes");

        for(int j = 0; j < routesize; ++j)
        {
            if(airportID == routedata[j].SourceOpenFlightPortID) // if matches with airport code and source airport code
            {
                for(int m = 0; m < portsize;++m)
                {
                    if(portdata[m].OpenFlightAirportID == routedata[j].DestOpenFlightPortID) // if dest. airport actually exists
                    {
                        writer.writeStartElement("Edge");
                        writer.writeTextElement("Des_Port_IATA",routedata[j].DestAirportCode); //destination port
                        writer.writeTextElement("Des_Airline_Code",routedata[j].AirCode); //airline IATA or ICAO
//                        qtemp = routedata[j].OpenFlightAirlineID;
//                        for(int k = 0; k < linesize;++k)
//                        {
//                            if(qtemp == airlinedata[k].OpenFlightAirlineID)
//                            {
//                                writer.writeTextElement("Carrier_AirlineName",airlinedata[k].AirlineName);
//                                found = true;
//                                break;
//                            }
//                        }
//                        if(qtemp =="\\N") //NEW, in case no airline names for that route;
//                            writer.writeTextElement("Carrier_AirlineName","unknown");

                        writer.writeTextElement("Distance",
                                                distances(portdata[i].Latitude,portdata[i].Longitude,

                                                          portdata[m].Latitude,portdata[m].Longitude));
                        writer.writeEndElement(); // </Edge>

                    }
                }
            }
        }
        writer.writeEndElement();//</Routes>
        //END EDGE WRITING
        writer.writeEndElement();//</Airport>
    }
    writer.writeEndElement();//</Datas>
    writer.writeEndElement();//</Document>

    writer.writeEndDocument();

    file.close();
    std::cout<<"DONE, FILE CLOSED\n";

}

void construct_XML(vector<AIRPORTDATA>& portdata, vector<AIRLINEDATA>& airlinedata,vector<ROUTES> &routedata)
{
    int portsize = portdata.size(),routesize = routedata.size(), linesize = airlinedata.size();
    //QString portcode,
    QString airportID,qtemp;
    QString filename = "dataTest.xml";
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&file);
    bool found = false,hasroute= false;

    writer.setAutoFormatting(true);
    writer.writeStartDocument("1.0"); // XML version 1.0

    writer.writeStartElement("Document");

    writer.writeStartElement("AirportSize");
    writer.writeTextElement("PortSize",QString::number(portsize));
    writer.writeEndElement(); // </AriportSize>

    //writing out all airport names
    //include IATA, use ICAO if IATA does not exist
    writer.writeStartElement("AirportNames");
    for(int i = 0; i < portsize;++i)
    {
        if(portdata[i].AirportCode[0]!="\\N")
            writer.writeTextElement("PortName",portdata[i].AirportCode[0]);
        else
        {
           // std::cout<<"USE ICAO FOR AIRPORT NAME...\n";
            writer.writeTextElement("PortName",portdata[i].AirportCode[1]);
        }
    }
    writer.writeEndElement(); // </AirportNames>

    writer.writeStartElement("Datas");
    for(int i = 0; i < portsize; ++i)
    {
        writer.writeStartElement("Airport");
        //writer.writeTextElement("AirportID",portdata[i].OpenFlightAirportID);
        writer.writeTextElement("Port_IATA",portdata[i].AirportCode[0]);
        writer.writeTextElement("Port_ICAO",portdata[i].AirportCode[1]);
        writer.writeTextElement("City",portdata[i].CityName);
        writer.writeTextElement("Country",portdata[i].CountryName);

        //BEGIN EDGE WRITING

        //portcode =  (portdata[i].AirportCode[0] == "\\N" ? portdata[i].AirportCode[1] : portdata[i].AirportCode[0]);
        airportID = portdata[i].OpenFlightAirportID;
        writer.writeStartElement("Routes");


        for(int j = 0; j < routesize; ++j)
        {
            if(airportID == routedata[j].SourceOpenFlightPortID) // if matches with airport code and source airport code
            {
                for(int m = 0; m < portsize;++m)
                {
                    if(portdata[m].OpenFlightAirportID == routedata[j].DestOpenFlightPortID) // if dest. airport actually exists
                    {
                        writer.writeStartElement("Edge");
                        writer.writeTextElement("Des_Port_IATA",routedata[j].DestAirportCode); //destination port
                        writer.writeTextElement("Des_Airline_Code",routedata[j].AirCode);
                        qtemp = routedata[j].OpenFlightAirlineID;

                        for(int k = 0; k < linesize;++k)
                        {
                            if(qtemp == airlinedata[k].OpenFlightAirlineID)
                            {
                                writer.writeTextElement("Carrier_AirlineName",airlinedata[k].AirlineName);
                                found = true;
                                break;
                            }
                        }
                        if(qtemp =="\\N") //NEW, in case no airline names for that route;
                            writer.writeTextElement("Carrier_AirlineName","unknown");


                        writer.writeTextElement("Distance",
                                                distances(portdata[i].Latitude,portdata[i].Longitude,

                                                          portdata[m].Latitude,portdata[m].Longitude));
                        writer.writeEndElement(); // </Edge>

                    }
                }
            }

        }
        writer.writeEndElement();//</Routes>
        //END EDGE WRITING
        writer.writeEndElement();//</Airport>
    }
    writer.writeEndElement();//</Datas>
    writer.writeEndElement();//</Document>

    writer.writeEndDocument();

    file.close();
    std::cout<<"DONE, FILE CLOSED\n";

}

QString distances(QString& src_lat, QString& src_long, QString &des_lat, QString& des_long)
{
    double pi = 3.1415926535898;
    double num[4];
    double c_angle; //central angle
    double radius =3958.76;// 6378.137;//6399.594;//6371;//3958.76; // radius of earth

    //double del_phi = fabs(num[2]-num[0]);
    num[0]=src_lat.toDouble(); //source latitudee
    num[1]=src_long.toDouble(); // source longitude
    num[2]=des_lat.toDouble(); // destination latitude
    num[3]=des_long.toDouble(); //destination longitude

    /*
    //double del_lam = fabs(num[3]-num[1]);
    // Vincenty Formula
    c_angle = atan2( sqrt( pow((cos(num[2]*(pi/180)) * sin(del_lam*(pi/180))),2.0)
            + pow((cos(num[0]*(pi/180))*sin(num[2]*(pi/180)) - sin(num[0]*(pi/180))*cos(num[2]*(pi/180))*cos(del_lam*(pi/180))),2.0)),
            (sin(num[0]*(pi/180)) *sin(num[2]*(pi/180)) + cos(num[0]*(pi/180)) * cos(num[2]*(pi/180)) * cos(del_lam*(pi/180))));
    */
    //chord length
    double del_x = cos(num[2]*(pi/180))*cos(num[3]*(pi/180))-cos(num[0]*(pi/180))*cos(num[1]*(pi/180));
    double del_y = cos(num[2]*(pi/180))*sin(num[3]*(pi/180))-cos(num[0]*(pi/180))*sin(num[1]*(pi/180));
    double del_z = sin(num[2]*(pi/180)) - sin(num[0]*(pi/180));
    double c = sqrt(pow(del_x,2.0) + pow(del_y,2.0) +pow(del_z,2.0));
    c_angle = 2*asin((c/2));
    return QString::number(c_angle * radius);
}
