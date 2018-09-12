import qbs 1.0

Product {

    Depends { name: "cpp" }

    property string resDir: {
        if (qbs.targetOS.contains("windows"))
            return "resources";
        else if (qbs.targetOS.contains("macos"))
            return "../Resources";
        else
            return project.installPrefix + '/share/fwbuilder-' + project.version;
    }

    qbs.optimization: "fast"
    cpp.discardUnusedData: true
    cpp.cxxLanguageVersion: "c++14"
    cpp.includePaths: [product.sourceDirectory]

    cpp.dynamicLibraries: {
        if (qbs.targetOS.contains("windows"))
            return ["pthread"];
        else
            return [];
    }

    cpp.defines: [
        'WIN32_LEAN_AND_MEAN',
        '__STDC_FORMAT_MACROS',
        'GENERATION="' + project.version.slice(0,project.version.lastIndexOf('.')) + '"',
        'VERSION="' + project.version + '"',
        'PREFIX="' + project.installPrefix + '"',
        'RES_DIR="' + resDir + '"',
        'FS_SEPARATOR="/"',
        'FWBUILDER_XML_VERSION="' + project.xmlVersion + '"'
    ]

    cpp.cxxFlags: {
        return base.concat(["-Wall", "-Wextra"]);
    }
}
