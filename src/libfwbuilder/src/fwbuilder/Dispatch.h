/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: Dispatch.h 516 2010-01-26 03:25:44Z vadim $


  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef  __DISPATCH_HH_FLAG__
#define  __DISPATCH_HH_FLAG__

#include <string>
#include <list>
#include <assert.h>


namespace libfwbuilder {
    class InetAddr;
    class InetAddrMask;

    class Address;
    class AddressRange;
    class AddressTable;
    class Cluster;
    class ClusterGroup;
    class ClusterGroupOptions;
    class CustomService;
    class DNSName;
    class FWBDManagement;
    class FWIntervalReference;
    class FWObject;
    class FWObjectDatabase;
    class FWObjectReference;
    class FWOptions;
    class FWReference;
    class FWServiceReference;
    class FailoverClusterGroup;
    class Firewall;
    class FirewallOptions;
    class Group;
    class Host;
    class HostOptions;
    class ICMP6Service;
    class ICMPService;
    class IPService;
    class IPv4;
    class IPv6;
    class Interface;
    class InterfaceOptions;
    class Interval;
    class IntervalGroup;
    class Library;
    class Management;
    class MultiAddress;
    class MultiAddressRunTime;
    class NAT;
    class NATRule;
    class NATRuleOptions;
    class Network;
    class NetworkIPv6;
    class ObjectGroup;
    class Policy;
    class PolicyInstallScript;
    class PolicyRule;
    class PolicyRuleOptions;
    class Routing;
    class RoutingRule;
    class RoutingRuleOptions;
    class Rule;
    class RuleElement;
    class RuleElementDst;
    class RuleElementInterval;
    class RuleElementItf;
    class RuleElementODst;
    class RuleElementOSrc;
    class RuleElementOSrv;
    class RuleElementRDst;
    class RuleElementRGtw;
    class RuleElementRItf;
    class RuleElementSrc;
    class RuleElementSrv;
    class RuleElementTDst;
    class RuleElementTSrc;
    class RuleElementTSrv;
    class RuleSet;
    class RuleSetOptions;
    class SNMPManagement;
    class Service;
    class ServiceGroup;
    class StateSyncClusterGroup;
    class TCPService;
    class TCPUDPService;
    class TagService;
    class UDPService;
    class UserService;
    class physAddress;
    class DynamicGroup;
};

/*
 * Keeping return nullptr; after assert() to make sure the function
 * returns something even if compiled with  -DNDEBUG that disables assert()
 * and to make gcc happy at compile time.
 */
#define __DECLARE_DISPATCH_METHODS(classname) \
 virtual void* dispatch(classname*, void*) \
 { assert("Call to unimplemented virtual void* Dispatch::dispatch("#classname"*, void*)"==nullptr); \
 return nullptr; } \
 virtual void* dispatch(classname*, const void*) \
 { assert("Call to unimplemented virtual void* Dispatch::dispatch("#classname"*, const void*)"==nullptr); \
 return nullptr; } \
 virtual void* dispatch(const classname*, void*) \
 { assert("Call to unimplemented virtual void* Dispatch::dispatch(const "#classname"*, void*)"==nullptr); \
 return nullptr; } \
 virtual void* dispatch(const classname*, const void*) \
 { assert("Call to unimplemented virtual void* Dispatch::dispatch(const "#classname"*, const void*)"==nullptr); \
 return nullptr; } \
 virtual const void* const_dispatch(const classname*, void*) \
 { assert("Call to unimplemented virtual const void* Dispatch::const_dispatch(const "#classname"*, void*)"==nullptr); \
 return nullptr; } \
 virtual const void* const_dispatch(const classname*, const void*) \
 { assert("Call to unimplemented virtual const void* Dispatch::const_dispatch(const "#classname"*, const void*)"==nullptr); \
 return nullptr; }\

namespace libfwbuilder
{

    class Dispatch {

public:
        Dispatch() {}
        virtual ~Dispatch() {}

        __DECLARE_DISPATCH_METHODS(Address);
        __DECLARE_DISPATCH_METHODS(AddressRange);
        __DECLARE_DISPATCH_METHODS(AddressTable);
        __DECLARE_DISPATCH_METHODS(Cluster);
        __DECLARE_DISPATCH_METHODS(ClusterGroup);
        __DECLARE_DISPATCH_METHODS(ClusterGroupOptions);
        __DECLARE_DISPATCH_METHODS(CustomService);
        __DECLARE_DISPATCH_METHODS(DNSName);
        __DECLARE_DISPATCH_METHODS(FWBDManagement);
        __DECLARE_DISPATCH_METHODS(FWIntervalReference);
        __DECLARE_DISPATCH_METHODS(FWObject);
        __DECLARE_DISPATCH_METHODS(FWObjectDatabase);
        __DECLARE_DISPATCH_METHODS(FWObjectReference);
        __DECLARE_DISPATCH_METHODS(FWOptions);
        __DECLARE_DISPATCH_METHODS(FWReference);
        __DECLARE_DISPATCH_METHODS(FWServiceReference);
        __DECLARE_DISPATCH_METHODS(FailoverClusterGroup);
        __DECLARE_DISPATCH_METHODS(Firewall);
        __DECLARE_DISPATCH_METHODS(FirewallOptions);
        __DECLARE_DISPATCH_METHODS(Group);
        __DECLARE_DISPATCH_METHODS(Host);
        __DECLARE_DISPATCH_METHODS(HostOptions);
        __DECLARE_DISPATCH_METHODS(ICMP6Service);
        __DECLARE_DISPATCH_METHODS(ICMPService);
        __DECLARE_DISPATCH_METHODS(IPService);
        __DECLARE_DISPATCH_METHODS(IPv4);
        __DECLARE_DISPATCH_METHODS(IPv6);
        __DECLARE_DISPATCH_METHODS(Interface);
        __DECLARE_DISPATCH_METHODS(InterfaceOptions);
        __DECLARE_DISPATCH_METHODS(Interval);
        __DECLARE_DISPATCH_METHODS(IntervalGroup);
        __DECLARE_DISPATCH_METHODS(Library);
        __DECLARE_DISPATCH_METHODS(Management);
        __DECLARE_DISPATCH_METHODS(MultiAddress);
        __DECLARE_DISPATCH_METHODS(MultiAddressRunTime);
        __DECLARE_DISPATCH_METHODS(NAT);
        __DECLARE_DISPATCH_METHODS(NATRule);
        __DECLARE_DISPATCH_METHODS(NATRuleOptions);
        __DECLARE_DISPATCH_METHODS(Network);
        __DECLARE_DISPATCH_METHODS(NetworkIPv6);
        __DECLARE_DISPATCH_METHODS(ObjectGroup);
        __DECLARE_DISPATCH_METHODS(Policy);
        __DECLARE_DISPATCH_METHODS(PolicyInstallScript);
        __DECLARE_DISPATCH_METHODS(PolicyRule);
        __DECLARE_DISPATCH_METHODS(PolicyRuleOptions);
        __DECLARE_DISPATCH_METHODS(Routing);
        __DECLARE_DISPATCH_METHODS(RoutingRule);
        __DECLARE_DISPATCH_METHODS(RoutingRuleOptions);
        __DECLARE_DISPATCH_METHODS(Rule);
        __DECLARE_DISPATCH_METHODS(RuleElement);
        __DECLARE_DISPATCH_METHODS(RuleElementDst);
        __DECLARE_DISPATCH_METHODS(RuleElementInterval);
        __DECLARE_DISPATCH_METHODS(RuleElementItf);
        __DECLARE_DISPATCH_METHODS(RuleElementODst);
        __DECLARE_DISPATCH_METHODS(RuleElementOSrc);
        __DECLARE_DISPATCH_METHODS(RuleElementOSrv);
        __DECLARE_DISPATCH_METHODS(RuleElementRDst);
        __DECLARE_DISPATCH_METHODS(RuleElementRGtw);
        __DECLARE_DISPATCH_METHODS(RuleElementRItf);
        __DECLARE_DISPATCH_METHODS(RuleElementSrc);
        __DECLARE_DISPATCH_METHODS(RuleElementSrv);
        __DECLARE_DISPATCH_METHODS(RuleElementTDst);
        __DECLARE_DISPATCH_METHODS(RuleElementTSrc);
        __DECLARE_DISPATCH_METHODS(RuleElementTSrv);
        __DECLARE_DISPATCH_METHODS(RuleSet);
        __DECLARE_DISPATCH_METHODS(RuleSetOptions);
        __DECLARE_DISPATCH_METHODS(SNMPManagement);
        __DECLARE_DISPATCH_METHODS(Service);
        __DECLARE_DISPATCH_METHODS(ServiceGroup);
        __DECLARE_DISPATCH_METHODS(StateSyncClusterGroup);
        __DECLARE_DISPATCH_METHODS(TCPService);
        __DECLARE_DISPATCH_METHODS(TCPUDPService);
        __DECLARE_DISPATCH_METHODS(TagService);
        __DECLARE_DISPATCH_METHODS(UDPService);
        __DECLARE_DISPATCH_METHODS(UserService);
        __DECLARE_DISPATCH_METHODS(physAddress);
        __DECLARE_DISPATCH_METHODS(DynamicGroup);
    };
};

#endif
