#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

DOLLAR = $

build_tests.commands = cd src/unit_tests; make build_tests; cd -
build_tests.depends = all

run_tests.commands = cd src/unit_tests; make run_tests; cd -
run_tests.depends = all

tests.depends = run_tests

clean_tests.commands = cd src/unit_tests; make clean_tests; cd -

QMAKE_EXTRA_TARGETS += build_tests run_tests clean_tests tests 

