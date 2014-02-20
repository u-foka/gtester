TARGET = GTesterSelfTests
TEMPLATE = app
INCLUDEPATH += \
    gtest \
    gtest/include \
    ../../src

SOURCES += \
    gtest/src/gtest-all.cc \
    gtest/src/gtest_main.cc \
    \
    utilsTest.cpp \
    \
    ../../src/utils.cpp
