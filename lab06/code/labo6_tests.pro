
CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle
QT -= gui qt

unix {
    LIBS += -lpthread
}

LIBS += -lgtest
LIBS += -lpcosynchro

INCLUDEPATH += src test
SOURCES += \
    test/main.cpp

HEADERS += \
    src/abstractmatrixmultiplier.h \
    src/matrix.h \
    src/simplematrixmultiplier.h \
    src/threadedmatrixmultiplier.h \
    test/multipliertester.h \
    test/multiplierthreadedtester.h

DISTFILES += \
    ../sources.pri \
    ../sources.pri
