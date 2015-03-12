QT += core widgets

TARGET = preview
TEMPLATE = app

INCLUDEPATH += $$PWD/../bubblib/src
LIBS += -L../bubblib -lbubblib


include(../common.pri)
include(src/src.pri)

FORMS += forms/mainwindow.ui

RESOURCES += glsl.qrc
