set (CMAKE_AUTOMOC ON)

add_executable(PIXImporterTest PIXImporterTest.cpp main_PIXImporterTest.cpp)
add_test(NAME PIXImporterTest COMMAND PIXImporterTest)

target_link_libraries(PIXImporterTest PRIVATE test_main netsnmp gui import fwbparser antlr common iptlib fwbpf fwbjuniper fwbcisco compilerdriver fwcompiler fwbuilder xml2 xslt z pthread Qt5::Test)

file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\")
