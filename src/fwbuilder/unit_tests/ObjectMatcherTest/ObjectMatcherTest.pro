
include(../../../../qmake.inc)

QT -= core gui

TARGET = ObjectMatcherTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$CPPUNIT_LIBS

SOURCES += main.cpp ObjectMatcherTest.cpp

HEADERS += ObjectMatcherTest.h

INCLUDEPATH += ../../..

LIBS += -L../.. -lfwbuilder

run.commands = echo "Running tests..." && ./${TARGET}

run.depends = all

QMAKE_EXTRA_TARGETS += run

