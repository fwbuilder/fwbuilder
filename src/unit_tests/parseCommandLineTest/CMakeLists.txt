add_executable(parseCommandLineTest parseCommandLineTest.cpp main_parseCommandLineTest.cpp)
add_test(NAME parseCommandLineTest COMMAND parseCommandLineTest)

target_link_libraries(parseCommandLineTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)
