set(compiler_lib_srcs
    CompilerDriver.cpp
    CompilerDriver_files.cpp
    CompilerDriver_compile.cpp
    CompilerDriver_generators.cpp
    Configlet.cpp
    interfaceProperties.cpp
    linux24Interfaces.cpp
    openbsdInterfaces.cpp
    freebsdInterfaces.cpp
    nxosInterfaces.cpp
    iosInterfaces.cpp
    junosInterfaces.cpp
    procurveInterfaces.cpp
    pixInterfaces.cpp
    interfacePropertiesObjectFactory.cpp
    AutomaticRules.cpp)

add_library(compilerdriver STATIC ${compiler_lib_srcs})

target_include_directories(compilerdriver PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

IF (UseQt6)
    target_link_libraries(compilerdriver fwbuilder Qt6::Core)
ELSE()
    target_link_libraries(compilerdriver fwbuilder Qt5::Core)
ENDIF()

target_compile_options(compilerdriver PRIVATE ${CXX_DEFAULT_FLAGS})

