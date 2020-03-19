SET(CMAKE_AUTOMOC ON)

add_executable(newClusterDialogTest newClusterDialogTest.cpp main_newClusterDialogTest.cpp)
add_test(NAME newClusterDialogTest COMMAND newClusterDialogTest)

target_link_libraries(newClusterDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
