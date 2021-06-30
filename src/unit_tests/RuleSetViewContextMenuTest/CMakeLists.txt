SET(CMAKE_AUTOMOC ON)

add_executable(RuleSetViewContextMenuTest RuleSetViewContextMenuTest.cpp main_RuleSetViewContextMenuTest.cpp)
add_test(NAME RuleSetViewContextMenuTest COMMAND RuleSetViewContextMenuTest)

target_link_libraries(RuleSetViewContextMenuTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(RuleSetViewContextMenuTest PRIVATE netsnmp)
ENDIF()
