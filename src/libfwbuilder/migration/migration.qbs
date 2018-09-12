import qbs 1.0

FWBProduct {
    type: "application"
    name: "migration"

    Git {
        id: git
    }

    Group {
        name: "FWBuilder migration files"
        files: ["*.xslt"]
        qbs.install: true
        qbs.installDir: {
            if (qbs.targetOS.contains("macos"))
                return project.name + ".app/Contents/Resources/";
            else if (qbs.targetOS.contains("windows"))
                return "resources"
            else
                return "share/fwbuilder-"+project.version
        }
        qbs.installSourceBase: ".."
    }
}
