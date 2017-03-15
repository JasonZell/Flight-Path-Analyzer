TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt

SOURCES += main.cpp
QT += core
QT += xml
include(deployment.pri)
#qtcAddDeployment()

