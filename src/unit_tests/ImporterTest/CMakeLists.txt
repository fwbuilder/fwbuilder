set (CMAKE_AUTOMOC ON)

add_executable(ImporterTest ImporterTest.cpp main_ImporterTest.cpp)
add_test(NAME ImporterTest COMMAND ImporterTest)

IF (UseQt6)
    target_link_libraries(ImporterTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(ImporterTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(ImporterTest PRIVATE netsnmp)
ENDIF()

file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
