SET(CMAKE_AUTOMOC ON)

add_executable(instDialogCompileTest instDialogCompileTest.cpp main_instDialogCompileTest.cpp)
add_test(NAME instDialogCompileTest COMMAND instDialogCompileTest)

target_link_libraries(instDialogCompileTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(instDialogCompileTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
