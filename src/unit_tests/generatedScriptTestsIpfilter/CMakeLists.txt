SET(CMAKE_AUTOMOC ON)

add_executable(generatedScriptTestsIpfilter generatedScriptTestsIpfilter.cpp main_generatedScriptTestsIpfilter.cpp)
add_test(NAME generatedScriptTestsIpfilter COMMAND generatedScriptTestsIpfilter)

target_link_libraries(generatedScriptTestsIpfilter PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

file(COPY test1.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
