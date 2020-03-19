SET(CMAKE_AUTOMOC ON)

add_executable(AddressRangeDialogTest AddressRangeDialogTest.cpp main_AddressRangeDialogTest.cpp)
add_test(NAME AddressRangeDialogTest COMMAND AddressRangeDialogTest)

target_link_libraries(AddressRangeDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
