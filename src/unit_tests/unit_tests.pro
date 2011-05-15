DOLLAR = $

libgui.target = ../libgui/libgui.a
libgui.commands = cd ../libgui && qmake -spec $$QMAKESPEC && make && cd -

build_tests.commands = ./unit_tests.sh make build_tests

run_tests.commands = ./unit_tests.sh make run_tests

clean_tests.commands = ./unit_tests.sh make clean

build_tests.depends = libgui
run_tests.depends = libgui

QMAKE_EXTRA_TARGETS += libgui run_tests clean_tests build_tests
