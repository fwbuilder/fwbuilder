import qbs 1.0
import qbs.Process

Module {
    readonly property bool found: probe.found
    readonly property string packageVersion: probe.modversion

    Depends { name: "cpp" }

    cpp.defines: probe.defines == undefined ? [] : probe.defines
    cpp.commonCompilerFlags: probe.compilerFlags == undefined ? [] : probe.compilerFlags
    cpp.includePaths: probe.includePaths == undefined ? [] : probe.includePaths
    cpp.libraryPaths: probe.libraryPaths == undefined ? [] : probe.libraryPaths
    cpp.dynamicLibraries: probe.libraries == undefined ? [] : probe.libraries
    cpp.linkerFlags: probe.linkerFlags == undefined ? [] : probe.linkerFlags

    Probe {
        id: probe

        property string executable: 'net-snmp-config'

        property stringList cflags
        property stringList libs

        property stringList defines
        property stringList libraries
        property stringList libraryPaths
        property stringList includePaths
        property stringList compilerFlags
        property stringList linkerFlags
        property string modversion
        
        configure: {
            var p = new Process()
            try {
                if (p.exec(executable, ['--version']) === 0) {
                    modversion = p.readStdOut().trim()
                    if (p.exec(executable, ['--netsnmp-libs']) === 0) {
                        libs = p.readStdOut().trim()
                        libs = libs ? libs.split(/\s/) : []
                        libraries = []
                        libraryPaths = []
                        linkerFlags = []
                        for (var i = 0; i < libs.length; ++i) {
                            var flag = libs[i]
                            if (flag.startsWith("-l"))
                                libraries.push(flag.slice(2))
                            else if (flag.startsWith("-L"))
                                libraryPaths.push(flag.slice(2))
                            else
                                linkerFlags.push(flag)
                        }
                    }
                    defines = ['HAVE_LIBSNMP=1', 'NET_SNMP=1']
                    console.debug("Found net-snmp " + modversion)
                    found = true
                } else {
                    found: false
                }
            } finally {
                p.close()
            }
        }
    }
}
