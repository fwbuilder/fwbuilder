#-*- mode: makefile; tab-width: 4; -*-
# -------------------------------------------------
# Project created by QtCreator 2009-12-08T19:22:04
# -------------------------------------------------

include(../../../../qmake.inc)

QMAKE_CXX = g++

QT += gui network

INCLUDEPATH += ../../.ui ../../../compiler_lib/
TARGET = UsageResolverTest
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    UsageResolverTest.cpp \
    ../../UsageResolver.cpp \
    ../../FWBTree.cpp

HEADERS += UsageResolverTest.h ../../UsageResolver.h ../../FWBTree.h

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov # -lcppunit

run.commands = ./${TARGET} && gcov -o . ../../UsageResolver.cpp >/dev/null 2>/dev/null
run.depends = all

QMAKE_EXTRA_TARGETS += run
