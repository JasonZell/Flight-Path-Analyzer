#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <limits>
#include <QXmlStreamReader>
#include <QFile>
#include <QMap>
#include "AirportNode.h"
#include "RouteData.h"

class XMLParser
{
    public:
        explicit XMLParser();


        const double INFINITYNUM = std::numeric_limits<double>::max();
        const double UNDEFINED = -2;

    private:

        void loadXMLFile();
        void initialize();

        QXmlStreamReader xmlReader;
        QFile xmlFile;
        QMap<string,AirportNode> *portMap;
        RouteData **adjTable;
        int portSize, *prev;
        double *dist;

};

#endif // XMLPARSER_H
