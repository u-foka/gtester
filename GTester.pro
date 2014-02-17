QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GTester
TEMPLATE = app

ICON = resources/gtester.icns
RC_FILE = resources/gtester.rc

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/executablebase.cpp \
    src/executabletester.cpp \
    src/testmodel.cpp \
    src/testitembase.cpp \
    src/testitemexecutable.cpp \
    src/testitemcase.cpp \
    src/testitem.cpp \
    src/testitemroot.cpp \
    src/executabletestrunner.cpp

HEADERS  += src/mainwindow.h \
    src/executablebase.h \
    src/executabletester.h \
    src/testmodel.h \
    src/testitembase.h \
    src/testitemexecutable.h \
    src/testitemcase.h \
    src/testitem.h \
    src/testitemroot.h \
    src/executabletestrunner.h

FORMS    += ui/mainwindow.ui

RESOURCES += resources/icons.qrc
