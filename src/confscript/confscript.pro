#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#

# TEMPLATE = app

win32 {
	CONFIG -= embed_manifest_exe
}

QMAKE_RUN_CC = @echo
QMAKE_RUN_CXX = @echo
QMAKE_LINK = @echo
QMAKE_LIB = @echo

TARGET = confscript

script.files = libfwbuilder-config-4
script.path = "$$prefix/bin"

INSTALLS -= target
INSTALLS += script
