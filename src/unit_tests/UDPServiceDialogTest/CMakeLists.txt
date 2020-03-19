SET(CMAKE_AUTOMOC ON)

add_executable(UDPServiceDialogTest UDPServiceDialogTest.cpp main_UDPServiceDialogTest.cpp)
add_test(NAME UDPServiceDialogTest COMMAND UDPServiceDialogTest)

target_link_libraries(UDPServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(UDPServiceDialogTest PRIVATE netsnmp)
ENDIF()
