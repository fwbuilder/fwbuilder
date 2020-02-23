add_library(test_main STATIC main.cpp)

target_link_libraries(test_main gui)

qt5_use_modules(test_main Test)

