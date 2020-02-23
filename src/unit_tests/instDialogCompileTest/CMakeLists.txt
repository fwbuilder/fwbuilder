SET(CMAKE_AUTOMOC ON)

add_executable(instDialogCompileTest instDialogCompileTest.cpp main_instDialogCompileTest.cpp)
add_test(NAME instDialogCompileTest COMMAND instDialogCompileTest)

target_link_libraries(instDialogCompileTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
