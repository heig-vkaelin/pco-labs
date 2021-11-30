#message("Building student project")
include(../../QtrainSim/QtrainSim.pri)

CONFIG += c++17

LIBS += -lpcosynchro

HEADERS +=  \
    src/route.h \
    src/sharedsectioninterface.h \
    src/locomotive.h \
    src/launchable.h \
    src/locomotivebehavior.h \
    src/sharedsection.h

SOURCES +=  \
    src/locomotive.cpp \
    src/cppmain.cpp \
    src/locomotivebehavior.cpp \
    src/route.cpp
