SET(CMAKE_AUTOMOC ON)

add_executable(DNSTest DNSTest.cpp main.cpp)
add_test(NAME DNSTest COMMAND DNSTest)

target_link_libraries(DNSTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(DNSTest PRIVATE netsnmp)
ENDIF()
