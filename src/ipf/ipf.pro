#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
SOURCES	 =  ipf.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

# unix { !macx: CONFIG -= qt }

INCLUDEPATH += ../common ../pflib ../compiler_lib ../libfwbuilder/src

DEPENDPATH   = ../pflib

win32:LIBS  += ../common/release/common.lib \
      ../pflib/release/fwbpf.lib \
      ../compiler_lib/release/compilerdriver.lib \
      ../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
      ../libfwbuilder/src/fwcompiler/release/fwcompiler.lib

!win32:LIBS += ../common/libcommon.a \
      ../pflib/libfwbpf.a \
      ../compiler_lib/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/libfwbuilder.a

win32:PRE_TARGETDEPS  = ../common/release/common.lib \
      ../pflib/release/fwbpf.lib \
      ../compiler_lib/release/compilerdriver.lib \
      ../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
      ../libfwbuilder/src/fwcompiler/release/fwcompiler.lib

!win32:PRE_TARGETDEPS = ../common/libcommon.a \
      ../pflib/libfwbpf.a \
      ../compiler_lib/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/libfwbuilder.a

TARGET = fwb_ipf
