include(../../../../qmake.inc)
QT += gui \
    network
TEMPLATE = app
LANGUAGE = C++
QMAKE_CXX = g++
HEADERS += ImporterTest.h \
    ../../IPTImporter.h
SOURCES += main.cpp \
    ImporterTest.cpp

HEADERS +=  ../../config.h \
        ../../utils_no_qt.h \
        ../../FWBTree.h \
        ../../Importer.h \
        ../../IOSImporter.h \
        ../../IPTImporter.h \
        ../../../parsers/IPTCfgLexer.hpp \
        ../../../parsers/IPTCfgParser.hpp \
        ../../../parsers/IOSCfgLexer.hpp \
        ../../../parsers/IOSCfgParser.hpp

SOURCES += ../../utils_no_qt.cpp \
        ../../FWBTree.cpp \
        ../../Importer.cpp \
        ../../IOSImporter.cpp \
        ../../IOSImporterRun.cpp \
        ../../IPTImporter.cpp \
        ../../IPTImporterRun.cpp \
        ../../../parsers/IPTCfgLexer.cpp \
        ../../../parsers/IPTCfgParser.cpp \
        ../../../parsers/IOSCfgLexer.cpp \
        ../../../parsers/IOSCfgParser.cpp

INCLUDEPATH += ../../.. ../.. ../../.ui

TARGET = ImporterTest
CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??
LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov# -l antlr
#LIBS += -L../../../antlr
DEPENDPATH = ../../../common

!win32:LIBS += ../../../common/libcommon.a
!win32:PRE_TARGETDEPS = ../../../common/libcommon.a

win32:CONFIG += console

win32:LIBS += ../../../common/release/common.lib
win32:PRE_TARGETDEPS = ../../../common/release/common.lib

INCLUDEPATH	 += $$ANTLR_INCLUDEPATH
LIBS		 += ../../../parsers/libfwbparser.a  $$ANTLR_LIBS
DEFINES		 += $$ANTLR_DEFINES


run.commands = echo "Running tests..." \
    && ./${TARGET} && echo "Running gcov..." && \
    gcov ${SOURCES} >/dev/null 2>/dev/null && \
    echo "OK" || echo "FAILED"
run.depends = all
QMAKE_EXTRA_TARGETS += run
