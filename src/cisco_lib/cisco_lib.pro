#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
#
SOURCES	 =  PolicyCompiler_cisco.cpp           \
			PolicyCompiler_cisco_acls.cpp      \
			NamedObjectsAndGroupsSupport.cpp   \
			NamedObjectsManager.cpp            \
			NamedObjectsManagerIOS.cpp         \
			NamedObjectsManagerPIX.cpp         \
			RoutingCompiler_cisco.cpp          \
			RoutingCompiler_cisco_writers.cpp  \
			splitByNetworkZonesForRE.cpp       \
			specialServices.cpp                \
			ACL.cpp                            \
			NamedObject.cpp                    \
			ASA8TwiceNatLogic.cpp              \
			Helper.cpp                         \
			inspectionProtocol.cpp             \
			InspectionClassMap.cpp             \
			OSConfigurator_ios.cpp             \
			CompilerDriver_iosacl.cpp          \
			CompilerDriver_iosacl_run.cpp      \
			PolicyCompiler_iosacl.cpp          \
			PolicyCompiler_iosacl_writers.cpp  \
			RoutingCompiler_iosacl.cpp         \
			RoutingCompiler_iosacl_writers.cpp \
			CompilerDriver_pix.cpp             \
			CompilerDriver_pix_run.cpp         \
			NATCompiler_pix.cpp                \
			NATCompiler_pix_find_translations.cpp      \
			NATCompiler_pix_writers.cpp        \
			NATCompiler_asa8.cpp               \
			NATCompiler_asa8_writers.cpp       \
			NATCompiler_pix_optimizers.cpp     \
			OSConfigurator_pix_os.cpp          \
			OSConfigurator_pix_os_fixups.cpp   \
			OSConfigurator_pix_os_inspectors.cpp       \
			OSConfigurator_pix_os_inspectors_pix8.cpp \
			CompilerDriver_procurve_acl.cpp	   \
			CompilerDriver_procurve_acl_run.cpp\
			OSConfigurator_procurve.cpp        \
			PolicyCompiler_procurve_acl.cpp    \
			PolicyCompiler_procurve_acl_writers.cpp  \
			RoutingCompiler_procurve_acl.cpp   \
			BaseObjectGroup.cpp              \
			PIXObjectGroup.cpp               \
			ASA8ObjectGroup.cpp              \
			IOSObjectGroup.cpp               \
			PolicyCompiler_pix.cpp           \
			PolicyCompiler_pix_writers.cpp   \
			PolicyCompiler_pix_v6_acls.cpp   \
			PolicyCompiler_pix_replace_translations.cpp   \
			RoutingCompiler_pix.cpp          \
			RoutingCompiler_pix_writers.cpp  \
			AutomaticRules_cisco.cpp         \
			AutomaticRules_iosacl.cpp

HEADERS	 = ../../config.h                   \
            PortRangeConverter.h            \
			splitByNetworkZonesForRE.h      \
			specialServices.h               \
			ACL.h                           \
			Helper.h                        \
			NamedObject.h                   \
			ASA8TwiceNatLogic.h             \
			NamedObjectsAndGroupsSupport.h  \
			NamedObjectsManager.h           \
			NamedObjectsManagerIOS.h        \
			NamedObjectsManagerPIX.h        \
			NamedObjectsManagerASA8.h       \
			inspectionProtocol.h            \
			InspectionClassMap.h            \
			PolicyCompiler_cisco.h          \
			RoutingCompiler_cisco.h         \
			CompilerDriver_iosacl.h         \
			OSConfigurator_ios.h            \
			PolicyCompiler_iosacl.h         \
			CompilerDriver_pix.h            \
			NATCompiler_pix.h               \
			NATCompiler_asa8.h              \
			OSConfigurator_pix_os.h         \
			CompilerDriver_procurve_acl.h   \
			OSConfigurator_procurve.h       \
			PolicyCompiler_procurve_acl.h   \
			RoutingCompiler_procurve_acl.h  \
			BaseObjectGroup.h               \
			PIXObjectGroup.h                \
			ASA8ObjectGroup.h               \
			IOSObjectGroup.h                \
			PolicyCompiler_pix.h            \
			RoutingCompiler_pix.h           \
			AutomaticRules_cisco.h          \
			AutomaticRules_iosacl.h


macx:LIBS  += $$LIBS_FWCOMPILER

INCLUDEPATH += ../compiler_lib ../libfwbuilder/src
DEPENDPATH  += ../compiler_lib ../libfwbuilder/src

win32:PRE_TARGETDEPS  = ../compiler_lib/release/libcompilerdriver.a
!win32:PRE_TARGETDEPS = ../compiler_lib/libcompilerdriver.a

CONFIG += staticlib

TARGET = fwbcisco

INSTALLS -= target
