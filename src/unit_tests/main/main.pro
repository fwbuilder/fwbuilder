# -*- mode: makefile; tab-width: 4; -*-

include(../tests_common.pri)

QT += testlib network gui

TEMPLATE = lib

CONFIG += staticlib

TARGET = test_main

INSTALLS -= target

SOURCES += main.cpp

run_tests.commands = echo ""
run_tests.depends = build_tests
