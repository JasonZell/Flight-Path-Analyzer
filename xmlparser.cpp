#include "xmlparser.h"

XMLParser::XMLParser()
{
    initialize();
}
void XMLParser::loadXMLFile()
{
    file.setFileName("dataV3.xml");
    xmlReader.setDevice(&file);

}

void XMLParser::initialize()
{
    portMap = new QMap<string,AirportNode>();

    loadXMLFile();
}
