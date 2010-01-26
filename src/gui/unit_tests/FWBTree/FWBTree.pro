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
           ../../FWBTree.cpp \
           ../../../compiler_lib/interfaceProperties.cpp \
           ../../../compiler_lib/interfacePropertiesObjectFactory.cpp \
           ../../../compiler_lib/CompilerDriver.cpp \
           ../../../compiler_lib/CompilerDriver_compile.cpp \
           ../../../compiler_lib/CompilerDriver_generators.cpp \
           ../../../compiler_lib/Configlet.cpp \
           ../../../compiler_lib/linux24Interfaces.cpp \
           ../../../compiler_lib/bsdInterfaces.cpp \
           ../../../compiler_lib/iosInterfaces.cpp \
           ../../../compiler_lib/pixInterfaces.cpp


HEADERS += FWBTreeTest.h \
           ../../FWBTree.h \
           ../../../compiler_lib/interfaceProperties.h \
           ../../../compiler_lib/interfacePropertiesObjectFactory.h \
           ../../../compiler_lib/CompilerDriver.h \
           ../../../compiler_lib/Configlet.h \
           ../../../compiler_lib/linux24Interfaces.h \
           ../../../compiler_lib/bsdInterfaces.h \
           ../../../compiler_lib/iosInterfaces.h \
           ../../../compiler_lib/pixInterfaces.h

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 $$CPPUNIT_CFLAGS
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lgcov -lcppunit

run.commands = echo "Running tests..." && \
    ./${TARGET} && echo "Running gcov..." && \
    gcov ${SOURCES} >/dev/null 2>/dev/null && \
    echo "OK" || { echo "FAILED"; exit 1; }

run.depends = all

QMAKE_EXTRA_TARGETS += run


INCLUDEPATH += ../../../common \
        ../../../iptlib \
    ../../../pflib \
    ../../../cisco_lib/ \
    ../../../compiler_lib/
DEPENDPATH = ../../../common \
        ../../../iptlib \
    ../../../pflib \
    ../../../cisco_lib/ \
    ../../../compiler_lib
win32:LIBS += ../../../common/release/common.lib \
        ../../../iptlib/release/iptlib.lib \
    ../../../pflib/release/fwbpf.lib \
    ../../../cisco_lib/release/fwbcisco.lib \
    ../../../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../../../common/libcommon.a \
        ../../../iptlib/libiptlib.a \
    ../../../pflib/libfwbpf.a \
    ../../../cisco_lib/libfwbcisco.a \
    ../../../compiler_lib/libcompilerdriver.a
win32:PRE_TARGETDEPS = ../../../common/release/common.lib \
        ../../../iptlib/release/iptlib.lib \
    ../../../pflib/release/fwbpf.lib \
    ../../../cisco_lib/release/fwbcisco.lib \
    ../../../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../../../common/libcommon.a \
        ../../../iptlib/libiptlib.a \
    ../../../pflib/libfwbpf.a \
    ../../../cisco_lib/libfwbcisco.a \
    ../../../compiler_lib/libcompilerdriver.a
