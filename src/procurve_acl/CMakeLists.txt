set(procurve_acl_srcs
    procurve_acl.cpp
    )

add_executable(fwb_procurve_acl ${procurve_acl_srcs})

target_link_libraries(fwb_procurve_acl common fwbcisco compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_procurve_acl lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_procurve_acl c dl util)
ENDIF()

target_compile_options(fwb_procurve_acl PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(procurve_acl.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_procurve_acl RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_procurve_acl Widgets)

