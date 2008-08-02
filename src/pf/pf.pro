#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
SOURCES	 =  pf.cpp

HEADERS	 = ../../config.h \
		../pflib/OSData.h \
		../pflib/NATCompiler_pf.h \
		../pflib/OSConfigurator_openbsd.h \
		../pflib/PolicyCompiler_pf.h

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += "../pflib"

win32:LIBS  += ../pflib/release/fwbpf.lib
!win32:LIBS += ../pflib/libfwbpf.a

LIBS  += $$LIBS_FWCOMPILER

unix: CONFIG -= qt

TARGET      = fwb_pf
