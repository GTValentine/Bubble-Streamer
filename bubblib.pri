INCLUDEPATH += $$PWD/bubblib/src
linux {
    LIBS += -L../bubblib -lbubblib
}
win32:debug {
    LIBS += -L../bubblib/debug -lbubblib
}
win32:release {
    LIBS += -L../bubblib/release -lbubblib
}
