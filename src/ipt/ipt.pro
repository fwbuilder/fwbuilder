#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
SOURCES	 =  ipt.cpp \
		    OSConfigurator_linux24.cpp \
			OSData.cpp \
			combinedAddress.cpp \
			PolicyCompiler_ipt.cpp \
			PolicyCompiler_PrintRule.cpp \
			PolicyCompiler_PrintRuleIptRst.cpp \
			PolicyCompiler_PrintRuleIptRstEcho.cpp \
			PolicyCompiler_ipt_optimizer.cpp \
			NATCompiler_ipt.cpp \
			NATCompiler_PrintRule.cpp \
			NATCompiler_PrintRuleIptRst.cpp \
			NATCompiler_PrintRuleIptRstEcho.cpp \
			MangleTableCompiler_ipt.cpp \
			RoutingCompiler_ipt.cpp \
			RoutingCompiler_ipt_writers.cpp

HEADERS	 = ../../config.h \
			combinedAddress.h \
			PolicyCompiler_ipt.h \
			NATCompiler_ipt.h \
			MangleTableCompiler_ipt.h \
			RoutingCompiler_ipt.h \
			OSConfigurator_linux24.h \
			OSData.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32: CONFIG += console

#unix { !macx: CONFIG -= qt }

TARGET      = fwb_ipt

LIBS  += $$LIBS_FWCOMPILER


