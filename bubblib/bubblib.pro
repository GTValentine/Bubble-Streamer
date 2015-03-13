CONFIG -= QT
QT -= core widgets gui

TARGET = bubblib
VERSION = 0.1.0
TEMPLATE = lib
CONFIG += staticlib

include(../common.pri)
include(src/src.pri)
