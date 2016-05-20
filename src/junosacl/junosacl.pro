#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
QT -= gui
#
#
# PACKAGE = fwbuilder-junosacl-$$FWB_VERSION
#
# QMAKE_CXXFLAGS_DEBUG += -DPACKAGE="\"$$PACKAGE\""
# QMAKE_CXXFLAGS_RELEASE += -DPACKAGE="\"$$PACKAGE\""
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
SOURCES	 =  junosacl.cpp

HEADERS	 = ../../config.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += ../juniper_lib ../compiler_lib ../libfwbuilder/src
DEPENDPATH  += ../juniper_lib ../compiler_lib ../libfwbuilder/src

PRE_TARGETDEPS  = ../common/$$BINARY_SUBDIR/libcommon.a \
      ../juniper_lib/$$BINARY_SUBDIR/libfwbjuniper.a \
      ../compiler_lib/$$BINARY_SUBDIR/libcompilerdriver.a \
      ../libfwbuilder/src/fwcompiler/$$BINARY_SUBDIR/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/$$BINARY_SUBDIR/libfwbuilder.a \

LIBS  += $$PRE_TARGETDEPS $$LIBS

TARGET = fwb_junosacl

