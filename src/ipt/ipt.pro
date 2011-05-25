#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)

SOURCES	 =  ipt.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY = ../../install.sh -m 0755 -s
}

win32: CONFIG += console

INCLUDEPATH += ../iptlib ../compiler_lib ../libfwbuilder/src
DEPENDPATH  += ../iptlib ../compiler_lib ../libfwbuilder/src

PRE_TARGETDEPS  = ../common/$$BINARY_SUBDIR/libcommon.a \
      ../iptlib/$$BINARY_SUBDIR/libiptlib.a \
      ../compiler_lib/$$BINARY_SUBDIR/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/$$BINARY_SUBDIR/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/$$BINARY_SUBDIR/libfwbuilder.a \

LIBS  += $$PRE_TARGETDEPS $$LIBS

TARGET = fwb_ipt


