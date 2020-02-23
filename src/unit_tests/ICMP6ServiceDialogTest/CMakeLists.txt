SET(CMAKE_AUTOMOC ON)

add_executable(ICMP6ServiceDialogTest ICMP6ServiceDialogTest.cpp main_ICMP6ServiceDialogTest.cpp)
add_test(NAME ICMP6ServiceDialogTest COMMAND ICMP6ServiceDialogTest)

target_link_libraries(ICMP6ServiceDialogTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
