SET(CMAKE_AUTOMOC ON)

add_executable(instDialogInstallTest instDialogInstallTest.cpp main_instDialogInstallTest.cpp)
add_test(NAME instDialogInstallTest COMMAND instDialogInstallTest)

target_link_libraries(instDialogInstallTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(instDialogInstallTest PRIVATE netsnmp)
ENDIF()

file(COPY test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
