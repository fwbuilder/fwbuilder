set(common_srcs
    init.cpp
    init2.cpp)

add_library(common STATIC ${common_srcs})

target_include_directories(common PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(common fwbuilder)

target_compile_options(common PRIVATE ${CXX_DEFAULT_FLAGS})

set_source_files_properties(init.cpp PROPERTIES COMPILE_DEFINITIONS PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

qt5_use_modules(common Core)

