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


QMAKE_COPY    = ../../install.sh -m 0755 -s


win32:CONFIG += console

INCLUDEPATH += "../pflib"

win32:LIBS  += $$PREFIX/fwbpf.lib
!win32:LIBS += ../pflib/libfwbpf.a

LIBS  += $$LIBS_FWCOMPILER

TARGET      = fwb_pf
