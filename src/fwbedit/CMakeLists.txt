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

target_link_libraries(fwbedit gui import fwbparser antlr common iptlib fwbpf fwbcisco compilerdriver fwcompiler fwbuilder c xml2 m dl xslt z util netsnmp crypto pthread) 

target_compile_options(fwbedit PRIVATE ${CXX_DEFAULT_FLAGS})

install(TARGETS fwbedit RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwbedit Widgets Network PrintSupport)

