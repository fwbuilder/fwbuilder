find_package(OpenSSL REQUIRED)

set(fwbuilder-gui_srcs
    main.cpp)

if(APPLE)
  set_source_files_properties(${APP_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  add_executable(fwbuilder-gui MACOSX_BUNDLE ${fwbuilder-gui_srcs} ${APP_ICON_FILE})
elseif(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
  add_executable(fwbuilder-gui ${fwbuilder-gui_srcs} fwbuilder-windows.rc)
else()
  add_executable(fwbuilder-gui ${fwbuilder-gui_srcs})
endif()

target_link_libraries(fwbuilder-gui gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 m xslt z OpenSSL::Crypto pthread Qt5::Core Qt5::Widgets Qt5::Network Qt5::PrintSupport)

IF (WIN32)
  target_link_libraries(fwbuilder-gui lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwbuilder-gui c ${CMAKE_DL_LIBS} util)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(fwbuilder-gui netsnmp)
ENDIF()

target_compile_options(fwbuilder-gui PRIVATE ${CXX_DEFAULT_FLAGS})

set_target_properties(fwbuilder-gui PROPERTIES OUTPUT_NAME "fwbuilder")

if(APPLE)
  install(TARGETS fwbuilder-gui DESTINATION .)
else()
  install(TARGETS fwbuilder-gui DESTINATION ${FWB_INSTALL_BINDIR} COMPONENT Runtime)
endif()

