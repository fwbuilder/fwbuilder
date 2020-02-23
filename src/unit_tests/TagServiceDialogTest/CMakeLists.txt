SET(CMAKE_AUTOMOC ON)

add_executable(TagServiceDialogTest TagServiceDialogTest.cpp main_TagServiceDialogTest.cpp)
add_test(NAME TagServiceDialogTest COMMAND TagServiceDialogTest)

target_link_libraries(TagServiceDialogTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
