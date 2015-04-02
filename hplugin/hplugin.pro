CONFIG -= QT
QT -= core widgets gui

TARGET = SOP_Bubbles
VERSION = 0.1.0
TEMPLATE = lib

include(../common.pri)
include(../bubblib.pri)
include(src/src.pri)


######## HOUDINI ##############################################################

*-clang* {
    error("Houdini (on Linux) WILL NOT load Clang-generated .so files!")
}

linux {
    LIBS += \
        -lHoudiniUI -lHoudiniOPZ \
        -lHoudiniOP3 -lHoudiniOP2 -lHoudiniOP1 \
        -lHoudiniSIM -lHoudiniGEO -lHoudiniPRM -lHoudiniUT

    QMAKE_CXXFLAGS += -isystem /opt/hfs14.0.258/toolkit/include
    INCLUDEPATH += /opt/hfs14.0.258/toolkit/include
    LIBS += -L/opt/hfs14.0.258/dsolib/

    # From outputs of hcustom -c/-m and results of Makefile.gnu
    QMAKE_CXXFLAGS += -m64 -fPIC
    DEFINES += \
        AMD64 \
        ENABLE_THREADS \
        FBX_ENABLED=1 \
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
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -fno-strict-aliasing
    QMAKE_LFLAGS += -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lGLU -lGL -lX11 -lXext -lXi -ldl -lpthread
}

win32 {
    HPATH = "C:\Program Files (x86)\Side Effects Software\Houdini 14.0.258"
    INCLUDEPATH += \"$$HPATH\toolkit\include\"
    DEFINES += \
        BOOST_ALL_NO_LIB \
        FBX_ENABLED=1 \
        HOUDINI_TEMPLATE_EXPORTS \
        I386 \
        MAKING_DSO \
        NDEBUG \
        NEED_SPECIALIZATION_STORAGE \
        NOMINMAX \
        OPENCL_ENABLED=1 \
        OPENVDB_ENABLED=1 \
        SESI_LITTLE_ENDIAN \
        SIZEOF_VOID_P=4 \
        STRICT \
        SWAP_BITFIELDS \
        VERSION=\"14.0.258\" \
        WIN32 \
        WIN32_LEAN_AND_MEAN \
        WINVER=0x0502 \
        _CRT_NONSTDC_NO_DEPRECATE \
        _CRT_SECURE_NO_DEPRECATE \
        _SCL_SECURE_NO_WARNINGS \
        _USE_MATH_DEFINES \
        _USRDLL \
        _WIN32_WINNT=0x0502 \
        _WINDOWS
    QMAKE_CXXFLAGS += \
        -EHsc -GR -TP -Zc:forScope -bigobj -nologo \
        -w14996 -wd4355

    # Link with everything in dsolib.
    QMAKE_LFLAGS += \
        \"$$HPATH/custom/houdini/dsolib/*.a\" \
        \"$$HPATH/custom/houdini/dsolib/*.lib\"
}
