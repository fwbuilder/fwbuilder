SET(CMAKE_AUTOMOC ON)

add_executable(FWObjectTest FWObjectTest.cpp main.cpp)
add_test(NAME FWObjectTest COMMAND FWObjectTest)

IF (UseQt6)
    target_link_libraries(FWObjectTest PRIVATE fwbuilder xml2 xslt z Qt6::Test)
ELSE()
    target_link_libraries(FWObjectTest PRIVATE fwbuilder xml2 xslt z Qt5::Test)
ENDIF()
