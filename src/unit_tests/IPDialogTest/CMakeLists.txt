SET(CMAKE_AUTOMOC ON)

add_executable(IPDialogTest IPDialogTest.cpp main_IPDialogTest.cpp)
add_test(NAME IPDialogTest COMMAND IPDialogTest)

target_link_libraries(IPDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(IPDialogTest PRIVATE netsnmp)
ENDIF()
