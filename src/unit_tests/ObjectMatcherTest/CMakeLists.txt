add_executable(ObjectMatcherTest ObjectMatcherTest.cpp main.cpp)
add_test(NAME ObjectMatcherTest COMMAND ObjectMatcherTest)

target_link_libraries(ObjectMatcherTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)

file(COPY test_data.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
