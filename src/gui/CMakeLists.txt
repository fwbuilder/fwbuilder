project(fwbuilder-gui)

set(fwbuilder-gui_srcs
    main.cpp)

add_executable(fwbuilder-gui ${fwbuilder-gui_srcs})

target_link_libraries(fwbuilder-gui gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 m xslt z crypto pthread)

IF (WIN32)
  target_link_libraries(fwbuilder-gui lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwbuilder-gui c dl util)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(fwbuilder-gui netsnmp)
ENDIF()

target_compile_options(fwbuilder-gui PRIVATE ${CXX_DEFAULT_FLAGS})

set_target_properties(fwbuilder-gui PROPERTIES OUTPUT_NAME "fwbuilder")

install(TARGETS fwbuilder-gui RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwbuilder-gui Widgets Network PrintSupport)

