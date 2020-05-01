set(ipf_srcs
    ipf.cpp)

add_executable(fwb_ipf ${ipf_srcs})

target_link_libraries(fwb_ipf common fwbpf compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_ipf lzma iconv ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_ipf c dl util)
ENDIF()

target_compile_options(fwb_ipf PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(ipf.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_ipf RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_ipf Widgets)

