#include "xmlparser.h"

XMLParser::XMLParser()
{
    initialize();
}
void XMLParser::loadXMLFile()
{
   xmlFile.setFileName("dataV3.xml");
   xmlReader.setDevice(&xmlFile);

}

void XMLParser::initialize()
{
    portMap = new QMap<string,AirportNode>();

    loadXMLFile();
}
