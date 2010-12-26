#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = etc migration src

DOLLAR = $

build_tests.commands = ./unit_tests.sh make build_tests
build_tests.depends = all

run_tests.commands = ./unit_tests.sh make run_tests
run_tests.depends = all

clean_tests.commands = ./unit_tests.sh make clean_tests

tests.depends = run_tests

QMAKE_EXTRA_TARGETS += build_tests run_tests clean_tests tests
