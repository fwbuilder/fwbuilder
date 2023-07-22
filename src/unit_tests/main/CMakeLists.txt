add_library(test_main STATIC main.cpp)

IF (UseQt6)
    target_link_libraries(test_main gui Qt6::Test)
ELSE()
    target_link_libraries(test_main gui Qt5::Test)
ENDIF()

