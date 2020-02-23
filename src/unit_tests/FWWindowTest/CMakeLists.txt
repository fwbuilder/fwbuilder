SET(CMAKE_AUTOMOC ON)

add_executable(FWWindowTest FWWindowTest.cpp main_FWWindowTest.cpp)
add_test(NAME FWWindowTest COMMAND FWWindowTest)

target_link_libraries(FWWindowTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)
#target_link_libraries(fwb_ipt common iptlib compilerdriver fwcompiler fwbuilder xml2 pthread m xslt z)
#target_link_libraries(fwbuilder-gui gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 m xslt z OpenSSL::Crypto pthread)
