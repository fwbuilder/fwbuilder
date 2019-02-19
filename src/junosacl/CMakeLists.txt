project(junosacl)

set(junosacl_srcs
    junosacl.cpp
    )

add_executable(fwb_junosacl ${junosacl_srcs})

target_link_libraries(fwb_junosacl common fwbcisco fwbjuniper compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util)

target_compile_options(fwb_junosacl PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_junosacl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_junosacl Widgets)

