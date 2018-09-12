import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "libimport"
    targetName: "import"

    Depends { name: "cpp" }
    Depends { name: "Qt.gui" }
    Depends { name: "libfwbuilder" }
    Depends { name: "antlr" }
    Depends { name: "parsers" }
    Depends { name: "compiler_lib" }
    Depends { name: "libxml2" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "QStringListOperators.cpp",
        "PreImport.cpp",
        "objectMaker.cpp",
        "objectSignature.cpp",
        "addressObjectMaker.cpp",
        "serviceObjectMaker.cpp",
        "getProtoByName.cpp",
        "getServByName.cpp",
        "Importer.cpp",
        "IOSImporter.cpp",
        "IOSImporterRun.cpp",
        "IPTImporter.cpp",
        "IPTImporterRun.cpp",
        "PIXImporter.cpp",
        "PIXImporterNat.cpp",
        "PIXImporterRun.cpp",
        "PFImporter.cpp",
        "PFImporterRun.cpp"
    ]
}
