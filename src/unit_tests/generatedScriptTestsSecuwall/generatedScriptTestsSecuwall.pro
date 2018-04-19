include(../tests_common.pri)
QT += gui widgets printsupport network
HEADERS = generatedScriptTestsSecuwall.h
SOURCES = main_generatedScriptTestsSecuwall.cpp \
	  generatedScriptTestsSecuwall.cpp
TARGET = generatedScriptTestsSecuwall

run_tests.commands = echo "Running tests..." && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

build_tests.commands = @tar -zxf ./ref.secuwall-1.tar.gz

clean_tests.commands = @rm -rf ./secuwall-* && \
    rm -rf ./ref.secuwall-1 && \
	rm -f ${TARGET}

run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests
