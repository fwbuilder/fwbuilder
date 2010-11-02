include(../../../../qmake.inc)

QT += gui network

TEMPLATE	= app
LANGUAGE	= C++

INCLUDEPATH += ../../.ui ../../../compiler_lib/

HEADERS	+= parseCommandLineTest.h
SOURCES	+= main_parseCommandLineTest.cpp parseCommandLineTest.cpp
TARGET = parseCommandLineTest

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
# RESOURCES += ../../MainRes.qrc
LIBS += ../guilib/libguilib.a
LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS

run_tests.commands = echo "Running tests..." && ./${TARGET} && echo "OK" || echo "FAILED"
run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests

INCLUDEPATH += ../../../common ../../../compiler_lib
DEPENDPATH = ../../../common ../../../compiler_lib
win32:LIBS += ../../../common/release/common.lib ../../../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../../../common/libcommon.a ../../../compiler_lib/libcompilerdriver.a
win32:PRE_TARGETDEPS = ../../../common/release/common.lib ../../../compiler_lib/release/libcompilerdriver.lib
!win32:PRE_TARGETDEPS = ../../../common/libcommon.a ../../../compiler_lib/libcompilerdriver.a
