include(../../../../qmake.inc)

QT += gui network

TEMPLATE	= app
LANGUAGE	= C++

INCLUDEPATH += ../../.ui ../../../compiler_lib/

HEADERS	+= RCSTest.h

SOURCES	+=  main_RCS.cpp \
        RCSTest.cpp

TARGET = RCSTest

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
RESOURCES += ../../MainRes.qrc
LIBS += ../../libgui.a
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
