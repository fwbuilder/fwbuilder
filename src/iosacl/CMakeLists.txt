set(iosacl_srcs
    iosacl.cpp)

add_executable(fwb_iosacl ${iosacl_srcs})

target_link_libraries(fwb_iosacl common fwbcisco compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z Qt5::Core Qt5::Widgets)

IF (WIN32)
  target_link_libraries(fwb_iosacl lzma iconv ws2_32)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_iosacl c ${CMAKE_DL_LIBS} util)
ENDIF()

target_compile_options(fwb_iosacl PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(iosacl.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_iosacl RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

