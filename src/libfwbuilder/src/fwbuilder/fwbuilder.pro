#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += "../../../../"

#
SOURCES  = 	InetAddr.cpp \
			InetAddrMask.cpp \
			Inet6AddrMask.cpp \
			IPRoute.cpp \
			Address.cpp \
			AddressRange.cpp \
			BackgroundOp.cpp \
			Constants.cpp \
			CustomService.cpp \
			dns.cpp \
			Firewall.cpp \
			Cluster.cpp \
			ClusterGroup.cpp \
			FailoverClusterGroup.cpp \
			StateSyncClusterGroup.cpp \
			FWException.cpp \
			FWIntervalReference.cpp \
			FWObject.cpp \
			FWObjectDatabase.cpp \
			FWObjectDatabase_create_object.cpp \
			FWObjectDatabase_tree_ops.cpp \
			FWObjectDatabase_search.cpp \
			FWObjectReference.cpp \
			FWOptions.cpp \
			FWReference.cpp \
			FWServiceReference.cpp \
			Group.cpp \
			Host.cpp \
			ICMPService.cpp \
			ICMP6Service.cpp \
			Interface.cpp \
			InterfaceData.cpp \
			Interval.cpp \
			IntervalGroup.cpp \
			IPService.cpp \
			IPv4.cpp \
			IPv6.cpp \
			Library.cpp \
			Logger.cpp \
			Management.cpp \
			MultiAddress.cpp \
			NAT.cpp \
			Network.cpp \
			NetworkIPv6.cpp \
			AttachedNetworks.cpp \
			ObjectGroup.cpp \
			DynamicGroup.cpp \
			physAddress.cpp \
            DNSName.cpp\
            AddressTable.cpp\
			Policy.cpp \
			Resources.cpp \
			Routing.cpp \
			Rule.cpp \
			RuleElement.cpp \
			RuleSet.cpp \
			SecuwallMgmtFile.cpp \
			Service.cpp \
			ServiceGroup.cpp \
			snmp.cpp \
			TCPService.cpp \
			ThreadTools.cpp \
			Tools.cpp \
			TCPUDPService.cpp \
			UDPService.cpp \
			UserService.cpp \
            TagService.cpp \
			XMLTools.cpp \
			ObjectMatcher.cpp \
			ObjectMirror.cpp \
			inet_net_ntop.c \
			inet_net_pton.c \
			uint128.cpp

HEADERS  = 	inet_net.h \
			uint128.h \
			InetAddr.h \
			InetAddrMask.h \
			Inet6AddrMask.h \
			Dispatch.h \
			IPRoute.h \
			Address.h \
			AddressRange.h \
			BackgroundOp.h \
			Constants.h \
			CustomService.h \
			dns.h \
			Firewall.h \
			Cluster.h \
			ClusterGroup.h \
			FailoverClusterGroup.h \
			StateSyncClusterGroup.h \
			FWException.h \
			FWIntervalReference.h \
			FWObjectDatabase.h \
			FWObject.h \
			FWObjectReference.h \
			FWOptions.h \
			FWReference.h \
			FWServiceReference.h \
			Group.h \
			Host.h \
			ICMPService.h \
			ICMP6Service.h \
			Interface.h \
			InterfaceData.h \
			IntervalGroup.h \
			Interval.h \
			IPService.h \
			IPv4.h \
			IPv6.h \
			libfwbuilder-config.h \
			libfwbuilder-version.h \
			Library.h \
			Logger.h \
			Management.h \
			MultiAddress.h \
			NAT.h \
			Network.h \
			NetworkIPv6.h \
			AttachedNetworks.h \
			ObjectGroup.h \
			DynamicGroup.h \
			physAddress.h \
            DNSName.h\
            AddressTable.h\
			Policy.h \
			Pool.h \
			Resources.h \
			Routing.h \
			RuleElement.h \
			Rule.h \
			RuleSet.h \
			SecuwallMgmtFile.h \
			ServiceGroup.h \
			Service.h \
			snmp.h \
			SyncQueue.h \
			TCPService.h \
			ThreadTools.h \
			Tools.h \
			TCPUDPService.h \
			UDPService.h \
			UserService.h \
            TagService.h \
			ObjectMatcher.h \
			ObjectMirror.h \
			XMLTools.h

TARGET    = fwbuilder

# no need to install headers 
#headers.files = $$HEADERS
#headers.path  = "$$prefix/include/fwb-4/fwbuilder"
#INSTALLS += headers

# and no need to install .a library
INSTALLS -= target
