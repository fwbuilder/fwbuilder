SET(CMAKE_AUTOMOC ON)

add_executable(generatedScriptTestsSecuwall generatedScriptTestsSecuwall.cpp main_generatedScriptTestsSecuwall.cpp)
add_test(NAME generatedScriptTestsSecuwall COMMAND generatedScriptTestsSecuwall)

target_link_libraries(generatedScriptTestsSecuwall PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(generatedScriptTestsSecuwall PRIVATE netsnmp)
ENDIF()

file(COPY test1.fwb ref.secuwall-1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
