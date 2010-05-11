include(../../../../qmake.inc)
QT += gui network
TEMPLATE = app
LANGUAGE = C++
CONFIG += console

INCLUDEPATH += ../../.. ../.. ../../.ui ../../../compiler_lib/

HEADERS += ImporterTest.h \
    	../../IPTImporter.h \
        ../../config.h \
        ../../utils_no_qt.h \
        ../../FWBTree.h \
        ../../Importer.h \
        ../../IOSImporter.h \
        ../../IPTImporter.h \
        ../../../parsers/IPTCfgLexer.hpp \
        ../../../parsers/IPTCfgParser.hpp \
        ../../../parsers/IOSCfgLexer.hpp \
        ../../../parsers/IOSCfgParser.hpp \
        ../../../compiler_lib/interfaceProperties.h \
        ../../../compiler_lib/interfacePropertiesObjectFactory.h \
        ../../../compiler_lib/linux24Interfaces.h \
        ../../../compiler_lib/bsdInterfaces.h \
        ../../../compiler_lib/iosInterfaces.h \
        ../../../compiler_lib/procurveInterfaces.h \
        ../../../compiler_lib/pixInterfaces.h

SOURCES += main_ImporterTest.cpp \
        ImporterTest.cpp \
    	../../utils_no_qt.cpp \
    	../../FWBTree.cpp \
        ../../Importer.cpp \
        ../../IOSImporter.cpp \
        ../../IOSImporterRun.cpp \
        ../../IPTImporter.cpp \
        ../../IPTImporterRun.cpp \
        ../../../parsers/IPTCfgLexer.cpp \
        ../../../parsers/IPTCfgParser.cpp \
        ../../../parsers/IOSCfgLexer.cpp \
        ../../../parsers/IOSCfgParser.cpp \
        ../../../compiler_lib/interfaceProperties.cpp \
        ../../../compiler_lib/interfacePropertiesObjectFactory.cpp \
        ../../../compiler_lib/linux24Interfaces.cpp \
        ../../../compiler_lib/bsdInterfaces.cpp \
        ../../../compiler_lib/iosInterfaces.cpp \
        ../../../compiler_lib/procurveInterfaces.cpp \
        ../../../compiler_lib/pixInterfaces.cpp


TARGET = ImporterTest
CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
DEPENDPATH = ../../../common

!win32:LIBS += ../../../common/libcommon.a
!win32:PRE_TARGETDEPS = ../../../common/libcommon.a

win32:CONFIG += console

win32:LIBS += ../../../common/release/common.lib
win32:PRE_TARGETDEPS = ../../../common/release/common.lib

INCLUDEPATH	 += $$ANTLR_INCLUDEPATH
LIBS		 += ../../../parsers/libfwbparser.a  $$ANTLR_LIBS
DEFINES		 += $$ANTLR_DEFINES


run_tests.commands = echo "Running tests..." && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests
