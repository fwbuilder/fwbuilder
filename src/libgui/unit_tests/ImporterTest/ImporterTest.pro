include(../../../../qmake.inc)
QT += gui network
TEMPLATE = app
LANGUAGE = C++
CONFIG += console

INCLUDEPATH += ../../.. ../.. ../../.ui ../../../compiler_lib/

HEADERS += ImporterTest.h

SOURCES += main_ImporterTest.cpp \
        ImporterTest.cpp

RESOURCES += ../../MainRes.qrc
LIBS += ../../libgui.a
TARGET = ImporterTest
CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS
LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS

INCLUDEPATH += $$ANTLR_INCLUDEPATH
LIBS += ../../$$FWBPARSER_LIB $$ANTLR_LIBS
DEFINES += $$ANTLR_DEFINES

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

run_tests.commands = echo "Running tests..." && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests
