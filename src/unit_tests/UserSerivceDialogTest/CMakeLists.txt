SET(CMAKE_AUTOMOC ON)

add_executable(UserServiceDialogTest UserServiceDialogTest.cpp main_UserServiceDialogTest.cpp)
add_test(NAME UserServiceDialogTest COMMAND UserServiceDialogTest)

target_link_libraries(UserServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
