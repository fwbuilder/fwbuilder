#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)

SOURCES	 =  ipt.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY = ../../install.sh -m 0755 -s
}

win32: CONFIG += console

LIBS += $$LIBS_FWCOMPILER

INCLUDEPATH += ../iptlib ../compiler_lib/
DEPENDPATH   = ../iptlib ../compiler_lib

win32:LIBS  += ../iptlib/release/iptlib.lib ../compiler_lib/release/compilerdriver.lib 
!win32:LIBS += ../iptlib/libiptlib.a ../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../iptlib/release/iptlib.lib ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../iptlib/libiptlib.a ../compiler_lib/libcompilerdriver.a

TARGET = fwb_ipt


