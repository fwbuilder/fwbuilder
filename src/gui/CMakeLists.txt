project(fwbuilder-gui)

set(fwbuilder-gui_srcs
    main.cpp)

add_executable(fwbuilder-gui ${fwbuilder-gui_srcs})

target_link_libraries(fwbuilder-gui gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder c xml2 m dl xslt z util netsnmp crypto pthread) 

target_compile_options(fwbuilder-gui PRIVATE ${CXX_DEFAULT_FLAGS})

set_target_properties(fwbuilder-gui PROPERTIES OUTPUT_NAME "fwbuilder")

install(TARGETS fwbuilder-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt5_use_modules(fwbuilder-gui Widgets Network PrintSupport)

