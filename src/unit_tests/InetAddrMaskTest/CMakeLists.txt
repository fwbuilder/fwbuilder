set (CMAKE_AUTOMOC ON)

add_executable(InetAddrMaskTest InetAddrMaskTest.cpp main.cpp)
add_test(NAME InetAddrMaskTest COMMAND InetAddrMaskTest)

target_link_libraries(InetAddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt5::Test)
