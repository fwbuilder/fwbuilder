set(pf_srcs
    pf.cpp)

add_executable(fwb_pf ${pf_srcs})

target_link_libraries(fwb_pf common fwbpf compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_pf lzma iconv ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_pf c dl util)
ENDIF()

target_compile_options(fwb_pf PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(pf.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_pf RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_pf Widgets)

