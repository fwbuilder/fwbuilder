SET(CMAKE_AUTOMOC ON)

add_executable(newClusterDialogTest newClusterDialogTest.cpp main_newClusterDialogTest.cpp)
add_test(NAME newClusterDialogTest COMMAND newClusterDialogTest)

IF (UseQt6)
    target_link_libraries(newClusterDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(newClusterDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(newClusterDialogTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
