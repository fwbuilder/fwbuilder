import qbs 1.0

FWBProduct {
    type: "application"
    name: "gui"
    targetName: "fwbuilder"
    bundle.isBundle: qbs.targetOS.contains("macos")
    consoleApplication: false

    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "widgets",
            "network",
            "printsupport"
        ]
    }

    Depends { name: "bundle" }
    Depends { name: "libfwbuilder" }
    Depends { name: "common" }
    Depends { name: "antlr" }
    Depends { name: "parsers" }
    Depends { name: "compiler_lib" }
    Depends { name: "libimport" }
    Depends { name: "libgui" }
    Depends { name: "libxml2" }
    Depends { name: "libxslt" }
    Depends { name: "zlib" }
    Depends { name: "libsnmp" }

    cpp.dynamicLibraries: {
        if (qbs.targetOS.contains("unix"))
            return base.concat(["util"]);
        return base;
    }

    files: [
        "main.cpp"
    ]

    Group {
        condition: qbs.targetOS.contains("windows")
        files: [
            "fwbuilder-windows.rc"
        ]
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        files: [
            "fwbuilder-windows.ico"
        ]
        qbs.install: true
    }

    Group {
        fileTagsFilter: "application"
        qbs.install: !bundle.isBundle
        qbs.installDir: {
            if (qbs.targetOS.contains("windows"))
                return "";
            if (qbs.targetOS.contains("macos"))
                return "";
            return "bin";
        }
    }

    Group {
        fileTagsFilter: ["bundle.content"]
        qbs.install: true
        qbs.installDir: '.'
        qbs.installSourceBase: product.buildDirectory
    }
}
