SET(CMAKE_AUTOMOC ON)

add_executable(instDialogObjectListTest instDialogObjectListTest.cpp main_instDialogObjectListTest.cpp)
add_test(NAME instDialogObjectListTest COMMAND instDialogObjectListTest)

IF (UseQt6)
    target_link_libraries(instDialogObjectListTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(instDialogObjectListTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(instDialogObjectListTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
