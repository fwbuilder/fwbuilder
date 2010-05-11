#-*- mode: makefile; tab-width: 4; -*-
# -------------------------------------------------
# Project created by QtCreator 2009-12-08T19:22:04
# -------------------------------------------------

include(../../../../qmake.inc)

QT += gui network

INCLUDEPATH += ../../.ui ../../../compiler_lib/
TARGET = UsageResolverTest
#CONFIG += console
#CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main_UsageResolverTest.cpp \
    UsageResolverTest.cpp \
    ../../UsageResolver.cpp \
    ../../FWBTree.cpp \
    ../../../compiler_lib/interfaceProperties.cpp \
    ../../../compiler_lib/interfacePropertiesObjectFactory.cpp \
    ../../../compiler_lib/Configlet.cpp \
    ../../../compiler_lib/linux24Interfaces.cpp \
    ../../../compiler_lib/bsdInterfaces.cpp \
    ../../../compiler_lib/iosInterfaces.cpp \
    ../../../compiler_lib/procurveInterfaces.cpp \
    ../../../compiler_lib/pixInterfaces.cpp

HEADERS += UsageResolverTest.h ../../UsageResolver.h ../../FWBTree.h \
   ../../../compiler_lib/interfaceProperties.h \
   ../../../compiler_lib/interfacePropertiesObjectFactory.h \
   ../../../compiler_lib/Configlet.h \
   ../../../compiler_lib/linux24Interfaces.h \
   ../../../compiler_lib/bsdInterfaces.h \
   ../../../compiler_lib/iosInterfaces.h \
   ../../../compiler_lib/procurveInterfaces.h \
   ../../../compiler_lib/pixInterfaces.h

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = ../../.obj
QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS
LIBS += -lcppunit

run_tests.commands = ./${TARGET}
run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests