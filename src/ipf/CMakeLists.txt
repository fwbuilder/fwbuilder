project(ipf)

set(ipf_srcs
    ipf.cpp)

add_executable(fwb_ipf ${ipf_srcs})

target_link_libraries(fwb_ipf common fwbpf compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util)

target_compile_options(fwb_ipf PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_ipf RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_ipf Widgets)

