#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#

TEMPLATE = app
QT += network

SOURCES	 = fwbedit.cpp new_object.cpp repair_tree.cpp list_object.cpp merge.cpp import.cpp
HEADERS	 = ../../config.h fwbedit.h upgradePredicate.h

INCLUDEPATH += ../libfwbuilder/src ../import ../compiler_lib/ ../libgui

win32:INCLUDEPATH += ../libgui/ui
!win32:INCLUDEPATH += ../libgui/.ui

TARGET = fwbedit

QMAKE_COPY    = ../../install.sh -m 0755 -s

DEPENDPATH = ../common \
             ../libfwbuilder/src/fwbuilder \
             ../libfwbuilder/src/fwcompiler

!win32:LIBS = ../common/libcommon.a \
    ../import/libimport.a \
    ../parsers/libfwbparser.a \
    ../compiler_lib/libcompilerdriver.a \
	../libfwbuilder/src/fwbuilder/libfwbuilder.a \
    ../libgui/libgui.a \
    $$ANTLR_LIBS \
    $$LIBS

!win32:PRE_TARGETDEPS = ../common/libcommon.a \
	../libfwbuilder/src/fwbuilder/libfwbuilder.a

win32:CONFIG += console

win32:LIBS += ../common/release/common.lib \
    ../import/release/import.lib \
    ../parsers/release/fwbparser.lib \
    ../compiler_lib/release/compilerdriver.lib \
	../libfwbuilder/src/fwbuilder/release/fwbuilder.lib \
    ../libgui/release/gui.lib \
    $$ANTLR_LIBS \
    $$LIBS

win32:PRE_TARGETDEPS = ../common/release/common.lib \
	../libfwbuilder/src/fwbuilder/release/fwbuilder.lib
