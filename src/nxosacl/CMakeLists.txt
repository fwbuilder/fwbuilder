project(nxosacl)

set(nxosacl_srcs
    nxosacl.cpp
    )

add_executable(fwb_nxosacl ${nxosacl_srcs})

target_link_libraries(fwb_nxosacl common fwbcisco compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util)

target_compile_options(fwb_nxosacl PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_nxosacl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_nxosacl Widgets)

