
TEMPLATE = app
TARGET = labo_toboggan
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += debug

CONFIG += c++17

MOC_DIR = moc
OBJECTS_DIR = obj

LIBS += -lpcosynchro

QT += testlib gui widgets

# Input
HEADERS += \
    src/playground.h \
    src/display.h \
    src/mainwindow.h \
    src/place.h \
    src/pslideinterface.h \
    src/kid.h
SOURCES += \
    src/display.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/pslideinterface.cpp \
    src/kid.cpp
RESOURCES += toboggan.qrc
