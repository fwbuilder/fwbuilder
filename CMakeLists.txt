cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(firewallbuilder LANGUAGES CXX C)
include(VERSION)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(LibXml2 REQUIRED)

include(GNUInstallDirs)
set(FWB_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/fwbuilder-${PROJECT_VERSION})
set(FWB_INSTALL_DOCDIR ${CMAKE_INSTALL_DATADIR}/doc/fwbuilder-${PROJECT_VERSION})
set(FWB_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CXX_DEFAULT_FLAGS # clang/GCC warnings
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall;-Wextra;-Werror;-Wno-overloaded-virtual>
                         # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:/WX;/W4>)

add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-DGENERATION="${PROJECT_GENERATION}")
add_definitions(-DVERSION="${PROJECT_VERSION}")
add_definitions(-DFS_SEPARATOR="/")
add_definitions(-DFWBUILDER_XML_VERSION="${FWBUILDER_XML_VERSION}")
add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")
add_definitions(-DRES_DIR="${CMAKE_INSTALL_PREFIX}/${FWB_INSTALL_DATADIR}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: Debug or Release."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

FIND_PACKAGE(NetSNMP)
IF (NETSNMP_FOUND)
    add_definitions(-DHAVE_LIBSNMP -DNET_SNMP -DHAVE_SNPRINT_OBJID)
ENDIF()

add_subdirectory(src/libfwbuilder/src/fwcompiler)
add_subdirectory(src/libfwbuilder/src/fwbuilder)
add_subdirectory(src/common)
add_subdirectory(src/antlr)
add_subdirectory(src/compiler_lib)
add_subdirectory(src/cisco_lib)
add_subdirectory(src/juniper_lib)
add_subdirectory(src/pflib)
add_subdirectory(src/iptlib)
add_subdirectory(src/ipt)
add_subdirectory(src/pf)
add_subdirectory(src/ipf)
add_subdirectory(src/ipfw)
add_subdirectory(src/iosacl)
add_subdirectory(src/nxosacl)
add_subdirectory(src/junosacl)
add_subdirectory(src/pix)
add_subdirectory(src/procurve_acl)
add_subdirectory(src/libgui)
add_subdirectory(src/import)
add_subdirectory(src/parsers)
add_subdirectory(src/gui)
add_subdirectory(src/fwbedit)
add_subdirectory(src/libfwbuilder/etc)
add_subdirectory(src/libfwbuilder/migration)
add_subdirectory(src/res)
add_subdirectory(doc)

