SET(CMAKE_AUTOMOC ON)

add_executable(TCPServiceDialogTest TCPServiceDialogTest.cpp main_TCPServiceDialogTest.cpp)
add_test(NAME TCPServiceDialogTest COMMAND TCPServiceDialogTest)

IF (UseQt6)
    target_link_libraries(TCPServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(TCPServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(TCPServiceDialogTest PRIVATE netsnmp)
ENDIF()
