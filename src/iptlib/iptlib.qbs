import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "iptlib"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "compiler_lib" }
    Depends { name: "libfwcompiler" }
    Depends { name: "libxml2" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "CompilerDriver_ipt.cpp",
        "CompilerDriver_ipt_run.cpp",
        "CompilerDriver_ipt_policy.cpp",
        "CompilerDriver_ipt_nat.cpp",
        "MangleTableCompiler_ipt.cpp",
        "NATCompiler_PrintRule.cpp",
        "NATCompiler_PrintRuleIptRst.cpp",
        "NATCompiler_PrintRuleIptRstEcho.cpp",
        "NATCompiler_ipt.cpp",
        "OSConfigurator_ipcop.cpp",
        "OSConfigurator_linux24.cpp",
        "OSConfigurator_linux24_interfaces.cpp",
        "OSConfigurator_secuwall.cpp",
        "OSData_ipt.cpp",
        "PolicyCompiler_PrintRule.cpp",
        "PolicyCompiler_PrintRuleIptRst.cpp",
        "PolicyCompiler_PrintRuleIptRstEcho.cpp",
        "PolicyCompiler_ipt.cpp",
        "PolicyCompiler_ipt_optimizer.cpp",
        "PolicyCompiler_secuwall.cpp",
        "RoutingCompiler_ipt.cpp",
        "RoutingCompiler_ipt_writers.cpp",
        "Preprocessor_ipt.cpp",
        "combinedAddress.cpp",
        "AutomaticRules_ipt.cpp",
        "ipt_utils.cpp"
    ]
}
