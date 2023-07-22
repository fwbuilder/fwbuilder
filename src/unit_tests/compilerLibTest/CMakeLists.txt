SET(CMAKE_AUTOMOC ON)

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")

add_executable(compilerLibTest interfacePropertiesTest.cpp tests_main.cpp)
add_test(NAME compilerLibTest COMMAND compilerLibTest)

IF (UseQt6)
    target_link_libraries(compilerLibTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(compilerLibTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(compilerLibTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
