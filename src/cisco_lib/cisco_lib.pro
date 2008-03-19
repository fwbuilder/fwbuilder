#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES	 =  PolicyCompiler_cisco.cpp \
			PolicyCompiler_cisco_acls.cpp \
			ACL.cpp \
			Helper.cpp

HEADERS	 = ../../config.h \
			ACL.h \
			Helper.h \
			PolicyCompiler_cisco.h

!macx:LIBS  += $$LIBS_FWCOMPILER
# macx:LIBS   += -L../../../libfwbuilder2-2.0.0/src/fwcompiler -lfwcompiler-2.0 

CONFIG += staticlib

TARGET = fwbcisco

INSTALLS -= target
