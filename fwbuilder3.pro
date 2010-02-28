#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

DOLLAR = $

build_tests.commands = ./unit_tests.sh make build_tests
build_tests.depends = all

run_tests.commands = ./unit_tests.sh make run_tests
run_tests.depends = all

testclean.commands = /usr/bin/env python testclean.py

tests.depends = run_tests

clean_tests.depends = testclean

QMAKE_EXTRA_TARGETS += build_tests run_tests clean_tests tests testclean

