project(libfwbuilder)

set(libfwbuilder_srcs
    InetAddr.cpp 
    InetAddrMask.cpp 
    Inet6AddrMask.cpp 
    IPRoute.cpp 
    Address.cpp 
    AddressRange.cpp 
    BackgroundOp.cpp 
    Constants.cpp 
    CustomService.cpp 
    dns.cpp 
    Firewall.cpp 
    Cluster.cpp 
    ClusterGroup.cpp 
    FailoverClusterGroup.cpp 
    StateSyncClusterGroup.cpp
    FWException.cpp 
    FWIntervalReference.cpp 
    FWObject.cpp 
    FWObjectDatabase.cpp 
    FWObjectDatabase_create_object.cpp 
    FWObjectDatabase_tree_ops.cpp 
    FWObjectDatabase_search.cpp 
    FWObjectReference.cpp 
    FWOptions.cpp 
    FWReference.cpp 
    FWServiceReference.cpp 
    Group.cpp
    Host.cpp 
    ICMPService.cpp 
    ICMP6Service.cpp 
    Interface.cpp 
    InterfaceData.cpp 
    Interval.cpp 
    IntervalGroup.cpp 
    IPService.cpp 
    IPv4.cpp 
    IPv6.cpp 
    Library.cpp 
    Logger.cpp 
    Management.cpp 
    MultiAddress.cpp 
    NAT.cpp 
    Network.cpp 
    NetworkIPv6.cpp 
    AttachedNetworks.cpp 
    ObjectGroup.cpp 
    DynamicGroup.cpp 
    physAddress.cpp 
    DNSName.cpp
    AddressTable.cpp
    Policy.cpp 
    Resources.cpp 
    Routing.cpp 
    Rule.cpp 
    RuleElement.cpp 
    RuleSet.cpp 
    SecuwallMgmtFile.cpp 
    Service.cpp 
    ServiceGroup.cpp 
    snmp.cpp 
    TCPService.cpp 
    ThreadTools.cpp 
    Tools.cpp 
    TCPUDPService.cpp 
    UDPService.cpp 
    UserService.cpp 
    TagService.cpp 
    XMLTools.cpp 
    ObjectMatcher.cpp 
    ObjectMirror.cpp 
    inet_net_ntop.c 
    inet_net_pton.c 
    uint128.cpp)

add_library(fwbuilder STATIC ${libfwbuilder_srcs})

target_include_directories(fwbuilder PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..
    ${LIBXML2_INCLUDE_DIR})

set(CXX_EXTRA_FLAGS # clang/GCC
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wno-parentheses>)

set(C_EXTRA_FLAGS # clang/GCC
    $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:GNU>>:-Wno-shift-negative-value>)

target_compile_options(fwbuilder PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${CXX_DEFAULT_FLAGS} ${CXX_EXTRA_FLAGS}>
	$<$<COMPILE_LANGUAGE:C>:${C_EXTRA_FLAGS}>)

