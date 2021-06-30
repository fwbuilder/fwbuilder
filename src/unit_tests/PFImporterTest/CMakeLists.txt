set (CMAKE_AUTOMOC ON)

add_executable(PFImporterTest PFImporterTest.cpp main_PFImporterTest.cpp)
add_test(NAME PFImporterTest COMMAND PFImporterTest)

target_link_libraries(PFImporterTest PRIVATE test_main gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

IF (NETSNMP_FOUND)
  target_link_libraries(PFImporterTest PRIVATE netsnmp)
ENDIF()

file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
