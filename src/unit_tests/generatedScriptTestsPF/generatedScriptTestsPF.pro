include(../tests_common.pri)
QT += gui network
HEADERS = generatedScriptTestsPF.h
SOURCES = main_generatedScriptTestsPF.cpp \
	  generatedScriptTestsPF.cpp

TARGET = generatedScriptTestsPF

run_tests.commands = echo "Running tests..." && \
    rm -f *.fw *.conf && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

