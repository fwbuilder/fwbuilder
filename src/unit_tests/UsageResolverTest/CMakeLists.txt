add_executable(UsageResolverTest UsageResolverTest.cpp main_UsageResolverTest.cpp)
add_test(NAME UsageResolverTest COMMAND UsageResolverTest)

target_link_libraries(UsageResolverTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)
