import qbs 1.0

FWBProduct {
    type: ["application", "dtd_files"]
    name: "etc"

    Group {
        files: ["fwbuilder.dtd.in"]
        fileTags: "in_dtd_files"
    }

    InputFileRule {
        inputs: ["in_dtd_files"]
        artifactFileTags: ["dtd_files"]
    }

    Group {
        name: "FWBuilder DTD"
        fileTagsFilter: ["dtd_files"]
        qbs.install: true
        qbs.installDir: {
            if (qbs.targetOS.contains("macos"))
                return project.name + ".app/Contents/Resources/";
            else if (qbs.targetOS.contains("windows"))
                return "resources";
            else
                return "share/fwbuilder-"+project.version;
        }
    }
}
