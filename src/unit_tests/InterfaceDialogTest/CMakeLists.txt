SET(CMAKE_AUTOMOC ON)

add_executable(InterfaceDialogTest InterfaceDialogTest.cpp main_InterfaceDialogTest.cpp)
add_test(NAME InterfaceDialogTest COMMAND InterfaceDialogTest)

target_link_libraries(InterfaceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
