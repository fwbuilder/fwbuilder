import qbs 1.0
import qbs.Probes
import qbs.Environment

Module {
    readonly property bool found: probe.found
    readonly property string packageVersion: probe.modversion

    Depends { name: "cpp" }

    cpp.defines: probe.defines == undefined ? [] : probe.defines
    cpp.commonCompilerFlags: probe.compilerFlags == undefined ? [] : probe.compilerFlags
    cpp.linkerFlags: probe.linkerFlags == undefined ? [] : probe.linkerFlags

    cpp.includePaths: {
        if (qbs.toolchain.contains("msvc")) {
            var includePath = Environment.getEnv('ZLIB_INC_DIR');
            if (includePath === '')
                console.error('ZLIB_INC_DIR is not set');
            else
                return includePath;
        }
        if (probe.includePaths)
                        return probe.includePaths;
        return [];
    }

    cpp.libraryPaths: {
        if (qbs.toolchain.contains("msvc")) {
            var libraryPath = Environment.getEnv('ZLIB_LIB_DIR');
            if (libraryPath === '')
                console.error('ZLIB_LIB_DIR is not set');
            else
                return libraryPath;
        }
        if (probe.libraryPaths)
                        return probe.libraryPaths;
        return [];
    }

    cpp.dynamicLibraries: {
        if (qbs.toolchain.contains("msvc")) {
            var library = Environment.getEnv('ZLIB_LIB_NAME');
            if (library === '')
                console.error('ZLIB_LIB_NAME is not set');
            else
                return library;
        }
        if (probe.libraries)
                        return probe.libraries;
        return [];
    }

    Probes.PkgConfigProbe {
        id: probe
        name: "zlib"
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
