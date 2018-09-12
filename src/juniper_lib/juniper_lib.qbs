import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "juniper_lib"
    targetName: "fwbjuniper"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "libfwcompiler" }
    Depends { name: "compiler_lib" }
    Depends { name: "cisco_lib" }
    Depends { name: "libxml2" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    files: [
        "CompilerDriver_junosacl.cpp",
        "CompilerDriver_junosacl_run.cpp",
        "OSConfigurator_junos.cpp",
        "../cisco_lib/PolicyCompiler_cisco.cpp",
        "../cisco_lib/Helper.cpp",
        "PolicyCompiler_junosacl.cpp",
        "PolicyCompiler_junosacl_writers.cpp",
        "../cisco_lib/NamedObjectsAndGroupsSupport.cpp",
        "../cisco_lib/NamedObject.cpp",
        "../cisco_lib/PolicyCompiler_cisco_acls.cpp",
        "../cisco_lib/BaseObjectGroup.cpp",
        "../cisco_lib/IOSObjectGroup.cpp",
        "../cisco_lib/NamedObjectsManager.cpp",
        "../cisco_lib/ACL.cpp",
        "../cisco_lib/NXOSObjectGroup.cpp",
        "../cisco_lib/PIXObjectGroup.cpp",
        "../cisco_lib/ASA8ObjectGroup.cpp"
    ]
}
