import qbs 1.0

FWBProduct {
    type: ["nsi"]
    name: "packaging"

    Group {
        files: ["fwbuilder.nsi.in"]
        fileTags: "nsi.in"
    }

    InputFileRule {
        inputs: ["nsi.in"]
        artifactFileTags: ["nsi"]
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        name: "NSIS Installer script"
        fileTagsFilter: ["nsi"]
        qbs.install: true
    }

    Group {
        condition: qbs.targetOS.contains("windows")
        name: "NSIS Installer images"
        files: [
            "fwbuilder-160x60.bmp",
            "fwbuilder-164x314.bmp"
        ]
        qbs.install: true
    }
}
