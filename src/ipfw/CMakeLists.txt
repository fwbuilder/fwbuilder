set(ipfw_srcs
    ipfw.cpp)

add_executable(fwb_ipfw ${ipfw_srcs})

target_link_libraries(fwb_ipfw common fwbpf compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z Qt5::Core Qt5::Widgets)

IF (WIN32)
  target_link_libraries(fwb_ipfw lzma iconv ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_ipfw c ${CMAKE_DL_LIBS} util)
ENDIF()

target_compile_options(fwb_ipfw PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(ipfw.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_ipfw RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

