SET(CMAKE_AUTOMOC ON)

add_executable(UserServiceDialogTest UserServiceDialogTest.cpp main_UserServiceDialogTest.cpp)
add_test(NAME UserServiceDialogTest COMMAND UserServiceDialogTest)

IF (UseQt6)
    target_link_libraries(UserServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(UserServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(UserServiceDialogTest PRIVATE PRIVATE netsnmp)
ENDIF()
