CONFIG -= QT
QT -= core widgets gui

TARGET = bubblib
VERSION = 0.1.0
TEMPLATE = lib
CONFIG += staticlib

include(../openvdb.pri)
include(../common.pri)
include(src/src.pri)

INCLUDEPATH += $$PWD/../
DEPENDPATH += $$PWD/../

INCLUDEPATH += /usr/include/tbb
LIBS += /usr/lib/libtbb*

INCLUDEPATH += /usr/include/boost
LIBS += /usr/lib/x86_64-linux-gnu/libboost*

INCLUDEPATH += /usr/include/OpenEXR
LIBS += /usr/lib/x86_64-linux-gnu/libHalf*
