project(pf)

set(pf_srcs
    pf.cpp)

add_executable(fwb_pf ${pf_srcs})

target_link_libraries(fwb_pf common fwbpf compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_pf lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_pf c dl util)
ENDIF()

target_compile_options(fwb_pf PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_pf RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_pf Widgets)

