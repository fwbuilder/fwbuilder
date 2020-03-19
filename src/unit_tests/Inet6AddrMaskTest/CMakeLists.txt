set (CMAKE_AUTOMOC ON)

add_executable(Inet6AddrMaskTest Inet6AddrMaskTest.cpp main.cpp)
add_test(NAME Inet6AddrMaskTest COMMAND Inet6AddrMaskTest)

target_link_libraries(Inet6AddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt5::Test)
