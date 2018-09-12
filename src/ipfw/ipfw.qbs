import qbs 1.0

FWBProduct {
    type: "application"
    name: "ipfw"
    targetName: "fwb_ipfw"

    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "widgets"
        ]
    }
    Depends { name: "libfwbuilder" }
    Depends { name: "compiler_lib" }
    Depends { name: "common" }
    Depends { name: "pflib" }
    Depends { name: "libxml2" }
    Depends { name: "libxslt" }
    Depends { name: "zlib" }

    files: [
        "ipfw.cpp",
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
