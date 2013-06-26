
include(../../../qmake.inc)

QT -= core gui

TARGET = DNSTest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$CPPUNIT_LIBS

SOURCES += main.cpp DNSTest.cpp
HEADERS += DNSTest.h
INCLUDEPATH += ../../.. ../../libfwbuilder/src
DEPENDPATH  += ../../libfwbuilder/src
LIBS = ../../libfwbuilder/src/fwbuilder/libfwbuilder.a $$LIBS
run_tests.commands = echo "Running tests..." && ./${TARGET}
run_tests.depends = all
clean_tests.depends = clean
build_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests clean_tests build_tests
