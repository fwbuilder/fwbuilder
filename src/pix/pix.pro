#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
#
# PACKAGE = fwbuilder-pix-$$FWB_VERSION
#
# QMAKE_CXXFLAGS_DEBUG += -DPACKAGE="\"$$PACKAGE\""
# QMAKE_CXXFLAGS_RELEASE += -DPACKAGE="\"$$PACKAGE\""

SOURCES	 =  NATCompiler_pix.cpp             \
			NATCompiler_pix_writers.cpp     \
			OSConfigurator_pix_os.cpp       \
			OSConfigurator_pix_os_fixups.cpp       \
			pix.cpp                         \
			PIXObjectGroup.cpp              \
			PolicyCompiler_pix.cpp          \
			PolicyCompiler_pix_writers.cpp  \
			PolicyCompiler_pix_v6_acls.cpp

HEADERS	 = ../../config.h                   \
			NATCompiler_pix.h               \
			OSConfigurator_pix_os.h         \
			PIXObjectGroup.h                \
			PolicyCompiler_pix.h            \
			../cisco_lib/PolicyCompiler_cisco.h \
			../cisco_lib/ACL.h 				\
			../cisco_lib/Helper.h 

!win32 {
	QMAKE_COPY    = ../../install.sh -m 0755 -s
}

win32:CONFIG += console

unix { !macx: CONFIG -= qt }

INCLUDEPATH += ../cisco_lib/

win32:LIBS  += ../cisco_lib/release/fwbcisco.lib
!win32:LIBS += ../cisco_lib/libfwbcisco.a

LIBS  += $$LIBS_FWCOMPILER

TARGET      = fwb_pix

