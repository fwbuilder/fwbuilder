set(junosacl_srcs
    junosacl.cpp
    )

add_executable(fwb_junosacl ${junosacl_srcs})

target_link_libraries(fwb_junosacl common fwbcisco fwbjuniper compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_junosacl iconv lzma ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_junosacl c dl util)
ENDIF()

target_compile_options(fwb_junosacl PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(junosacl.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_junosacl RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_junosacl Widgets)

