import qbs 1.0

FWBProduct {
    type: "application"
    name: "ipt"
    targetName: "fwb_ipt"

    Depends {
        name: "Qt"
        submodules: [
            "core",
            "gui",
            "widgets"
        ]
    }

    Depends { name: "libfwbuilder" }
    Depends { name: "libfwcompiler" }
    Depends { name: "compiler_lib" }
    Depends { name: "common" }
    Depends { name: "iptlib" }
    Depends { name: "libxml2" }
    Depends { name: "libxslt" }
    Depends { name: "zlib" }

    files: [
        "ipt.cpp",
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
