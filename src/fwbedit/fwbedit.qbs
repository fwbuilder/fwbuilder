import qbs 1.0

FWBProduct {
    type: "application"
    name: "fwbedit"

    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "widgets",
            "printsupport"
        ]
    }

    Depends { name: "cpp" }
    Depends { name: "libfwbuilder" }
    Depends { name: "compiler_lib" }
    Depends { name: "libimport" }
    Depends { name: "common" }
    Depends { name: "libgui" }
    Depends { name: "libxml2" }
    Depends { name: "libxslt" }
    Depends { name: "zlib" }

    files: [
        "fwbedit.cpp",
        "new_object.cpp",
        "repair_tree.cpp",
        "list_object.cpp",
        "merge.cpp",
        "import.cpp",
        "../libgui/FWBTree.cpp",
        "../libgui/platforms.cpp"
    ]

    Group {
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: {
            if (qbs.targetOS.contains("windows"))
                return "";
            if (qbs.targetOS.contains("macos"))
                return project.name + ".app/Contents/MacOS/";
            return "bin";
        }
    }
}
