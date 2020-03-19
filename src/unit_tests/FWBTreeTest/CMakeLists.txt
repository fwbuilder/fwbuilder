SET(CMAKE_AUTOMOC ON)

add_executable(FWBTreeTest FWBTreeTest.cpp main_FWBTreeTest.cpp)
add_test(NAME FWBTreeTest COMMAND FWBTreeTest)

target_link_libraries(FWBTreeTest PRIVATE gui fwbuilder xml2 xslt z Qt5::Test)
