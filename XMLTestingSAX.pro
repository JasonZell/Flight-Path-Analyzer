#-------------------------------------------------
#
# Project created by QtCreator 2015-04-27T11:14:08
#
#-------------------------------------------------

QT       += core
QT       += xml

QT       -= gui

TARGET = XMLTestingSAX
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    AirportNode.cpp \
    RouteData.cpp \
    xmlparser.cpp

HEADERS += \
    AirportNode.h \
    RouteData.h \
    xmlparser.h
