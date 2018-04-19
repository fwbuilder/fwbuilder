include(../tests_common.pri)
QT += gui widgets printsupport network

HEADERS = generatedScriptTestsIpfilter.h

SOURCES = main_generatedScriptTestsIpfilter.cpp \
	  generatedScriptTestsIpfilter.cpp


TARGET = generatedScriptTestsIpfilter

run_tests.commands = echo "Running tests..." && \
    rm -f *.fw *.conf && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }
