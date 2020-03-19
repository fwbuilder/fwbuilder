SET(CMAKE_AUTOMOC ON)

add_executable(FirewallDialogTest FirewallDialogTest.cpp main_FirewallDialogTest.cpp)
add_test(NAME FirewallDialogTest COMMAND FirewallDialogTest)

target_link_libraries(FirewallDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(FirewallDialogTest PRIVATE netsnmp)
ENDIF()
