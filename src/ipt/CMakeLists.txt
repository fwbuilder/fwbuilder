project(ipt)

set(ipt_srcs
    ipt.cpp)

add_executable(fwb_ipt ${ipt_srcs})

target_link_libraries(fwb_ipt common iptlib compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util) 

target_compile_options(fwb_ipt PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_ipt RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_ipt Widgets)

