#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#

TEMPLATE = app

SOURCES	 = fwbedit.cpp new_object.cpp repair_tree.cpp list_object.cpp merge.cpp
HEADERS	 = ../../config.h fwbedit.h upgradePredicate.h

TARGET = fwbedit

QMAKE_COPY    = ../../install.sh -m 0755 -s

DEPENDPATH = ../common

!win32:LIBS += ../common/libcommon.a
!win32:PRE_TARGETDEPS = ../common/libcommon.a

win32:CONFIG += console

win32:LIBS += ../common/libcommon.lib
win32:PRE_TARGETDEPS = ../common/libcommon.lib
