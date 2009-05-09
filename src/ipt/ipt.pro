#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
SOURCES	 =  ipt.cpp \
			MangleTableCompiler_ipt.cpp \
			NATCompiler_PrintRule.cpp \
			NATCompiler_PrintRuleIptRst.cpp \
			NATCompiler_PrintRuleIptRstEcho.cpp \
			NATCompiler_ipt.cpp \
			OSConfigurator_ipcop.cpp \
		    OSConfigurator_linux24.cpp \
			OSData.cpp \
			PolicyCompiler_PrintRule.cpp \
			PolicyCompiler_PrintRuleIptRst.cpp \
			PolicyCompiler_PrintRuleIptRstEcho.cpp \
			PolicyCompiler_ipt.cpp \
			PolicyCompiler_ipt_optimizer.cpp \
			RoutingCompiler_ipt.cpp \
			RoutingCompiler_ipt_writers.cpp \
			combinedAddress.cpp

HEADERS	 = ../../config.h \
			MangleTableCompiler_ipt.h \
			NATCompiler_ipt.h \
			OSConfigurator_ipcop.h \
			OSConfigurator_linux24.h \
			OSData.h \
			PolicyCompiler_ipt.h \
			RoutingCompiler_ipt.h \
			combinedAddress.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32: CONFIG += console

#unix { !macx: CONFIG -= qt }

TARGET      = fwb_ipt

LIBS  += $$LIBS_FWCOMPILER


