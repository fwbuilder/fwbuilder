import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "libfwcompiler"
    targetName: "fwcompiler"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: 'libxml2' }

    cpp.includePaths: base.concat(['..'])

    files: [
        "BaseCompiler.cpp",
        "Compiler.cpp",
        "Compiler_helpers.cpp",
        "Compiler_ops.cpp",
        "Compiler_object_match.cpp",
        "Preprocessor.cpp",
        "NATCompiler.cpp",
        "OSConfigurator.cpp",
        "PolicyCompiler.cpp",
        "ServiceRuleProcessors.cpp",
        "RoutingCompiler.cpp",
        "GroupRegistry.cpp"
    ]
}
