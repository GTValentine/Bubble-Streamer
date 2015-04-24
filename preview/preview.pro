QT += core widgets

TARGET = preview
TEMPLATE = app

include(../common.pri)
include(../bubblib.pri)
include(src/src.pri)

FORMS += forms/mainwindow.ui

RESOURCES += glsl.qrc

unix:!macx: LIBS += -L$$PWD/../../../../../tmp/OpenVDB/lib/ -lopenvdb

INCLUDEPATH += $$PWD/../../../../../tmp/OpenVDB/include
DEPENDPATH += $$PWD/../../../../../tmp/OpenVDB/include

INCLUDEPATH += /usr/include/tbb
LIBS += /usr/lib/libtbb*

INCLUDEPATH += /usr/include/boost
LIBS += /usr/lib/x86_64-linux-gnu/libboost*

INCLUDEPATH += /usr/include/OpenEXR
LIBS += /usr/lib/x86_64-linux-gnu/libHalf*

CONFIG += console
