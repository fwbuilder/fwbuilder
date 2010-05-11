include(../../../../qmake.inc)

QT += gui network

TEMPLATE	= app
LANGUAGE	= C++

INCLUDEPATH += ../../.ui ../../../compiler_lib/

HEADERS	+= RCSTest.h \
    ../../../../config.h \
    ../../RCS.h \
    ../../FWBTree.h \
    ../../global.h\
       ../../../compiler_lib/interfaceProperties.h \
       ../../../compiler_lib/interfacePropertiesObjectFactory.h \
       ../../../compiler_lib/Configlet.h \
       ../../../compiler_lib/linux24Interfaces.h \
       ../../../compiler_lib/bsdInterfaces.h \
       ../../../compiler_lib/iosInterfaces.h \
       ../../../compiler_lib/procurveInterfaces.h \
       ../../../compiler_lib/pixInterfaces.h

SOURCES	+=  main_RCS.cpp \
        RCSTest.cpp \
        ../../RCS.cpp \
        ../../FWBTree.cpp\
        ../../../compiler_lib/interfaceProperties.cpp \
        ../../../compiler_lib/interfacePropertiesObjectFactory.cpp \
        ../../../compiler_lib/Configlet.cpp \
        ../../../compiler_lib/linux24Interfaces.cpp \
        ../../../compiler_lib/bsdInterfaces.cpp \
        ../../../compiler_lib/iosInterfaces.cpp \
        ../../../compiler_lib/procurveInterfaces.cpp \
        ../../../compiler_lib/pixInterfaces.cpp

TARGET = RCSTest

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS

run_tests.commands = echo "Running tests..." && ./${TARGET} && echo "OK" || echo "FAILED"
run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests

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
