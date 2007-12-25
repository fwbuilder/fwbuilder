#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
SOURCES	 = fwblookup.cpp
HEADERS	 = ../../config.h

TARGET = fwblookup

QMAKE_COPY    = ../../install.sh -m 0755 -s

win32:CONFIG += console

