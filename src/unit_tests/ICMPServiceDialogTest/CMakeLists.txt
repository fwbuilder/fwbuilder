SET(CMAKE_AUTOMOC ON)

add_executable(ICMPServiceDialogTest ICMPServiceDialogTest.cpp main_ICMPServiceDialogTest.cpp)
add_test(NAME ICMPServiceDialogTest COMMAND ICMPServiceDialogTest)

target_link_libraries(ICMPServiceDialogTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
