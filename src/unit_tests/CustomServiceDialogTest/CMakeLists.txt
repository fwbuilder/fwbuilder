SET(CMAKE_AUTOMOC ON)

add_executable(CustomServiceDialogTest CustomServiceDialogTest.cpp main_CustomServiceDialogTest.cpp)
add_test(NAME CustomServiceDialogTest COMMAND CustomServiceDialogTest)

IF (UseQt6)
    target_link_libraries(CustomServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(CustomServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(CustomServiceDialogTest PRIVATE netsnmp)
ENDIF()
