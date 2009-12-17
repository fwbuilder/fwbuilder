#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
SOURCES	 =  ipfw.cpp

HEADERS	 = ../../config.h

!win32 {
    QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += ../common ../pflib ../compiler_lib
DEPENDPATH   = ../pflib

win32:LIBS  += ../common/release/common.lib ../pflib/release/fwbpf.lib ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../common/libcommon.a ../pflib/libfwbpf.a ../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../common/release/common.lib ../pflib/release/fwbpf.lib ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../common/libcommon.a ../pflib/libfwbpf.a ../compiler_lib/libcompilerdriver.a


LIBS  += $$LIBS_FWCOMPILER

TARGET      = fwb_ipfw
