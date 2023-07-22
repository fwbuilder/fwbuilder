SET(CMAKE_AUTOMOC ON)

add_executable(generatedScriptTestsSecuwall generatedScriptTestsSecuwall.cpp main_generatedScriptTestsSecuwall.cpp)
add_test(NAME generatedScriptTestsSecuwall COMMAND generatedScriptTestsSecuwall)

IF (UseQt6)
    target_link_libraries(generatedScriptTestsSecuwall PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(generatedScriptTestsSecuwall PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(generatedScriptTestsSecuwall PRIVATE netsnmp)
ENDIF()

file(COPY test1.fwb ref.secuwall-1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
