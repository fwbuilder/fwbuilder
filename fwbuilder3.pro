#-*- mode: makefile; tab-width: 4; -*-
#

TEMPLATE = subdirs

SUBDIRS = src doc

DOLLAR = $

build_tests.commands = for directory in `find . -name unit_tests`; \
                 do \
                    home=`pwd`; \
                    echo; \
                    echo "Building test in "$${DOLLAR}$${DOLLAR}directory; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    make; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done
build_tests.depends = all

run_tests.commands = for directory in `find . -name unit_tests`; \
                 do \
                    home=`pwd`; \
                    echo; \
                    echo "Running test in "$${DOLLAR}$${DOLLAR}directory; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    make run; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done
run_tests.depends = all

testclean.commands = for directory in `find . -name unit_tests`; \
                 do \
                    home=`pwd`; \
                    echo; \
                    echo "Cleaning test files in "$${DOLLAR}$${DOLLAR}directory; \
                    cd $${DOLLAR}$${DOLLAR}directory; \
                    qmake -spec $${DOLLAR}$${DOLLAR}QMAKESPEC; \
                    make clean; \
                    cd $${DOLLAR}$${DOLLAR}home; \
                done

tests.depends = run_tests

QMAKE_EXTRA_TARGETS += build_tests run_tests tests testclean

