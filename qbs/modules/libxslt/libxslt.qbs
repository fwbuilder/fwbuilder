import qbs 1.0
import qbs.Probes
import qbs.Environment

Module {
    readonly property bool found: probe.found
    readonly property string packageVersion: probe.modversion

    Depends { name: "cpp" }
    Depends { name: "libxml2" }

    cpp.defines: probe.defines == undefined ? [] : probe.defines
    cpp.commonCompilerFlags: probe.compilerFlags == undefined ? [] : probe.compilerFlags
    cpp.includePaths: probe.includePaths == undefined ? [] : probe.includePaths
    cpp.libraryPaths: probe.libraryPaths == undefined ? [] : probe.libraryPaths
    cpp.dynamicLibraries: probe.libraries == undefined ? [] : probe.libraries
    cpp.linkerFlags: {
        if (qbs.toolchain.contains("mingw"))
            return [];
        return (probe.linkerFlags == undefined ? [] : probe.linkerFlags);
    }

    Probes.PkgConfigProbe {
        id: probe
        name: "libxslt"
        executable: {
        if (qbs.toolchain.contains("mingw"))
                return "i686-w64-mingw32.shared-pkg-config"
            else
                return "pkg-config"
        }
        sysroot: {
            if (qbs.targetOS.contains("macos"))
                return undefined;
            else
                return qbs.sysroot;
        }
    }
}
