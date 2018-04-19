include(../tests_common.pri)
QT += gui widgets printsupport network

HEADERS = generatedScriptTestsIpfw.h

SOURCES = main_generatedScriptTestsIpfw.cpp \
	  generatedScriptTestsIpfw.cpp


TARGET = generatedScriptTestsIpfw

run_tests.commands = echo "Running tests..." && \
    rm -f *.fw *.conf && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }
