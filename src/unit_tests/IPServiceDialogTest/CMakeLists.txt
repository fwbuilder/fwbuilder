SET(CMAKE_AUTOMOC ON)

add_executable(IPServiceDialogTest IPServiceDialogTest.cpp main_IPServiceDialogTest.cpp)
add_test(NAME IPServiceDialogTest COMMAND IPServiceDialogTest)

target_link_libraries(IPServiceDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(IPServiceDialogTest PRIVATE netsnmp)
ENDIF()
