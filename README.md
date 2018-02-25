# Flight-Path-Analyzer

[![Build Status](https://travis-ci.org/JasonZell/Flight-Path-Analyzer.svg?branch=master)](https://travis-ci.org/JasonZell/Flight-Path-Analyzer)


The purpose of this program is to find the shortest path between two airports using IATA or ICAO airport codes. 

The airport and route data comes from OpenFlights at http://openflights.org/data.html

The purpose of sanitizer program is solely for creating XML file from raw OpenFlight data files.

Sample screenshot:

![samplescreenshot](https://raw.githubusercontent.com/JasonZell/Flight-Path-Analyzer/master/sampleScreenshot/samplescreen.png)

Details: 
Qt Streamreader is utilized to read the XML file. 
Dijkstra's shortest path algorithm is implemented in this program.

CONTRIBUTORS:
Chang Zhang (https://github.com/JasonZell) Ryan Waer (https://github.com/rwhere)
