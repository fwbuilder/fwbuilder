SET(CMAKE_AUTOMOC ON)

add_executable(AddressRangeDialogTest AddressRangeDialogTest.cpp main_AddressRangeDialogTest.cpp)
add_test(NAME AddressRangeDialogTest COMMAND AddressRangeDialogTest)

IF (UseQt6)
    target_link_libraries(AddressRangeDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(AddressRangeDialogTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(AddressRangeDialogTest PRIVATE netsnmp)
ENDIF()
