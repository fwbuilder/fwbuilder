set(nxosacl_srcs
    nxosacl.cpp)

add_executable(fwb_nxosacl ${nxosacl_srcs})

target_link_libraries(fwb_nxosacl common fwbcisco compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z Qt5::Core Qt5::Widgets)

IF (WIN32)
  target_link_libraries(fwb_nxosacl lzma iconv ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_nxosacl c ${CMAKE_DL_LIBS} util)
ENDIF()

target_compile_options(fwb_nxosacl PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(nxosacl.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_nxosacl RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

