#-*- mode: makefile; tab-width: 4; -*-

include(../../../qmake.inc)

# exists(../../qmake.inc):include( ../../qmake.inc)

QMAKE_CXX = g++

TEMPLATE = app
TARGET =
DEPENDPATH += .

SOURCES	 =  ../CompilerDriver.cpp \
    ../CompilerDriver_compile.cpp \
    ../CompilerDriver_generators.cpp \
    ../Configlet.cpp \
    ../interfaceProperties.cpp \
    ../linux24Interfaces.cpp \
    ../bsdInterfaces.cpp \
    ../iosInterfaces.cpp \
    ../pixInterfaces.cpp \
    ../interfacePropertiesObjectFactory.cpp

HEADERS	 = ../../../config.h \
    ../CompilerDriver.h \
    ../Configlet.h \
    ../interfaceProperties.h \
    ../linux24Interfaces.h \
    ../bsdInterfaces.h \
    ../iosInterfaces.h \
    ../pixInterfaces.h \
    ../interfacePropertiesObjectFactory.h

# Actual tests
HEADERS += interfacePropertiesTest.h
SOURCES += tests_main.cpp interfacePropertiesTest.cpp

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov

run.commands = ./${TARGET} && gcov -o . ../*.h ../*.cpp >/dev/null
run.depends = all

QMAKE_EXTRA_TARGETS += run


