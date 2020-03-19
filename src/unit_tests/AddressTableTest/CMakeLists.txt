SET(CMAKE_AUTOMOC ON)

add_executable(AddressTableTest AddressTableTest.cpp main.cpp)
add_test(NAME AddressTableTest COMMAND AddressTableTest)

target_link_libraries(AddressTableTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

file(COPY addresstable-1.txt addresstable-2.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

