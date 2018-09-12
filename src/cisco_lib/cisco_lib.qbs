import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "cisco_lib"
    targetName: "fwbcisco"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libfwbuilder" }
    Depends { name: "compiler_lib" }
    Depends { name: "libfwcompiler" }
    Depends { name: "libxml2" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory, '..']
    }

    files: [
        "PolicyCompiler_cisco.cpp",
        "PolicyCompiler_cisco_acls.cpp",
        "NamedObjectsAndGroupsSupport.cpp",
        "NamedObjectsManager.cpp",
        "NamedObjectsManagerNXOS.cpp",
        "NamedObjectsManagerIOS.cpp",
        "NamedObjectsManagerPIX.cpp",
        "RoutingCompiler_cisco.cpp",
        "RoutingCompiler_cisco_writers.cpp",
        "splitByNetworkZonesForRE.cpp",
        "specialServices.cpp",
        "ACL.cpp",
        "NamedObject.cpp",
        "ASA8TwiceNatLogic.cpp",
        "Helper.cpp",
        "inspectionProtocol.cpp",
        "inspectionClassMap.cpp",
        "OSConfigurator_nxos.cpp",
        "OSConfigurator_ios.cpp",
        "CompilerDriver_nxosacl.cpp",
        "CompilerDriver_nxosacl_run.cpp",
        "CompilerDriver_iosacl.cpp",
        "CompilerDriver_iosacl_run.cpp",
        "PolicyCompiler_nxosacl.cpp",
        "PolicyCompiler_nxosacl_writers.cpp",
        "PolicyCompiler_iosacl.cpp",
        "PolicyCompiler_iosacl_writers.cpp",
        "RoutingCompiler_nxosacl.cpp",
        "RoutingCompiler_nxosacl_writers.cpp",
        "RoutingCompiler_iosacl.cpp",
        "RoutingCompiler_iosacl_writers.cpp",
        "CompilerDriver_pix.cpp",
        "CompilerDriver_pix_run.cpp",
        "NATCompiler_pix.cpp",
        "NATCompiler_pix_find_translations.cpp",
        "NATCompiler_pix_writers.cpp",
        "NATCompiler_asa8.cpp",
        "NATCompiler_asa8_writers.cpp",
        "NATCompiler_pix_optimizers.cpp",
        "OSConfigurator_pix_os.cpp",
        "OSConfigurator_pix_os_fixups.cpp",
        "OSConfigurator_pix_os_inspectors.cpp",
        "OSConfigurator_pix_os_inspectors_pix8.cpp",
        "CompilerDriver_procurve_acl.cpp",
        "CompilerDriver_procurve_acl_run.cpp",
        "OSConfigurator_procurve.cpp",
        "PolicyCompiler_procurve_acl.cpp",
        "PolicyCompiler_procurve_acl_writers.cpp",
        "RoutingCompiler_procurve_acl.cpp",
        "BaseObjectGroup.cpp",
        "PIXObjectGroup.cpp",
        "ASA8ObjectGroup.cpp",
        "NXOSObjectGroup.cpp",
        "IOSObjectGroup.cpp",
        "PolicyCompiler_pix.cpp",
        "PolicyCompiler_pix_writers.cpp",
        "PolicyCompiler_pix_v6_acls.cpp",
        "PolicyCompiler_pix_replace_translations.cpp",
        "RoutingCompiler_pix.cpp",
        "RoutingCompiler_pix_writers.cpp",
        "AutomaticRules_cisco.cpp",
        "AutomaticRules_iosacl.cpp",
        "AutomaticRules_nxosacl.cpp"
    ]
}
