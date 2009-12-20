include(../../../../qmake.inc)

QT += gui network

TEMPLATE	= app
LANGUAGE	= C++

QMAKE_CXX = g++

HEADERS	+= RCSTest.h \
    ../../../../config.h \
    ../../RCS.h \
    ../../FWBTree.h \
    ../../global.h

SOURCES	+=  main.cpp \
        RCSTest.cpp \
        ../../RCS.cpp \
        ../../FWBTree.cpp

TARGET = RCSTest

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov #-lcppunit

run.commands = echo "Running tests..." && ./${TARGET} && \
               echo "Running gcov..." && gcov ${SOURCES} >/dev/null 2>/dev/null && echo "OK" || echo "FAILED"
run.depends = all

QMAKE_EXTRA_TARGETS += run
