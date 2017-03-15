/* Flight Path Analyzer
 *
 * The purpose of this program is to find the shortest path between two airports using IATA or ICAO airport codes, Dijkstra's shortest path algorithm is implemented.
 *
 *
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

/* Note: on April 29, 2016, i compiled the program again under Ubuntu 16.04LTS with QT5.6, apparaently the qsort function does not work. so i used the sort from algorithm.
 *
 *
 *
 *
 *
 *
 */

#include <QCoreApplication>
#include <QtXml>
#include <QDebug>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>
#include <ctime>
#include <algorithm>

#define INFINITYNUM 9999999

//resources: stackoverflow.com

using namespace std;

string readNextText(QXmlStreamReader &reader);
void processAirportNames(QXmlStreamReader &reader, string * portholder, int portsize);
void processDistances(QFile& file,QXmlStreamReader &reader, string *portNameHolder, double**table, const int &portSize);
int compare (const void * a, const void * b);
int binarySearch(string [], string target, int &midPoint,const int &portSize);
void dijkstra(int nodes, int start, int end,double **table, double dist[], int pred[]);
int output(int start, double dist[], int end, int pred[], vector<int>& shortpath);
void getInput(string &origin, string &dest);
bool validateInput(string& origin, string& dest);
int initialization(QFile& file,int &portSize, string* &portnamearray, double** &table, double* &dist, int* &pred, QXmlStreamReader& reader);
void summary(string * &portNameHolder, double* &dist, int end, vector<int>&shortpath);
void openXmlReadOp(QString filename,QFile& file, QXmlStreamReader& reader);
void deAllocation(double**& table, string*& portNameHolder, int*& pred, double *&dist, int &portSize,vector<int>& shortpath);
void skipElement(QXmlStreamReader& reader,int size);
void leep(QXmlStreamReader& reader, QString leepTo);

int main()
{
    // start = 3259;//LAX
     //end = 3439; //LGA in NYC
    // end = 4586; //OKA, (Naha, Japan)
     //end = 4602; //OLF,Wolfpoint, montana airport [3259]->[5666]->[566]->[4602]->Distance: 1188.25 (confirmed)

    //changes made: 1.) createad leep function (repetitive code)
    //              2.) readNextText() now returns string so we dont have to write qPrintable dozens of times
    //              3.) moved clocks = clock() to after obtaining valid user input for obvious reasons


    const double UNDEFINED = -2;
    QXmlStreamReader reader;
    QFile file;
    string *portNameHolder,origin,dest;
    vector<int> shortpath;
    double **table, *dist;
    int *pred, start, end, portSize, clocks;
    bool go;

    go = false;
    while(!go)
    {
       getInput(origin,dest);
       go = validateInput(origin,dest);
    }
    clocks = clock();
    if(initialization(file,portSize, portNameHolder,table,dist, pred, reader)==-1)
    {
        cout<<"INITIALIZATION FAILED"<<endl;
        return -1;
    }
    //throw the airport names into the array
    processAirportNames(reader, portNameHolder,portSize);
    binarySearch(portNameHolder,origin,start,portSize);
    binarySearch(portNameHolder,dest,end,portSize);
    processDistances(file,reader,portNameHolder,table,portSize);
    dijkstra(portSize,start,end,table,dist,pred);
    if(output(start,dist,end,pred,shortpath) == -1)
    {
        deAllocation(table,portNameHolder,pred,dist,portSize,shortpath);
        cout<<"EXIT DUE TO NOT PATH"<<endl;
        return -1;
    }
    summary(portNameHolder,dist,end,shortpath);
    clocks = clock() - clocks;
    cout<<"run time: "<<clocks/(CLOCKS_PER_SEC*1.0)<<" seconds."<<endl;
    deAllocation(table,portNameHolder,pred,dist,portSize,shortpath);

    return 0;
}

int initialization(QFile& file,int& portSize, string *&portNameHolder, double **&table, double *&dist, int *&pred, QXmlStreamReader& reader)
{
    file.setFileName("data.xml");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open data.xml file!\n";
        return -1;

    }
    reader.setDevice(&file);

    reader.readNextStartElement(); //read first token
    leep(reader, "PortSize");
    //portSize = readNextText(reader).toInt(); //save the portSize
    portSize = atoi(readNextText(reader).c_str()); //save the portSize

    //create airport name holder and adjancy table
    portNameHolder = new string[portSize];
    table = new double*[portSize];
    for(int i = 0; i < portSize; ++i)
        table[i] = new double[portSize];

    //initialize to INFINITYNUM
    for(int i = 0; i < portSize; ++i)
        for(int j = 0; j < portSize; ++j)
            table[i][j] = INFINITYNUM;

    //create array dist and pred
    pred = new int[portSize];
    dist = new double[portSize];
    return 0;
}

string readNextText(QXmlStreamReader &reader)
{
    reader.readNext();
    return qPrintable(reader.text().toString());
}

void processAirportNames(QXmlStreamReader &reader, string* portHolder,int portSize)
{
    int index = 0;
    leep(reader, "PortName");
    while(reader.name()=="PortName")
    {
        portHolder[index++] = readNextText(reader);
        reader.skipCurrentElement();
        reader.readNextStartElement();
    }
    //sort the array of strings
    //qsort(portHolder,portSize,sizeof(string),compare);
    sort(portHolder,portHolder+portSize);
}

void processDistances(QFile& file, QXmlStreamReader &reader, string* portNameHolder, double **table, const int &portSize)
{
    int origin, dest;
    double dist;
    string Origin, Dest;
    bool BREAK = false;

    leep(reader, "Port_IATA");
    while(!reader.atEnd())
    {
        //step 1 identify origin airport and get number in array using binary search
        Origin = readNextText(reader); // ********keep in mind that some of the IATA could be \N. need to get the ICAO for those
        if(Origin=="\\N") //if IATA = \N use ICAO
        {
            leep(reader, "Port_ICAO");
            Origin = readNextText(reader); // ********keep in mind that some of the IATA could be \N. need to get the ICAO for those
        }
        binarySearch(portNameHolder, Origin, origin, portSize); //origin is updated inside func
        //step 2 identify desitination airport and get number in array using binary search
        while(reader.name() != "Des_Port_IATA" && !reader.atEnd())
        {
            reader.readNextStartElement(); //fast forward to the first one
            if(reader.name() == "Routes" && reader.isEndElement())
            {
                BREAK = true;
                break;
            }
        }
        while(!reader.atEnd())
        {
            if(BREAK)
            {
                BREAK=0;
                break;
            }

            Dest = readNextText(reader); //get dest name

            if(binarySearch(portNameHolder, Dest, dest, portSize) != -1) //NEW
            //get dest index in array, AIRPORT MUST EXISTS IN ARRAY!!!
            {
                //step 3 store distance in the adjacency table
                leep(reader, "Distance");
                //dist = readNextText(reader).toDouble();
                dist = strtod(readNextText(reader).c_str(), NULL);
                table[origin][dest] = dist;
            }
            dist = 0;// we dont want ppl ahead to be saving any phony data

            //now we need to prepare it for the loop
            while(reader.name() != "Des_Port_IATA" && !reader.atEnd())
            {
                reader.readNextStartElement();
                if(reader.name()=="Airport")
                    break;
            }
            if(reader.name()=="Airport")
                break;
        }
        //need to prepare to start over //fast forward to the first airport
        leep(reader, "Port_IATA");
    }
    reader.clear();
    file.close();
}

int compare (const void * a, const void * b)
{
    if ( *(string*)a <  *(string*)b )
        return -1;
    if ( *(string*)a == *(string*)b )
        return 0;
    if ( *(string*)a >  *(string*)b )
        return 1;
}

int binarySearch(string portNameHolder[], string target, int &midPoint, const int &portSize)
{
    int lowerBound = 0, upperBound = portSize;
    while( upperBound - lowerBound > 1)
    {
        midPoint = ((upperBound - lowerBound)/ 2) + lowerBound;
        if(portNameHolder[midPoint] == target)
            return midPoint;
        if(target > portNameHolder[midPoint])
            lowerBound = midPoint;
        else
            upperBound = midPoint;
    }
    if(portNameHolder[upperBound] == target)
    {
        midPoint = upperBound;
        return midPoint;
    }
    if(portNameHolder[lowerBound] == target)
    {
        midPoint = lowerBound;
        return midPoint;
    }

    return -1; //return -1 if not found
}

void dijkstra(int portSize, int start, int end,double **table, double dist[], int pred[])
{
    double minDist;
    int minIndex;
    int undefined = -1;

    dist[start] = 0; //distance to itself is 0;
    pred[start] = undefined; // start location is not part of calculation.

    bool* flag = new bool[portSize];

    for(int i = 0; i < portSize; ++i) // initialization
    {
        flag[i] =  false;
        if(i != start)
        {
            dist[i] = table[start][i];
            pred[i] = undefined;
        }
    }

    for( int k = 1; k < portSize; ++k)
    {
        minDist = INFINITYNUM;
        for(int i = 0; i < portSize; ++i)
        {
            if(dist[i] < minDist && !flag[i]) // while flag is false(vertices not processed) and dist[i] less than min. distance
            {
                minDist = dist[i];
                minIndex = i;
            }
        }
        if(minIndex == end)
        {
            break;
        }
        flag[minIndex] = true; //the vertex is processed.
        //Below condition: only when flag is false, when table value is not zero, distance value is not infinity, and
        //sum of distance at min index plus value from table is less than distance at that iteration.
        for (int i = 0; i < portSize; i++)
            if (!flag[i] && (dist[minIndex] + table[minIndex][i] < dist[i]))
            {
               dist[i] = dist[minIndex] + table[minIndex][i];
               pred[i] = minIndex; //saving the shortest path
            }
    }
    delete [] flag;
}

int output(int start,double dist[], int end, int pred[],vector<int> &shortpath)
{

    if(dist[end]==INFINITYNUM) // for airport that have no connected flights.
    {
        cout<<"No Routes Found!\n";
        return -1;
    }

    int undefined = -1;
    int vertex;
    vertex = end;

    while(pred[vertex] != undefined)
    {
        shortpath.push_back(vertex);
        vertex = pred[vertex];
    }
    shortpath.push_back(vertex);
    shortpath.push_back(start);
    return 0;
}

bool validateInput(string &origin, string &dest)
{
    bool originBool = false, destBool = false;
    string IATA, ICAO;

    QFile file("data.xml");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open data.xml file!\n";
        return -1;

    }
    QXmlStreamReader reader;
    reader.setDevice(&file);

    reader.readNextStartElement(); //read first token
    while(!reader.atEnd())
    {
        leep(reader, "Port_IATA");
        IATA = readNextText(reader); //store the IATA
        leep(reader, "Port_ICAO");
        ICAO = readNextText(reader); //store the ICAO

        if(IATA==origin)
            originBool=true;
        if(ICAO==origin)
        {
            originBool=true;
            if(IATA!="\\N")
                origin=IATA;
        }
        if(IATA==dest)
            destBool=true;
        if(ICAO==dest)
        {
            destBool=true;
            if(IATA!="\\N")
                dest=IATA;
        }
        if(originBool&&destBool)
        {
            reader.clear();
            file.close();
            return true;
        }
    }
    reader.clear();
    file.close();
    cout<<"Invalid input, please try again.\n";
    return false;

}

void getInput(string &origin, string &dest)
{
    cout<<"Enter Origin Airport(IATA or ICAO): ";
    cin>>origin;
    cout<<"Enter Destination Airport(IATA or ICAO): ";
    cin>>dest;

    //make the input CAPITAL
    for(size_t i = 0; i < origin.length(); ++i)
        origin[i] = toupper(origin[i]);
    for(size_t i = 0; i < dest.length(); ++i)
        dest[i] = toupper(dest[i]);
}


void summary(string * &portNameHolder,double* & dist,int end,vector<int>&shortpath)
{
    int i = shortpath.size()-1;

    string distance,tempairname;
    QXmlStreamReader reader;
    QFile file;

    openXmlReadOp("data.xml",file,reader);

    cout<<"\nITINERARY:\n";

    while (i > 0)
    {
        cout << "\nDepart from ";

        while(!reader.isEndDocument())
        {
            reader.readNextStartElement(); //read first token

            if(reader.name() == "Port_IATA" && reader.isStartElement())
            {
                if(readNextText(reader) == portNameHolder[shortpath[i]]) // match source airport
                {
                    skipElement(reader,2);
                    cout<<portNameHolder[shortpath[i]];
                    cout<<" ( "<< readNextText(reader)<<", "; // City
                    reader.skipCurrentElement();
                    reader.readNextStartElement();
                    cout<< readNextText(reader)<<" ),"; //country is right after city
                    reader.skipCurrentElement();

                    cout<<" Via the following airlines:\n";
                    --i; //Decrement vector index to reach next airport

                    while(!(reader.name() == "Routes" && reader.isEndElement()))// while not at </Routes>
                    {
                        reader.readNext();
                        if(reader.name() == "Des_Port_IATA" && reader.isStartElement())
                        {
                            if(readNextText(reader) == portNameHolder[shortpath[i]]) // destination airport
                            {
                                skipElement(reader,2);
                                tempairname = readNextText(reader);
                                if(tempairname != "unknown")
                                    cout<< "-> " <<tempairname<<endl;  //Name of airline;
                                reader.skipCurrentElement();
                                reader.readNextStartElement();
                                distance = readNextText(reader);
                            }
                        }
                    }
                    break; //break out of while since source airport is dealt with.
                }
            }
        }

        cout<<"\nArriving at ";
        reader.clear();
        file.close();
        openXmlReadOp("data.xml",file,reader);

        reader.readNext();
        while(!reader.isEndDocument())
        {
            if(reader.name() =="Port_IATA" && reader.isStartElement())
            {
                if(readNextText(reader) == portNameHolder[shortpath[i]]) // match source airport
                {
                    skipElement(reader,2);
                    cout<<portNameHolder[shortpath[i]];
                    cout<<" ( "<< readNextText(reader)<<", "; // City
                    reader.skipCurrentElement();
                    reader.readNextStartElement(); //country tag is right after city
                    cout<< readNextText(reader)<<" ),";

                    cout<< " Distance: "<< distance<<" Miles.\n";
                    break;
                }
            }
            else
                reader.readNext();
        }
        reader.clear();
        file.close();
        openXmlReadOp("data.xml",file,reader);
    }
    cout<<"\nTotal Distance: "<<dist[end]<<" Miles."<<endl;
}

void openXmlReadOp(QString filename,QFile& file, QXmlStreamReader& reader)
{
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    reader.setDevice(&file); //rewind.
}

void deAllocation(double** &table,string* &portNameHolder,int* &pred, double* &dist, int& portSize,vector<int>& shortpath)
{
    shortpath.clear();
    for(int i = 0; i < portSize; ++i)
        delete table[i];
    delete [] table;
    delete [] portNameHolder;
    delete [] pred;
    delete [] dist;
}

void skipElement(QXmlStreamReader& reader,int size)
{
    for(int i = 0; i < size;++i)
    {
        reader.skipCurrentElement();
        reader.readNextStartElement();
    }
}

void leep(QXmlStreamReader& reader, QString leepTo)
{
    while(reader.name() != leepTo && !reader.atEnd()) //fast forward to the first airport
        reader.readNextStartElement();
}
