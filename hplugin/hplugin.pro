CONFIG -= QT
QT -= core widgets

TARGET = hplugin
VERSION = 0.1.0
TEMPLATE = lib

INCLUDEPATH += $$PWD/../bubblib/src
LIBS += -L../bubblib -lbubblib


include(../common.pri)
include(src/src.pri)
