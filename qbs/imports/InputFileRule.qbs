import qbs 1.0
import qbs.TextFile

Rule {
    property stringList artifactFileTags

    Artifact {
        filePath: input.completeBaseName
        fileTags: artifactFileTags
    }

    prepare: {
        var cmd = new JavaScriptCommand();
        cmd.description = input.fileName + " -> " + output.fileName;
        cmd.highlight = "codegen";
        cmd.sourceCode = function() {
            var inFile = new TextFile(input.filePath, TextFile.ReadOnly)
            var contents = inFile.readAll();
            inFile.close();

            var vars = {
                FWBUILDER_XML_VERSION: project.xmlVersion,
                VERSION: project.version,
                GENERATION: project.version.slice(0, project.version.lastIndexOf('.')),
                GENERATION_SHORT: project.version.slice(0, project.version.lastIndexOf('.')).replace('.', '')
            };

            for (var i in vars) {
                contents = contents.replace(new RegExp('@' + i + '@(?!\w)', 'g'), vars[i]);
            }

            var file = new TextFile(output.filePath, TextFile.WriteOnly);
            file.write(contents);
            file.close();
        };
        return [cmd];
    }
}
