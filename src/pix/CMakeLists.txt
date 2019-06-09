project(pix)

set(pix_srcs
    pix.cpp)

add_executable(fwb_pix ${pix_srcs})

target_link_libraries(fwb_pix common fwbcisco compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)

IF (WIN32)
  target_link_libraries(fwb_pix lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_pix c dl util)
ENDIF()

target_compile_options(fwb_pix PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwb_pix RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwb_pix Widgets)

