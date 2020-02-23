add_executable(RCSTest RCSTest.cpp main_RCS.cpp)
add_test(NAME RCSTest COMMAND RCSTest)

target_link_libraries(RCSTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread cppunit)

file(COPY rlog_unit_test.log "zu.fwb,v" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
