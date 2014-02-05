QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GTester
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/executablebase.cpp \
    src/executabletester.cpp \
    src/testmodel.cpp \
    src/testitem.cpp

HEADERS  += src/mainwindow.h \
    src/executablebase.h \
    src/executabletester.h \
    src/testmodel.h \
    src/testitem.h

FORMS    += ui/mainwindow.ui

RESOURCES += resources/icons.qrc
