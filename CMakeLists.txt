cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9"
    CACHE STRING "Minimum macOS deployment version"
)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project(firewallbuilder LANGUAGES CXX C)

### Version variables and include file

include(VERSION)
configure_file("${CMAKE_SOURCE_DIR}/src/version.h.in" "${CMAKE_BINARY_DIR}/src/version.h" @ONLY)
include_directories(${CMAKE_BINARY_DIR}/src)

### Build type

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: Debug or Release."
        FORCE)
endif()
include( CTest )

### Dependencies

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)

find_package(NetSNMP)
if(NETSNMP_FOUND)
    add_definitions(-DHAVE_LIBSNMP -DNET_SNMP -DHAVE_SNPRINT_OBJID)
endif()

### Compiler options

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CXX_DEFAULT_FLAGS # clang/GCC warnings
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall;-Wextra;-Werror;-Wno-overloaded-virtual>
                         # MSVC warnings
    $<$<CXX_COMPILER_ID:MSVC>:/WX;/W4>)

add_definitions(-D__STDC_FORMAT_MACROS)

if(WIN32)
  #add_definitions(-DIN_LIBXML) # Enable for static build
  add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

### Install

if(APPLE)
  set(APP_ICON_FILE fwbuilder-macosx.icns)
  set(APP_ICON "${PROJECT_SOURCE_DIR}/src/gui/${APP_ICON_FILE}")
  set(APPLICATION_NAME fwbuilder)
  set(MACOSX_BUNDLE_ICON_FILE ${APP_ICON_FILE})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_GENERATION})
  set(MACOSX_BUNDLE_VERSION ${VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION})
  set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
  set(FWB_INSTALL_DATADIR ${APPLICATION_NAME}.app/Contents/Resources)
  set(FWB_INSTALL_DOCDIR ${FWB_INSTALL_DATADIR}/doc)
  set(FWB_INSTALL_BINDIR ${APPLICATION_NAME}.app/Contents/MacOS)
elseif(WIN32)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
  set(FWB_INSTALL_DATADIR release/resources)
  set(FWB_INSTALL_DOCDIR release/doc)
  set(FWB_INSTALL_BINDIR release)
else()
  include(GNUInstallDirs)
  set(FWB_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/fwbuilder-${PROJECT_VERSION})
  set(FWB_INSTALL_DOCDIR ${CMAKE_INSTALL_DATADIR}/doc/fwbuilder-${PROJECT_VERSION})
  set(FWB_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
endif()

### Sources

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
add_subdirectory(src/unit_tests)

### Packaging
add_subdirectory(packaging)
