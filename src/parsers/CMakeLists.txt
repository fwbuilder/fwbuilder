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

IF (UseQt6)
    target_link_libraries(fwbparser import Qt6::Core Qt6::Widgets)
ELSE()
    target_link_libraries(fwbparser import Qt5::Core Qt5::Widgets)
ENDIF()

target_compile_options(fwbparser PRIVATE ${CXX_DEFAULT_FLAGS})

