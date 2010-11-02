include(../../../../qmake.inc)
QT += gui network
TEMPLATE = app
LANGUAGE = C++
CONFIG += console

INCLUDEPATH += ../../../.. \
		../.. \
		../../../compiler_lib/ \
		../../../common

DEPENDPATH   = ../../../.. \
		../.. \
		../../../compiler_lib/ \
		../../../common

HEADERS = generatedScriptTestsLinux.h

SOURCES = main_generatedScriptTestsLinux.cpp \
	  generatedScriptTestsLinux.cpp


TARGET = generatedScriptTestsLinux
CONFIG -= release
CONFIG += debug
win32:CONFIG += console

QMAKE_CXXFLAGS += $$CPPUNIT_CFLAGS

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER $$CPPUNIT_LIBS


win32:LIBS  += ../../../common/release/common.lib \
		../../release/iptlib.lib \
		../../../compiler_lib/release/compilerdriver.lib 

!win32:LIBS += ../../../common/libcommon.a \
		../../libiptlib.a \
		../../../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../../../common/release/common.lib \
			../../release/iptlib.lib \
			../compiler_lib/release/compilerdriver.lib

!win32:PRE_TARGETDEPS = ../../../common/libcommon.a \
			../../libiptlib.a \
			../../../compiler_lib/libcompilerdriver.a

run_tests.commands = echo "Running tests..." && \
    rm -f *.fw && \
    ./${TARGET} && \
    echo "OK" || { echo "FAILED"; exit 1; }

run_tests.depends = build_tests
build_tests.depends = all
clean_tests.depends = all
QMAKE_EXTRA_TARGETS += run_tests build_tests clean_tests
