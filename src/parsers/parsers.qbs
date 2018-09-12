import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "parsers"
    targetName: "fwbparser"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "antlr" }
    Depends { name: "libxml2" }

    cpp.includePaths: base.concat(['../import'])

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "IOSCfgLexer.cpp",
        "IOSCfgParser.cpp",
        "IPTCfgLexer.cpp",
        "IPTCfgParser.cpp",
        "PIXCfgLexer.cpp",
        "PIXCfgParser.cpp",
        "PFCfgLexer.cpp",
        "PFCfgParser.cpp",
    ]
}
