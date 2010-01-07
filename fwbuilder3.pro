#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

DOLLAR = $

tests.commands = for directory in `find . -name unit_tests`; \
                 do \
                    home=`pwd`; \
                    echo; \
                    echo "Running test in "$${DOLLAR}$${DOLLAR}directory; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    make run || exit 1; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done

tests.depends = all

QMAKE_EXTRA_TARGETS += tests
