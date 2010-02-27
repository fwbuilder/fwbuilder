#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

DOLLAR = $

build_tests.commands = ./unit_tests.sh make build
build_tests.depends = all

run_tests.commands = ./unit_tests.sh make run
run_tests.depends = all

testclean.commands = /usr/bin/env python testclean.py
tests.depends = run_tests

QMAKE_EXTRA_TARGETS += build_tests run_tests tests testclean

