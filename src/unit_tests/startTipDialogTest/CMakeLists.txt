SET(CMAKE_AUTOMOC ON)

add_executable(startTipDialogTest startTipDialogTest.cpp main_startTipDialogTest.cpp)
add_test(NAME startTipDialogTest COMMAND startTipDialogTest)

target_link_libraries(startTipDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
