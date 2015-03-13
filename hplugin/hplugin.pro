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

linux {
    # http://eulogger.blogspot.com/2011/09/qt-qmake-variable-reference.html
    DEFINES += \
        USE_PTHREADS  \
        GCC3 \
        SIZEOF_VOID_P=8 \
        DLLEXPORT="" \
        _GNU_SOURCE \
        ENABLE_THREADS \
        ENABLE_UI_THREADS \
        VERSION="14.0.258" \
        LINUX \
        AMD64 \
        MAKING_DSO \
        SESI_LITTLE_ENDIAN \
        SWAP_BITFIELDS
    QMAKE_CXXFLAGS += -isystem /opt/hfs14.0.258/toolkit/include
    LIBS += -L/opt/hfs14.0.258/dsolib/
}

LIBS += \
    -lHoudiniUI -lHoudiniOPZ \
    -lHoudiniOP3 -lHoudiniOP2 -lHoudiniOP1 \
    -lHoudiniSIM -lHoudiniGEO -lHoudiniPRM -lHoudiniUT
#    -lAlembicAbcCollection \
#    -lAlembicAbcCoreAbstract \
#    -lAlembicAbcCoreFactory \
#    -lAlembicAbcCoreHDF5 \
#    -lAlembicAbcCoreOgawa \
#    -lAlembicAbcGeom \
#    -lAlembicAbcMaterial \
#    -lAlembicAbc \
#    -lAlembicOgawa \
#    -lAlembicUtil \
#    -laudiere-1.9.4 \
#    -laudiere \
#    -lblosc \
#    -lboost_chrono \
#    -lboost_iostreams \
#    -lboost_program_options \
#    -lboost_regex \
#    -lboost_system \
#    -lboost_thread \
#    -lbz2 \
#    -lcharset \
#    -lcollada_dae \
#    -lcollada_dom \
#    -lcollada_LIBXMLPlugin \
#    -lcollada_stdErrPlugin \
#    -lcollada_STLDatabase \
#    -lcrypto \
#    -lcurl \
#    -ldwelf \
#    -lField3D \
#    -lfreetype \
#    -lGLEW \
#    -lHalf \
#    -lHAPIL \
#    -lHAPI \
#    -lhdf5_hl \
#    -lhdf5 \
#    -lHoudiniAPPS1 \
#    -lHoudiniAPPS2 \
#    -lHoudiniAPPS3 \
#    -lHoudiniDEVICE \
#    -lHoudiniGEO \
#    -lHoudiniOP1 \
#    -lHoudiniOP2 \
#    -lHoudiniOP3 \
#    -lHoudiniOPZ \
#    -lHoudiniPRM \
#    -lHoudiniRAY \
#    -lHoudiniSIM \
#    -lHoudiniUI \
#    -lHoudiniUT \
#    -liconv \
#    -lIex-2_2 \
#    -lIex \
#    -lIlmImf-2_2 \
#    -lIlmImf \
#    -lIlmThread-2_2 \
#    -lIlmThread \
#    -lImath-2_2 \
#    -lImath \
#    -lintl \
#    -ljemalloc \
#    -ljpeg \
#    -lOpenCL \
#    -lopenvdb_sesi \
#    -losdCPU \
#    -losdGPU \
#    -lphonon \
#    -lpng12 \
#    -lpng \
#    -lPtex \
#    -lQtCore \
#    -lQtDBus \
#    -lQtDesignerComponents \
#    -lQtDesigner \
#    -lQtGui \
#    -lQtHelp \
#    -lQtNetwork \
#    -lQtOpenGL \
#    -lQtScript \
#    -lQtSql \
#    -lQtSvg \
#    -lQtTest \
#    -lQtWebKit \
#    -lQtXml \
#    -lsndfile \
#    -lssl \
#    -ltbbmalloc_proxy \
#    -ltbbmalloc \
#    -lTBBPROXY \
#    -ltbb \
#    -ltiff \
#    -lxml2 \
#    -lz
