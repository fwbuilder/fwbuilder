SET(CMAKE_AUTOMOC ON)

add_executable(generatedScriptTestsLinux generatedScriptTestsLinux.cpp main_generatedScriptTestsLinux.cpp)
add_test(NAME generatedScriptTestsLinux COMMAND generatedScriptTestsLinux)

target_link_libraries(generatedScriptTestsLinux PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(generatedScriptTestsLinux PRIVATE netsnmp)
ENDIF()

file(COPY test1.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
