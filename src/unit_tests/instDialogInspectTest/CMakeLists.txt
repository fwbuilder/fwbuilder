SET(CMAKE_AUTOMOC ON)

add_executable(instDialogInspectTest instDialogInspectTest.cpp main_instDialogInspectTest.cpp)
add_test(NAME instDialogInspectTest COMMAND instDialogInspectTest)

IF (UseQt6)
    target_link_libraries(instDialogInspectTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(instDialogInspectTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(instDialogInspectTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
