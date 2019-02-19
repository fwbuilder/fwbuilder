project(iosacl)

set(iosacl_srcs
    iosacl.cpp
    )

add_executable(fwb_iosacl ${iosacl_srcs})

target_link_libraries(fwb_iosacl common fwbcisco compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util)

target_compile_options(fwb_iosacl PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_iosacl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_iosacl Widgets)

