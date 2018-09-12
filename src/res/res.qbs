import qbs 1.0
import qbs.File
import qbs.TextFile

FWBProduct {
    type: ["application", "object_def_files"]
    name: "res"

    property string installDir: {
        if (qbs.targetOS.contains("macos"))
            return "fwbuilder.app/Contents/Resources"
        else if (qbs.targetOS.contains("windows"))
            return "resources"
        else
            return "share/fwbuilder-"+project.version
    }

    Group {
        files: ["objects_init.xml.in", "templates.xml.in"]
        fileTags: "in_object_def_files"
    }

    InputFileRule {
        inputs: ["in_object_def_files"]
        artifactFileTags: ["object_def_files"]
        
    }

    Group {
        name: "Objects definitions"
        files: ["resources.xml"]
        qbs.install: true
        qbs.installDir: installDir

        Group {
            fileTagsFilter: "object_def_files"
        }
    }

    Group {
        name: "OS files"
        files: "os/*.xml"
        qbs.install: true
        qbs.installDir: installDir
        qbs.installSourceBase: "."
    }

    Group {
        name: "Platform files"
        files: "platform/*.xml"
        qbs.install: true
        qbs.installDir: installDir
        qbs.installSourceBase: "."
    }

    Group {
        name: "Help files"
        files: ["help/en_US/*.html", "help/en_US/*.png", "help/en_US/*.jpg"]
        qbs.install: true
        qbs.installDir: installDir
        qbs.installSourceBase: "."
    }

    Group {
        name: "Configlets"
        files: ["configlets/**/*"]
        qbs.install: true
        qbs.installDir: installDir
        qbs.installSourceBase: "."
    }

    Group {
        name: "Freedesktop Shortcut"
        files: ["fwbuilder.desktop"]
        qbs.install: qbs.targetOS.contains("linux")
        qbs.installDir: "share/applications"
    }

    Group {
        name: "Icons"
        qbs.install: qbs.targetOS.contains("linux")
        qbs.installDir: "share/icons/hicolor"

        Group {
            files: ["Icons/16x16/fwbuilder.png"]
            qbs.installDir: outer.concat("/16x16/apps/")
        }

        Group {
            files: ["Icons/24x24/fwbuilder.png"]
            qbs.installDir: outer.concat("/24x24/apps/")
        }

        Group {
            files: ["Icons/32x32/fwbuilder.png"]
            qbs.installDir: outer.concat("/32x32/apps/")
        }

        Group {
            files: ["Icons/48x48/fwbuilder.png"]
            qbs.installDir: outer.concat("/48x48/apps/")
        }

        Group {
            files: ["Icons/72x72/fwbuilder.png"]
            qbs.installDir: outer.concat("/72x72/apps/")
        }

        Group {
            files: ["Icons/256x256/fwbuilder.png"]
            qbs.installDir: outer.concat("/256x256/apps/")
        }

        Group {
            files: ["Icons/512x512/fwbuilder.png"]
            qbs.installDir: outer.concat("/512x512/apps/")
        }
    }
}
