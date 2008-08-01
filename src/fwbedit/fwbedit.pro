#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#

TEMPLATE = app

SOURCES	 = fwbedit.cpp new_object.cpp repair_tree.cpp list_object.cpp
HEADERS	 = ../../config.h fwbedit.h

TARGET = fwbedit

QMAKE_COPY    = ../../install.sh -m 0755 -s

win32:CONFIG += console

