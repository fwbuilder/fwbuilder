#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)

QT += network

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS

CONFIG -= release
CONFIG += debug

INCLUDEPATH += ../../.. \
               ../.. \
               ../../common \
               ../../parsers \
               ../../import \
               ../../compiler_lib \
               ../../libgui \
               ../../libgui/.ui \
               ../../iptlib \
               ../../pflib \
               ../../cisco_lib \
               ../../juniper_lib \
               ../../libfwbuilder/src

DEPENDPATH  += ../../.. \
               ../.. \
               ../../common \
               ../../parsers \
               ../../import \
               ../../compiler_lib \
               ../../libgui \
               ../../libgui/.ui \
               ../../iptlib \
               ../../pflib \
               ../../cisco_lib \
               ../../juniper_lib \
               ../../libfwbuilder/src

run_tests.commands = echo "Running tests..."; ./${TARGET}
run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = clean 
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests

INCLUDEPATH += $$ANTLR_INCLUDEPATH
DEPENDPATH  += $$ANTLR_INCLUDEPATH
DEFINES += $$ANTLR_DEFINES

DEPENDPATH = ../../common \
    ../../iptlib \
    ../../pflib \
    ../../cisco_lib/ \
    ../../juniper_lib/ \
    ../../compiler_lib \
    ../../libgui \
    ../../libfwbuilder/src/fwbuilder \
    ../../libfwbuilder/src/fwcompiler

STATIC_LIBS += \
    ../main/libtest_main.a \
    ../../libgui/libgui.a \
    ../../import/libimport.a \
    ../../common/libcommon.a \
    ../../iptlib/libiptlib.a \
    ../../pflib/libfwbpf.a \
    ../../cisco_lib/libfwbcisco.a \
    ../../juniper_lib/libfwbjuniper.a \
    ../../compiler_lib/libcompilerdriver.a \
    ../../libfwbuilder/src/fwcompiler/libfwcompiler.a \
    ../../libfwbuilder/src/fwbuilder/libfwbuilder.a \
    ../../parsers/libfwbparser.a \
    $$ANTLR_LIBS \
    $$CPPUNIT_LIBS

PRE_TARGETDEPS += ../../common/libcommon.a \
    ../main/libtest_main.a \
    ../../iptlib/libiptlib.a \
    ../../pflib/libfwbpf.a \
    ../../cisco_lib/libfwbcisco.a \
    ../../juniper_lib/libfwbjuniper.a \
    ../../compiler_lib/libcompilerdriver.a \
    ../../parsers/libfwbparser.a \
    ../../import/libimport.a \
    ../../libfwbuilder/src/fwcompiler/libfwcompiler.a \
    ../../libfwbuilder/src/fwbuilder/libfwbuilder.a \
    $$ANTLR_LIBS

LIBS = $$STATIC_LIBS $$LIBS

