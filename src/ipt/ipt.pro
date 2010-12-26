#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)

SOURCES	 =  ipt.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY = ../../install.sh -m 0755 -s
}

win32: CONFIG += console

INCLUDEPATH += ../common ../iptlib ../compiler_lib/ ../libfwbuilder/src

DEPENDPATH   = ../common ../iptlib ../compiler_lib

win32:LIBS  += ../common/release/common.lib \
      ../iptlib/release/iptlib.lib \
      ../compiler_lib/release/compilerdriver.lib  \
      ../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
      ../libfwbuilder/src/fwcompiler/release/fwcompiler.lib

!win32:LIBS += ../common/libcommon.a \
      ../iptlib/libiptlib.a \
      ../compiler_lib/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/libfwbuilder.a

win32:PRE_TARGETDEPS  = ../common/release/common.lib \
      ../iptlib/release/iptlib.lib \
      ../compiler_lib/release/compilerdriver.lib \
      ../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
      ../libfwbuilder/src/fwcompiler/release/fwcompiler.lib

!win32:PRE_TARGETDEPS = ../common/libcommon.a \
      ../iptlib/libiptlib.a \
      ../compiler_lib/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/libfwbuilder.a

TARGET = fwb_ipt


