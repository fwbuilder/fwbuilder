import qbs 1.0
import qbs.FileInfo

FWBProduct {
    type: ["application", "man_pages"]
    name: "doc"

    Group {
        name: "Docs"
        files: [
            "AUTHORS",
            "ChangeLog",
            "COPYING",
            "Credits",
            "FWBuilder-Routing-LICENSE.txt",
            "PatchAcceptancePolicy.txt",
            "README.*"
            ]
        qbs.install: qbs.targetOS.contains("linux") || qbs.targetOS.contains("windows")
        qbs.installDir: qbs.targetOS.contains("unix") ? "share/doc/fwbuilder-"+project.version : "doc"
    }

    Group {
        name: "Uncompressed man pages"
        files: [
            "fwbedit.1",
            "fwbuilder.1",
            "fwb_iosacl.1",
            "fwb_ipf.1",
            "fwb_ipfw.1",
            "fwb_ipt.1",
            "fwb_pf.1",
            "fwb_pix.1"
        ]
        fileTags: "source_man_pages"
        condition: !qbs.targetOS.contains("windows")
    }

    Rule {
        inputs: ["source_man_pages"]

        Artifact {
            filePath: input.fileName + ".gz"
            fileTags: ["man_pages"]
        }

        prepare: {
            var outPath = FileInfo.path(output.filePath);

            var copyCmd = new Command("cp", [input.filePath, [outPath, output.completeBaseName].join('/')]);
            copyCmd.silent = true;

            var compressCmd = new Command("gzip", [outPath, output.completeBaseName].join('/'));
            compressCmd.description = "Compressing man page: " + input.fileName;
            compressCmd.highlight = "codegen";

            return [copyCmd, compressCmd];
        }
    }

    Group {
        name: "Man pages"
        fileTagsFilter: "man_pages"
        qbs.install: qbs.targetOS.contains("linux")
        qbs.installDir: qbs.targetOS.contains("unix") ? "share/man/man1" : ""
    }
}
