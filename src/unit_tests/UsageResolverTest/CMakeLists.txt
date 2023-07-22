set (CMAKE_AUTOMOC ON)

add_executable(UsageResolverTest UsageResolverTest.cpp main_UsageResolverTest.cpp)
add_test(NAME UsageResolverTest COMMAND UsageResolverTest)

IF (UseQt6)
    target_link_libraries(UsageResolverTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt6::Test)
ELSE()
    target_link_libraries(UsageResolverTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
ENDIF()

IF (NETSNMP_FOUND)
  target_link_libraries(UsageResolverTest PRIVATE netsnmp)
ENDIF()
