SET(CMAKE_AUTOMOC ON)

add_executable(instDialogClusterTest instDialogClusterTest.cpp main_instDialogClusterTest.cpp)
add_test(NAME instDialogClusterTest COMMAND instDialogClusterTest)

target_link_libraries(instDialogClusterTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
