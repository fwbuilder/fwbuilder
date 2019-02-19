project(iptlib)

set(iptlib_srcs
    CompilerDriver_ipt_policy.cpp
    OSData_ipt.cpp
    AutomaticRules_ipt.cpp
    PolicyCompiler_ipt_optimizer.cpp
    PolicyCompiler_PrintRuleIptRst.cpp
    PolicyCompiler_secuwall.cpp
    CompilerDriver_ipt.cpp
    RoutingCompiler_ipt_writers.cpp
    Preprocessor_ipt.cpp
    combinedAddress.cpp
    CompilerDriver_ipt_nat.cpp
    RoutingCompiler_ipt.cpp
    NATCompiler_PrintRuleIptRstEcho.cpp
    PolicyCompiler_PrintRuleIptRstEcho.cpp
    OSConfigurator_secuwall.cpp
    NATCompiler_PrintRuleIptRst.cpp
    PolicyCompiler_ipt.cpp
    OSConfigurator_ipcop.cpp
    OSConfigurator_linux24_interfaces.cpp
    OSConfigurator_linux24.cpp
    ipt_utils.cpp
    PolicyCompiler_PrintRule.cpp
    NATCompiler_PrintRule.cpp
    MangleTableCompiler_ipt.cpp
    CompilerDriver_ipt_run.cpp
    NATCompiler_ipt.cpp)

add_library(iptlib STATIC ${iptlib_srcs})

target_include_directories(iptlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(iptlib compilerdriver)

target_compile_options(iptlib PRIVATE ${CXX_DEFAULT_FLAGS})

qt5_use_modules(iptlib Core)

