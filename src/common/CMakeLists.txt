project(common)

set(common_srcs
    init.cpp
    init2.cpp)

add_library(common STATIC ${common_srcs})

target_include_directories(common PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(common fwbuilder)

target_compile_options(common PRIVATE ${CXX_DEFAULT_FLAGS})

qt5_use_modules(common Core)

