import qbs 1.0

FWBProduct {
    type: "staticlibrary"
    name: "libfwbuilder"
    targetName: "fwbuilder"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "libxslt" }
    Depends { name: "libsnmp" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory + "/.."
    }

    cpp.includePaths: base.concat(['..'])
    cpp.cxxFlags: qbs.targetOS.contains("windows") ? base : base.concat(["-Wno-parentheses"])
    cpp.cFlags: qbs.targetOS.contains("windows") ? base : base.concat(["-Wno-shift-negative-value"])

    files: [
        "InetAddr.cpp",
        "InetAddrMask.cpp",
        "Inet6AddrMask.cpp",
        "IPRoute.cpp",
        "Address.cpp",
        "AddressRange.cpp",
        "BackgroundOp.cpp",
        "Constants.cpp",
        "CustomService.cpp",
        "dns.cpp",
        "Firewall.cpp",
        "Cluster.cpp",
        "ClusterGroup.cpp",
        "FailoverClusterGroup.cpp",
        "StateSyncClusterGroup.cpp",
        "FWException.cpp",
        "FWIntervalReference.cpp",
        "FWObject.cpp",
        "FWObjectDatabase.cpp",
        "FWObjectDatabase_create_object.cpp",
        "FWObjectDatabase_tree_ops.cpp",
        "FWObjectDatabase_search.cpp",
        "FWObjectReference.cpp",
        "FWOptions.cpp",
        "FWReference.cpp",
        "FWServiceReference.cpp",
        "Group.cpp",
        "Host.cpp",
        "ICMPService.cpp",
        "ICMP6Service.cpp",
        "Interface.cpp",
        "InterfaceData.cpp",
        "Interval.cpp",
        "IntervalGroup.cpp",
        "IPService.cpp",
        "IPv4.cpp",
        "IPv6.cpp",
        "Library.cpp",
        "Logger.cpp",
        "Management.cpp",
        "MultiAddress.cpp",
        "NAT.cpp",
        "Network.cpp",
        "NetworkIPv6.cpp",
        "AttachedNetworks.cpp",
        "ObjectGroup.cpp",
        "DynamicGroup.cpp",
        "physAddress.cpp",
        "DNSName.cpp",
        "AddressTable.cpp",
        "Policy.cpp",
        "Resources.cpp",
        "Routing.cpp",
        "Rule.cpp",
        "RuleElement.cpp",
        "RuleSet.cpp",
        "SecuwallMgmtFile.cpp",
        "Service.cpp",
        "ServiceGroup.cpp",
        "snmp.cpp",
        "TCPService.cpp",
        "ThreadTools.cpp",
        "Tools.cpp",
        "TCPUDPService.cpp",
        "UDPService.cpp",
        "UserService.cpp",
        "TagService.cpp",
        "XMLTools.cpp",
        "ObjectMatcher.cpp",
        "ObjectMirror.cpp",
        "inet_net_ntop.c",
        "inet_net_pton.c",
        "uint128.cpp"
    ]
}
