SET(CMAKE_AUTOMOC ON)

add_executable(GroupObjectDialogTest GroupObjectDialogTest.cpp main_GroupObjectDialogTest.cpp)
add_test(NAME GroupObjectDialogTest COMMAND GroupObjectDialogTest)

IF (UseQt6)
    target_link_libraries(GroupObjectDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(GroupObjectDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(GroupObjectDialogTest PRIVATE netsnmp)
ENDIF()
