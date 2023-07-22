set (CMAKE_AUTOMOC ON)

add_executable(InetAddrMaskTest InetAddrMaskTest.cpp main.cpp)
add_test(NAME InetAddrMaskTest COMMAND InetAddrMaskTest)

IF (UseQt6)
    target_link_libraries(InetAddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt6::Test)
ELSE()
    target_link_libraries(InetAddrMaskTest PRIVATE fwbuilder xml2 xslt z Qt5::Test)
ENDIF()
