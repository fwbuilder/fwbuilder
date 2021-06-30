SET(CMAKE_AUTOMOC ON)

add_executable(genericDialogTest genericDialogTest.cpp main_genericDialogTest.cpp)
add_test(NAME genericDialogTest COMMAND genericDialogTest)

target_link_libraries(genericDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(genericDialogTest PRIVATE netsnmp)
ENDIF()
