import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "compiler_lib"
    targetName: "compilerdriver"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "libxml2" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "CompilerDriver.cpp",
        "CompilerDriver_files.cpp",
        "CompilerDriver_compile.cpp",
        "CompilerDriver_generators.cpp",
        "Configlet.cpp",
        "interfaceProperties.cpp",
        "linux24Interfaces.cpp",
        "openbsdInterfaces.cpp",
        "freebsdInterfaces.cpp",
        "nxosInterfaces.cpp",
        "iosInterfaces.cpp",
        "junosInterfaces.cpp",
        "procurveInterfaces.cpp",
        "pixInterfaces.cpp",
        "interfacePropertiesObjectFactory.cpp",
        "AutomaticRules.cpp"
    ]
}
