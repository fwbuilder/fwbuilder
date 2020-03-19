set (CMAKE_AUTOMOC ON)

add_executable(UsageResolverTest UsageResolverTest.cpp main_UsageResolverTest.cpp)
add_test(NAME UsageResolverTest COMMAND UsageResolverTest)

target_link_libraries(UsageResolverTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder netsnmp xml2 xslt z pthread Qt5::Test)
