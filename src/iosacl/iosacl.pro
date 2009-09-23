#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
# PACKAGE = fwbuilder-iosacl-$$FWB_VERSION
#
# QMAKE_CXXFLAGS_DEBUG += -DPACKAGE="\"$$PACKAGE\""
# QMAKE_CXXFLAGS_RELEASE += -DPACKAGE="\"$$PACKAGE\""

SOURCES	 =  iosacl.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += ../cisco_lib/ ../compiler_lib

win32:LIBS  += ../cisco_lib/release/fwbcisco.lib  ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../cisco_lib/libfwbcisco.a ../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../cisco_lib/release/fwbcisco.lib ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../cisco_lib/libfwbcisco.a ../compiler_lib/libcompilerdriver.a


LIBS  += $$LIBS_FWCOMPILER

TARGET      = fwb_iosacl

