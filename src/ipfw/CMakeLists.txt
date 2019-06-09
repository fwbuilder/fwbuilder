project(ipfw)

set(ipfw_srcs
    ipfw.cpp)

add_executable(fwb_ipfw ${ipfw_srcs})

target_link_libraries(fwb_ipfw common fwbpf compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_ipfw lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_ipfw c dl util)
ENDIF()

target_compile_options(fwb_ipfw PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_ipfw RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_ipfw Widgets)

