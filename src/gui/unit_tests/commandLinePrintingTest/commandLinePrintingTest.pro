TEMPLATE	= app
LANGUAGE = bash
CONFIG += console
CONFIG -= app_bundle

TARGET = commandLinePrintingTest
run_tests.commands = echo "Running tests..." && ./${TARGET} && echo "OK" || echo "FAILED"
run_tests.depends = build_tests
clean_tests.commands = rm -f print.pdf
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests
