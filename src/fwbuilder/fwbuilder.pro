#-*- mode: makefile; tab-width: 4; -*-
#
include(../../qmake.inc)
#
VERSION = $$SO_VERSION
#
SOURCES  = 	inet_net_ntop.c \
			inet_net_pton.c \
			InetAddr.cpp \
			Inet6Addr.cpp \
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
			FWException.cpp \
			FWIntervalReference.cpp \
			FWObject.cpp \
			FWObjectDatabase.cpp \
			FWObjectReference.cpp \
			FWOptions.cpp \
			FWReference.cpp \
			FWServiceReference.cpp \
			Group.cpp \
			Host.cpp \
			HostsFile.cpp \
			ICMPService.cpp \
			Interface.cpp \
			InterfacePolicy.cpp \
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
			ObjectGroup.cpp \
			physAddress.cpp \
            DNSName.cpp\
            AddressTable.cpp\
			Policy.cpp \
			Resources.cpp \
			Routing.cpp \
			Rule.cpp \
			RuleElement.cpp \
			RuleSet.cpp \
			Service.cpp \
			ServiceGroup.cpp \
			snmp.cpp \
			TCPService.cpp \
			ThreadTools.cpp \
			Tools.cpp \
			UDPService.cpp \
			UserService.cpp \
            TagService.cpp \
			XMLTools.cpp 


HEADERS  = 	inet_net.h \
			InetAddr.h \
			Inet6Addr.h \
			InetAddrMask.h \
			Inet6AddrMask.h \
			IPRoute.h \
			Address.h \
			AddressRange.h \
			BackgroundOp.h \
			Constants.h \
			CustomService.h \
			dns.h \
			Firewall.h \
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
			HostsFile.h \
			ICMPService.h \
			Interface.h \
			InterfacePolicy.h \
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
			ObjectGroup.h \
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
			ServiceGroup.h \
			Service.h \
			snmp.h \
			SyncQueue.h \
			TCPService.h \
			ThreadTools.h \
			Tools.h \
			UDPService.h \
			UserService.h \
            TagService.h \
			XMLTools.h

headers.files =$$HEADERS
headers.path  =$$prefix/include/fwb-3/fwbuilder

TARGET    = fwbuilder

INSTALLS += headers
