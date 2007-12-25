#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
SOURCES	 = fwbedit.cpp
HEADERS	 = ../../config.h

TARGET = fwbedit

QMAKE_COPY    = ../../install.sh -m 0755 -s

win32:CONFIG += console

