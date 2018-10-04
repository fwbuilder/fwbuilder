import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "pflib"
    targetName: "fwbpf"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libxml2" }
    Depends { name: "libfwbuilder" }
    Depends { name: "libfwcompiler" }
    Depends { name: "compiler_lib" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "TableFactory.cpp",
        "Preprocessor_pf.cpp",
        "NATCompiler_ipf.cpp",
        "NATCompiler_ipfw.cpp",
        "NATCompiler_ipf_writers.cpp",
        "NATCompiler_ipfw_writers.cpp",
        "NATCompiler_pf.cpp",
        "NATCompiler_pf_negation.cpp",
        "NATCompiler_pf_writers.cpp",
        "OSConfigurator_bsd.cpp",
        "OSConfigurator_bsd_interfaces.cpp",
        "OSConfigurator_freebsd.cpp",
        "OSConfigurator_macosx.cpp",
        "OSConfigurator_openbsd.cpp",
        "OSConfigurator_solaris.cpp",
        "OSData_pf.cpp",
        "PolicyCompiler_ipf.cpp",
        "PolicyCompiler_ipf_optimizer.cpp",
        "PolicyCompiler_ipfw.cpp",
        "PolicyCompiler_ipf_writers.cpp",
        "PolicyCompiler_ipfw_writers.cpp",
        "PolicyCompiler_pf.cpp",
        "PolicyCompiler_pf_writers.cpp",
        "CompilerDriver_pf.cpp",
        "CompilerDriver_pf_run.cpp",
        "CompilerDriver_ipf.cpp",
        "CompilerDriver_ipf_run.cpp",
        "CompilerDriver_ipfw.cpp",
        "CompilerDriver_ipfw_run.cpp",
        "RoutingCompiler_openbsd.cpp",
        "RoutingCompiler_openbsd_writers.cpp",
        "RoutingCompiler_freebsd.cpp",
        "RoutingCompiler_freebsd_writers.cpp",
        "AutomaticRules_pf.cpp"
    ]
}
