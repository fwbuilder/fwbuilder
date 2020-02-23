add_executable(generatedScriptTestsPF generatedScriptTestsPF.cpp main_generatedScriptTestsPF.cpp)
add_test(NAME generatedScriptTestsPF COMMAND generatedScriptTestsPF)

target_link_libraries(generatedScriptTestsPF PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)

file(COPY test1.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
