
include(../../../../qmake.inc)

QT -= core \
    gui

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$CPPUNIT_LIBS

TARGET = DNSTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    DNSTest.cpp
HEADERS += DNSTest.h
INCLUDEPATH += ../../..
LIBS += -L ../.. -lfwbuilder
run.commands = echo "Running tests..." && ./${TARGET}
run.depends = all
QMAKE_EXTRA_TARGETS += run
