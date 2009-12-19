#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

tests.commands = cd src && cd compiler_lib && cd unit_tests && qmake && make run; \
                cd .. && cd .. && cd gui && unit_tests && qmake && make run

QMAKE_EXTRA_TARGETS += tests
