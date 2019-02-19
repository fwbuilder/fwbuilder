project(parsers)

set(parsers_srcs
    IOSCfgLexer.cpp
    IOSCfgParser.cpp
    IPTCfgLexer.cpp
    IPTCfgParser.cpp
    PFCfgLexer.cpp
    PFCfgParser.cpp
    PIXCfgLexer.cpp
    PIXCfgParser.cpp)

add_library(fwbparser STATIC ${parsers_srcs})

target_include_directories(fwbparser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(fwbparser import)

target_compile_options(fwbparser PRIVATE ${CXX_DEFAULT_FLAGS})

qt5_use_modules(fwbparser Widgets)

