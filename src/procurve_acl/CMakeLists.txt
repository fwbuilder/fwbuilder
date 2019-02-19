project(procurve_acl)

set(procurve_acl_srcs
    procurve_acl.cpp
    )

add_executable(fwb_procurve_acl ${procurve_acl_srcs})

target_link_libraries(fwb_procurve_acl common fwbcisco compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util) 

target_compile_options(fwb_procurve_acl PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_procurve_acl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_procurve_acl Widgets)

