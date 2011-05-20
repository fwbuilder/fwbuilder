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

INCLUDEPATH += ../cisco_lib ../compiler_lib ../libfwbuilder/src
DEPENDPATH  += ../cisco_lib ../compiler_lib ../libfwbuilder/src

PRE_TARGETDEPS  = ../common/$$BINARY_SUBDIR/libcommon.a \
      ../cisco_lib/$$BINARY_SUBDIR/libfwbcisco.a \
      ../compiler_lib/$$BINARY_SUBDIR/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/$$BINARY_SUBDIR/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/$$BINARY_SUBDIR/libfwbuilder.a \

LIBS  += $$PRE_TARGETDEPS $$LIBS

TARGET = fwb_iosacl

