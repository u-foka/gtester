QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GTester
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/executethread.cpp

HEADERS  += src/mainwindow.h \
    src/executethread.h

FORMS    += ui/mainwindow.ui

RESOURCES += resources/icons.qrc
