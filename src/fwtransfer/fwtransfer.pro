#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES  = TransferDevice.cpp

HEADERS  = TransferDevice.h

CONFIG += staticlib

TARGET    = fwtransfer

INSTALLS -= target
