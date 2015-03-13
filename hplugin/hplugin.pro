CONFIG -= QT
QT -= core widgets gui

TARGET = SOP_Bubbles
VERSION = 0.1.0
TEMPLATE = lib

include(../common.pri)
include(src/src.pri)

INCLUDEPATH += $$PWD/../bubblib/src
LIBS += -L../bubblib -lbubblib


######## HOUDINI ##############################################################

*-clang* {
    error("Houdini (on Linux) WILL NOT load Clang-generated .so files!")
}

linux {
    QMAKE_CXXFLAGS += -isystem /opt/hfs14.0.258/toolkit/include
    LIBS += -L/opt/hfs14.0.258/dsolib/

    # From outputs of hcustom -c/-m and results of Makefile.gnu
    QMAKE_CXXFLAGS += -m64 -fPIC
    DEFINES += \
        AMD64 \
        ENABLE_THREADS \
        FBX_ENABLED=1 \
        GCC3 \
        GCC4 \
        LINUX \
        MAKING_DSO \
        OPENCL_ENABLED=1 \
        OPENVDB_ENABLED=1 \
        SESI_LITTLE_ENDIAN \
        SIZEOF_VOID_P=8 \
        USE_PTHREADS \
        VERSION=\"14.0.258\" \
        _FILE_OFFSET_BITS=64 \
        _GNU_SOURCE \
        _REENTRANT
    QMAKE_CXXFLAGS += -fno-strict-aliasing
    QMAKE_LFLAGS += -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -lpthread
}

LIBS += \
    -lHoudiniUI -lHoudiniOPZ \
    -lHoudiniOP3 -lHoudiniOP2 -lHoudiniOP1 \
    -lHoudiniSIM -lHoudiniGEO -lHoudiniPRM -lHoudiniUT
