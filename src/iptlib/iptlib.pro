#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)


TEMPLATE = lib

SOURCES	 =  CompilerDriver_ipt.cpp \
			CompilerDriver_ipt_cluster.cpp \
			CompilerDriver_ipt_run.cpp \
			CompilerDriver_ipt_policy.cpp \
			CompilerDriver_ipt_nat.cpp \
			MangleTableCompiler_ipt.cpp \
			NATCompiler_PrintRule.cpp \
			NATCompiler_PrintRuleIptRst.cpp \
			NATCompiler_PrintRuleIptRstEcho.cpp \
			NATCompiler_ipt.cpp \
			OSConfigurator_ipcop.cpp \
		    OSConfigurator_linux24.cpp \
		    OSConfigurator_linux24_interfaces.cpp \
		    OSConfigurator_secuwall.cpp \
			OSData.cpp \
			PolicyCompiler_PrintRule.cpp \
			PolicyCompiler_PrintRuleIptRst.cpp \
			PolicyCompiler_PrintRuleIptRstEcho.cpp \
			PolicyCompiler_ipt.cpp \
			PolicyCompiler_ipt_optimizer.cpp \
			PolicyCompiler_secuwall.cpp \
			RoutingCompiler_ipt.cpp \
			RoutingCompiler_ipt_writers.cpp \
			combinedAddress.cpp

HEADERS	 = ../../config.h \
			CompilerDriver_ipt.h \
			MangleTableCompiler_ipt.h \
			NATCompiler_ipt.h \
			OSConfigurator_ipcop.h \
			OSConfigurator_linux24.h \
			OSConfigurator_secuwall.h \
			OSData.h \
			PolicyCompiler_ipt.h \
			PolicyCompiler_secuwall.h \
			RoutingCompiler_ipt.h \
			combinedAddress.h

CONFIG += staticlib

INCLUDEPATH += ../compiler_lib/
DEPENDPATH   = ../compiler_lib

win32:LIBS  += ../compiler_lib/release/compilerdriver.lib
!win32:LIBS += ../compiler_lib/libcompilerdriver.a

win32:PRE_TARGETDEPS  = ../compiler_lib/release/compilerdriver.lib
!win32:PRE_TARGETDEPS = ../compiler_lib/libcompilerdriver.a

LIBS += $$LIBS_FWCOMPILER

TARGET = iptlib

INSTALLS -= target


