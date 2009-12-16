# -------------------------------------------------
# Project created by QtCreator 2009-12-16T13:20:15
# -------------------------------------------------

include(../../../../qmake.inc)

QMAKE_CXX = g++

QT += gui network

INCLUDEPATH += ../../.ui ../../../compiler_lib/
TARGET = FWBTreeTest
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    FWBTreeTest.cpp \
    ../../FWBTree.cpp

HEADERS += FWBTreeTest.h ../../FWBTree.h

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov # -lcppunit

run.commands = ./${TARGET} && gcov -o . ../../FWBTree.cpp >/dev/null 2>/dev/null
run.depends = all

QMAKE_EXTRA_TARGETS += run

