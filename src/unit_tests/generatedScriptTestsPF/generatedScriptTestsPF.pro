include(../tests_common.pri)
QT += gui widgets printsupport network
HEADERS = generatedScriptTestsPF.h
SOURCES = main_generatedScriptTestsPF.cpp \
	  generatedScriptTestsPF.cpp

TARGET = generatedScriptTestsPF

run_tests.commands = echo "Running tests..." && \
    rm -f *.fw *.conf tmp/*.fw tmp/*.conf && \
    mkdir -p tmp && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

