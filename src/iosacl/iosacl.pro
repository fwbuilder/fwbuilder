#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
# PACKAGE = fwbuilder-iosacl-$$FWB_VERSION
#
# QMAKE_CXXFLAGS_DEBUG += -DPACKAGE="\"$$PACKAGE\""
# QMAKE_CXXFLAGS_RELEASE += -DPACKAGE="\"$$PACKAGE\""

SOURCES	 =  OSConfigurator_ios.cpp             \
			iosacl.cpp                         \
			PolicyCompiler_iosacl.cpp          \
			PolicyCompiler_iosacl_writers.cpp  \

HEADERS	 = ../../config.h                      \
			OSConfigurator_ios.h               \
			PolicyCompiler_iosacl.h            \
			../cisco_lib/PolicyCompiler_cisco.h \
			../cisco_lib/ACL.h 				    \
			../cisco_lib/Helper.h 

!win32 {
	QMAKE_COPY    = /usr/bin/install -m 0755 -s
}

win32:CONFIG += console

INCLUDEPATH += ../cisco_lib/

win32:LIBS  += ../cisco_lib/release/fwbcisco.lib
!win32:LIBS += ../cisco_lib/libfwbcisco.a

LIBS  += $$LIBS_FWCOMPILER

unix: CONFIG -= qt

TARGET      = fwb_iosacl

