import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "common"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "libfwcompiler" }
    Depends { name: "libxml2" }
    Depends { name: "libimport" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "init.cpp",
        "init2.cpp"
    ]

}
