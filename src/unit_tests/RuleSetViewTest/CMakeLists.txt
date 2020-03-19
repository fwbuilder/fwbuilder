SET(CMAKE_AUTOMOC ON)

add_executable(RuleSetViewTest RuleSetViewTest.cpp main_RuleSetViewTest.cpp)
add_test(NAME RuleSetViewTest COMMAND RuleSetViewTest)

target_link_libraries(RuleSetViewTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(RuleSetViewTest PRIVATE netsnmp)
ENDIF()
