
include(../../../qmake.inc)

# exists(../../qmake.inc):include( ../../qmake.inc)

TEMPLATE = app
TARGET =
DEPENDPATH += .

SOURCES	 =  ../CompilerDriver.cpp \
	../CompilerDriver_compile.cpp \
	../CompilerDriver_generators.cpp \
	../Configlet.cpp \
	../interfaceProperties.cpp \
	../linux24Interfaces.cpp \
	../bsdInterfaces.cpp \
	../iosInterfaces.cpp \
	../pixInterfaces.cpp \
	../interfacePropertiesObjectFactory.cpp

HEADERS	 = ../../../config.h \
	../CompilerDriver.h \
	../Configlet.h \
	../interfaceProperties.h \
	../linux24Interfaces.h \
	../bsdInterfaces.h \
	../iosInterfaces.h \
	../pixInterfaces.h \
	../interfacePropertiesObjectFactory.h

# Actual tests
HEADERS += interfacePropertiesTest.h
SOURCES += tests_main.cpp interfacePropertiesTest.cpp

CONFIG -= release
CONFIG += debug
OBJECTS_DIR = .
QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
QMAKE_CLEAN = *.gc??

LIBS += $$LIBS_FWCOMPILER $$LIBS_FWBUILDER
LIBS += -lgcov -lcppunit
