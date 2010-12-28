# -*- mode: makefile; tab-width: 4; -*-

include(../../qmake.inc)

OBJECTS_DIR = .obj
MOC_DIR = .moc

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS

CONFIG -= release
CONFIG += debug

INCLUDEPATH += ../../..
INCLUDEPATH += ../..
INCLUDEPATH += ../../libfwbuilder/src
INCLUDEPATH += ../../libgui
INCLUDEPATH += ../../libgui/.ui

QT += testlib network gui

TEMPLATE = lib

CONFIG += staticlib

TARGET = test_main

INSTALLS -= target

SOURCES += main.cpp

run_tests.commands = echo ""
run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = clean 
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests


