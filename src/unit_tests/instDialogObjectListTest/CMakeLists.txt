SET(CMAKE_AUTOMOC ON)

add_executable(instDialogObjectListTest instDialogObjectListTest.cpp main_instDialogObjectListTest.cpp)
add_test(NAME instDialogObjectListTest COMMAND instDialogObjectListTest)

target_link_libraries(instDialogObjectListTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
