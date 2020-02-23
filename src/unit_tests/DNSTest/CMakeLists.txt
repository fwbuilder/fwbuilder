add_executable(DNSTest DNSTest.cpp main.cpp)
add_test(NAME DNSTest COMMAND DNSTest)

target_link_libraries(DNSTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)
