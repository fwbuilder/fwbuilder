project(libfwcompiler)

set(libfwcompiler_srcs
    BaseCompiler.cpp
    Compiler.cpp
    Compiler_helpers.cpp
    Compiler_ops.cpp
    Compiler_object_match.cpp
    Preprocessor.cpp
    NATCompiler.cpp
    OSConfigurator.cpp
    PolicyCompiler.cpp
    ServiceRuleProcessors.cpp
    RoutingCompiler.cpp
    GroupRegistry.cpp)

add_library(fwcompiler STATIC ${libfwcompiler_srcs})

target_include_directories(fwcompiler PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..
    ${LIBXML2_INCLUDE_DIR})

target_compile_options(fwcompiler PRIVATE ${CXX_DEFAULT_FLAGS})

