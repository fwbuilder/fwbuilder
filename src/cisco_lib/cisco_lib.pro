#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES	 =  PolicyCompiler_cisco.cpp \
			PolicyCompiler_cisco_acls.cpp \
			RoutingCompiler_cisco.cpp \
			RoutingCompiler_cisco_writers.cpp \
			ACL.cpp \
			Helper.cpp \
			OSConfigurator_ios.cpp             \
			CompilerDriver_iosacl.cpp          \
			CompilerDriver_iosacl_run.cpp      \
			PolicyCompiler_iosacl.cpp          \
			PolicyCompiler_iosacl_writers.cpp  \
			RoutingCompiler_iosacl.cpp         \
			RoutingCompiler_iosacl_writers.cpp \
			CompilerDriver_pix.cpp          \
			CompilerDriver_pix_run.cpp      \
			NATCompiler_pix.cpp             \
			NATCompiler_pix_writers.cpp     \
			OSConfigurator_pix_os.cpp       \
			OSConfigurator_pix_os_fixups.cpp       \
			PIXObjectGroup.cpp              \
			PolicyCompiler_pix.cpp          \
			PolicyCompiler_pix_writers.cpp  \
			PolicyCompiler_pix_v6_acls.cpp	\
			RoutingCompiler_pix.cpp         \
			RoutingCompiler_pix_writers.cpp

HEADERS	 = ../../config.h \
			ACL.h \
			Helper.h \
			PolicyCompiler_cisco.h \
			RoutingCompiler_cisco.h \
			CompilerDriver_iosacl.h            \
			OSConfigurator_ios.h               \
			PolicyCompiler_iosacl.h            \
			CompilerDriver_pix.h            \
			NATCompiler_pix.h               \
			OSConfigurator_pix_os.h         \
			PIXObjectGroup.h                \
			PolicyCompiler_pix.h            \
			RoutingCompiler_pix.h            \

macx:LIBS  += $$LIBS_FWCOMPILER

INCLUDEPATH += ../compiler_lib

win32:LIBS  += ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../compiler_lib/libcompilerdriver.a

CONFIG += staticlib

TARGET = fwbcisco

INSTALLS -= target
