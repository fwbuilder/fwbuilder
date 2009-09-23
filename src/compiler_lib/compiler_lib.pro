#-*- mode: makefile; tab-width: 4; -*-
#
# This library is separate from fwcompiler library because it has dependency
# on QT

include(../../qmake.inc)

TEMPLATE = lib

SOURCES	 =  CompilerDriver.cpp \
			CompilerDriver_compile.cpp \
			Configlet.cpp \
			interfaceProperties.cpp \
			linux24Interfaces.cpp \
			interfacePropertiesObjectFactory.cpp

HEADERS	 = ../../config.h \
			CompilerDriver.h \
			Configlet.h \
			interfaceProperties.h \
			linux24Interfaces.h \
			interfacePropertiesObjectFactory.h

!macx:LIBS  += $$LIBS_FWCOMPILER

CONFIG += staticlib

TARGET = compilerdriver

INSTALLS -= target
