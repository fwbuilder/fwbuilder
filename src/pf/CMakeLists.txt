project(pf)

set(pf_srcs
    pf.cpp)

add_executable(fwb_pf ${pf_srcs})

target_link_libraries(fwb_pf common fwbpf compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util) 

target_compile_options(fwb_pf PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_pf RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_pf Widgets)

