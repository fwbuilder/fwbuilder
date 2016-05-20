#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES	 = \  
    CompilerDriver_junosacl.cpp \
    CompilerDriver_junosacl_run.cpp \
    OSConfigurator_junos.cpp \
    ../cisco_lib/PolicyCompiler_cisco.cpp \
    ../cisco_lib/Helper.cpp \
    PolicyCompiler_junosacl.cpp \
    PolicyCompiler_junosacl_writers.cpp \
    ../cisco_lib/NamedObjectsAndGroupsSupport.cpp \
    ../cisco_lib/NamedObject.cpp \
    ../cisco_lib/PolicyCompiler_cisco_acls.cpp \
    ../cisco_lib/BaseObjectGroup.cpp \
    ../cisco_lib/IOSObjectGroup.cpp \
    ../cisco_lib/NamedObjectsManager.cpp \
    ../cisco_lib/ACL.cpp \
    ../cisco_lib/NXOSObjectGroup.cpp \
    ../cisco_lib/PIXObjectGroup.cpp \
    ../cisco_lib/ASA8ObjectGroup.cpp

HEADERS	 = ../../config.h                   \
    CompilerDriver_junosacl.h \
    OSConfigurator_junos.h \
    PolicyCompiler_junosacl.h \
    ../cisco_lib/BaseObjectGroup.h

macx:LIBS  += $$LIBS_FWCOMPILER

INCLUDEPATH += ../compiler_lib ../libfwbuilder/src ..
DEPENDPATH  += ../compiler_lib ../libfwbuilder/src

win32:PRE_TARGETDEPS  = ../compiler_lib/release/libcompilerdriver.a
!win32:PRE_TARGETDEPS = ../compiler_lib/libcompilerdriver.a

CONFIG += staticlib

TARGET = fwbjuniper

INSTALLS -= target
