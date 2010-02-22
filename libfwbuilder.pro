#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = etc doc migration src

DOLLAR = $

build_tests.commands = ./unit_tests.sh make 
build_tests.depends = all

run_tests.commands = ./unit_tests.sh make run
run_tests.depends = all

testclean.commands = ./unit_tests.sh make clean
tests.depends = run_tests

QMAKE_EXTRA_TARGETS += build_tests run_tests testclean tests