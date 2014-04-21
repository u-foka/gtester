QT       += core xml gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GTester
TEMPLATE = app

DEFINES += "GTESTER_VERSION=\"\\\"$$system(git describe)\\\"\""

ICON = resources/gtester.icns
RC_FILE = resources/gtester.rc

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/executablebase.cpp \
    src/executabletester.cpp \
    src/testmodel.cpp \
    src/testitembase.cpp \
    src/testitemexecutable.cpp \
    src/testitemcase.cpp \
    src/testitem.cpp \
    src/testitemroot.cpp \
    src/executabletestrunner.cpp \
    src/fileformatbase.cpp \
    src/fileformatv10.cpp \
    src/utils.cpp \
    src/application.cpp \
    src/defaultmenu.cpp

HEADERS += \
    src/mainwindow.h \
    src/executablebase.h \
    src/executabletester.h \
    src/testmodel.h \
    src/testitembase.h \
    src/testitemexecutable.h \
    src/testitemcase.h \
    src/testitem.h \
    src/testitemroot.h \
    src/executabletestrunner.h \
    src/fileformatbase.h \
    src/fileformatv10.h \
    src/utils.h \
    src/application.h \
    src/defaultmenu.h \
    src/setfocus.h

FORMS += \
    ui/mainwindow.ui

RESOURCES += resources/icons.qrc

mac {
    LIBS += -framework cocoa

    OBJECTIVE_SOURCES += \
        src/setfocus.mm
}

linux|win32 {
    SOURCES += \
        src/setfocus.cpp
}
