SET(CMAKE_AUTOMOC ON)

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")

add_executable(compilerLibTest interfacePropertiesTest.cpp tests_main.cpp)
add_test(NAME compilerLibTest COMMAND compilerLibTest)

target_link_libraries(compilerLibTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
