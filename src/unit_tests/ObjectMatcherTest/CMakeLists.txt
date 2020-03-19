set (CMAKE_AUTOMOC ON)

add_executable(ObjectMatcherTest ObjectMatcherTest.cpp main.cpp)
add_test(NAME ObjectMatcherTest COMMAND ObjectMatcherTest)

target_link_libraries(ObjectMatcherTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(ObjectMatcherTest PRIVATE netsnmp)
ENDIF()

file(COPY test_data.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
