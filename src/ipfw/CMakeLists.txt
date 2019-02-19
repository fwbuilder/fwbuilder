project(ipfw)

set(ipfw_srcs
    ipfw.cpp)

add_executable(fwb_ipfw ${ipfw_srcs})

target_link_libraries(fwb_ipfw common fwbpf compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util) 

target_compile_options(fwb_ipfw PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_ipfw RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_ipfw Widgets)

