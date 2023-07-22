set (CMAKE_AUTOMOC ON)

add_executable(Inet6AddrMaskTest Inet6AddrMaskTest.cpp main.cpp)
add_test(NAME Inet6AddrMaskTest COMMAND Inet6AddrMaskTest)

IF (UseQt6)
    target_link_libraries(Inet6AddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt6::Test)
ELSE()
    target_link_libraries(Inet6AddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt5::Test)
ENDIF()
