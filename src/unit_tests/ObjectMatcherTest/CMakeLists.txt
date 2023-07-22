set (CMAKE_AUTOMOC ON)

add_executable(ObjectMatcherTest ObjectMatcherTest.cpp main.cpp)
add_test(NAME ObjectMatcherTest COMMAND ObjectMatcherTest)

IF (UseQt6)
    target_link_libraries(ObjectMatcherTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(ObjectMatcherTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(ObjectMatcherTest PRIVATE netsnmp)
ENDIF()

file(COPY test_data.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
