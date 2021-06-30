find_package(OpenSSL REQUIRED)

set(fwbedit_srcs
    fwbedit.cpp
    new_object.cpp
    repair_tree.cpp
    list_object.cpp
    merge.cpp
    import.cpp
    ../libgui/FWBTree.cpp
    ../libgui/platforms.cpp)

add_executable(fwbedit ${fwbedit_srcs})

target_link_libraries(fwbedit gui import fwbparser antlr common iptlib fwbpf fwbcisco compilerdriver fwcompiler fwbuilder xml2 m xslt z OpenSSL::Crypto pthread Qt5::Core Qt5::Widgets Qt5::PrintSupport)

IF (WIN32)
  target_link_libraries(fwbedit lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwbedit c ${CMAKE_DL_LIBS} util)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(fwbedit netsnmp)
ENDIF()

target_compile_options(fwbedit PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(fwbedit.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

install(TARGETS fwbedit RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

