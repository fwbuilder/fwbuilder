set (CMAKE_AUTOMOC ON)

add_executable(parseCommandLineTest parseCommandLineTest.cpp main_parseCommandLineTest.cpp)
add_test(NAME parseCommandLineTest COMMAND parseCommandLineTest)

target_link_libraries(parseCommandLineTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(parseCommandLineTest PRIVATE netsnmp)
ENDIF()
