project(pflib)

set(pflib_srcs
    AutomaticRules_pf.cpp
    CompilerDriver_ipf.cpp
    CompilerDriver_ipf_run.cpp
    CompilerDriver_ipfw.cpp
    CompilerDriver_ipfw_run.cpp
    CompilerDriver_pf.cpp
    CompilerDriver_pf_run.cpp
    NATCompiler_ipf.cpp
    NATCompiler_ipf_writers.cpp
    NATCompiler_ipfw.cpp
    NATCompiler_ipfw_writers.cpp
    NATCompiler_pf.cpp
    NATCompiler_pf_negation.cpp
    NATCompiler_pf_writers.cpp
    OSConfigurator_bsd.cpp
    OSConfigurator_bsd_interfaces.cpp
    OSConfigurator_freebsd.cpp
    OSConfigurator_macosx.cpp
    OSConfigurator_openbsd.cpp
    OSConfigurator_solaris.cpp
    OSData_pf.cpp
    PolicyCompiler_ipf.cpp
    PolicyCompiler_ipf_optimizer.cpp
    PolicyCompiler_ipf_writers.cpp
    PolicyCompiler_ipfw.cpp
    PolicyCompiler_ipfw_writers.cpp
    PolicyCompiler_pf.cpp
    PolicyCompiler_pf_writers.cpp
    Preprocessor_pf.cpp
    RoutingCompiler_freebsd.cpp
    RoutingCompiler_freebsd_writers.cpp
    RoutingCompiler_openbsd.cpp
    RoutingCompiler_openbsd_writers.cpp
    TableFactory.cpp)

add_library(fwbpf STATIC ${pflib_srcs})

target_include_directories(fwbpf PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(fwbpf compilerdriver)

target_compile_options(fwbpf PRIVATE ${CXX_DEFAULT_FLAGS})

qt5_use_modules(fwbpf Core)

