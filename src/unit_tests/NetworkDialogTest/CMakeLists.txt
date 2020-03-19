SET(CMAKE_AUTOMOC ON)

add_executable(NetworkDialogTest NetworkDialogTest.cpp main_NetworkDialogTest.cpp)
add_test(NAME NetworkDialogTest COMMAND NetworkDialogTest)

target_link_libraries(NetworkDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
