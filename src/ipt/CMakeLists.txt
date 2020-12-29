set(ipt_srcs
    ipt.cpp)

add_executable(fwb_ipt ${ipt_srcs})

target_link_libraries(fwb_ipt common iptlib compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z Qt5::Core Qt5::Widgets)

IF (WIN32)
  target_link_libraries(fwb_ipt lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwb_ipt c ${CMAKE_DL_LIBS} util)
ENDIF()

target_compile_options(fwb_ipt PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(ipt.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwb_ipt RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

