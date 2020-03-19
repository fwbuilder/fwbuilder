SET(CMAKE_AUTOMOC ON)

add_executable(genericDialogTest genericDialogTest.cpp main_genericDialogTest.cpp)
add_test(NAME genericDialogTest COMMAND genericDialogTest)

target_link_libraries(genericDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
