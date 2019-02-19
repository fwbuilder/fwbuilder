project(pix)

set(pix_srcs
    pix.cpp)

add_executable(fwb_pix ${pix_srcs})

target_link_libraries(fwb_pix common fwbcisco compilerdriver fwcompiler fwbuilder pthread c xml2 m dl xslt z util) 

target_compile_options(fwb_pix PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_pix RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwb_pix Widgets)

