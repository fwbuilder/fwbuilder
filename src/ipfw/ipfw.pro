#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
SOURCES	 =  ipfw.cpp

HEADERS	 = ../../config.h \
		../pflib/OSData.h \
		../pflib/NATCompiler_ipfw.h \
		../pflib/NATCompiler_pf.h \
		../pflib/OSConfigurator_freebsd.h \
		../pflib/OSConfigurator_macosx.h \
		../pflib/PolicyCompiler_ipfw.h \
		../pflib/PolicyCompiler_pf.h

!win32 {
    QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += "../pflib"
DEPENDPATH   = "../pflib"

win32:LIBS  += ../pflib/release/fwbpf.lib
!win32:LIBS += ../pflib/libfwbpf.a

LIBS  += $$LIBS_FWCOMPILER

unix: CONFIG -= qt

TARGET      = fwb_ipfw
