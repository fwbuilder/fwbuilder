#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#

TEMPLATE = app
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

SOURCES	 = fwbedit.cpp new_object.cpp repair_tree.cpp list_object.cpp merge.cpp import.cpp
HEADERS	 = ../../config.h fwbedit.h upgradePredicate.h

INCLUDEPATH += ../libfwbuilder/src ../import ../compiler_lib ../libgui
DEPENDPATH +=  ../libfwbuilder/src ../import ../compiler_lib ../libgui

win32:INCLUDEPATH += ../libgui/ui
!win32:INCLUDEPATH += ../libgui/.ui

TARGET = fwbedit

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

PRE_TARGETDEPS  = ../common/$$BINARY_SUBDIR/libcommon.a \
      ../import/$$BINARY_SUBDIR/libimport.a \
      ../parsers/$$BINARY_SUBDIR/libfwbparser.a \
      ../compiler_lib/$$BINARY_SUBDIR/libcompilerdriver.a \
      ../libgui/$$BINARY_SUBDIR/libgui.a \
      ../libfwbuilder/src/fwcompiler/$$BINARY_SUBDIR/libfwcompiler.a \
      ../libfwbuilder/src/fwbuilder/$$BINARY_SUBDIR/libfwbuilder.a \

LIBS  += $$PRE_TARGETDEPS $$ANTLR_LIBS $$LIBS

win32:CONFIG += console


