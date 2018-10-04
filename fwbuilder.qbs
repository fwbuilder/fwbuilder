import qbs 1.0
import "qbs/imports/Git.qbs" as Git

Project {
    name: "fwbuilder"
    qbsSearchPaths: "qbs"

    Git { id: git }

    property string version: git.version || "6.0.0-beta"
    property string xmlVersion: "24"
    property string installPrefix: (qbs.installPrefix ? qbs.installPrefix : '/usr')
    property string resourcesDir: {
        if (qbs.targetOS.contains("windows"))
            return "resources";
        else if (qbs.targetOS.contains("macos"))
            return "../Resources"
        else
            return "share"
    }
    property bool lto

    references: [
        "src/libfwbuilder/src/fwbuilder/fwbuilder.qbs",
        "src/libfwbuilder/src/fwcompiler/fwcompiler.qbs",
        "src/libfwbuilder/etc/etc.qbs",
        "src/libfwbuilder/migration/migration.qbs",
        "src/antlr/antlr.qbs",
        "src/import/import.qbs",
        "src/parsers/parsers.qbs",
        "src/common/common.qbs",
        "src/compiler_lib/compiler_lib.qbs",
        "src/juniper_lib/juniper_lib.qbs",
        "src/cisco_lib/cisco_lib.qbs",
        "src/pflib/pflib.qbs",
        "src/iptlib/iptlib.qbs",
        "src/ipfw/ipfw.qbs",
        "src/pf/pf.qbs",
        "src/iosacl/iosacl.qbs",
        "src/ipf/ipf.qbs",
        "src/nxosacl/nxosacl.qbs",
        "src/procurve_acl/procurve_acl.qbs",
        "src/ipt/ipt.qbs",
        "src/pix/pix.qbs",
        "src/junosacl/junosacl.qbs",
        "src/libgui/libgui.qbs",
        "src/gui/gui.qbs",
        "src/fwbedit/fwbedit.qbs",
        "src/res/res.qbs",
        "doc/doc.qbs",
	"packaging/packaging.qbs"
    ]
}
