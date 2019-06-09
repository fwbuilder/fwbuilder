project(fwbedit)

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

target_link_libraries(fwbedit gui import fwbparser antlr common iptlib fwbpf fwbcisco compilerdriver fwcompiler fwbuilder xml2 m xslt z crypto pthread)

IF (WIN32)
  target_link_libraries(fwbedit lzma iconv)
ENDIF()

IF (UNIX)
  target_link_libraries(fwbedit c dl util)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(fwbedit netsnmp)
ENDIF()

target_compile_options(fwbedit PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwbedit RUNTIME DESTINATION ${FWB_INSTALL_BINDIR})

qt5_use_modules(fwbedit Widgets Network PrintSupport)

