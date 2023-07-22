SET(CMAKE_AUTOMOC ON)

add_executable(instDialogInstallTest instDialogInstallTest.cpp main_instDialogInstallTest.cpp)
add_test(NAME instDialogInstallTest COMMAND instDialogInstallTest)

IF (UseQt6)
    target_link_libraries(instDialogInstallTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(instDialogInstallTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(instDialogInstallTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
