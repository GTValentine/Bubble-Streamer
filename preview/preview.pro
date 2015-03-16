QT += core widgets

TARGET = preview
TEMPLATE = app

include(../common.pri)
include(../bubblib.pri)
include(src/src.pri)

FORMS += forms/mainwindow.ui

RESOURCES += glsl.qrc
