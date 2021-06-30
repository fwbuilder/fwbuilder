/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

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



#include "global.h"
#include "utils.h"
#include "platforms.h"

#include "DialogFactory.h"

#include "LibraryDialog.h"
#include "RuleSetDialog.h"
#include "AddressRangeDialog.h"
#include "IPv4Dialog.h"
#include "IPv6Dialog.h"
#include "PhysicalAddressDialog.h"
#include "DNSNameDialog.h"
#include "AddressTableDialog.h"
#include "AttachedNetworksDialog.h"
#include "NetworkDialog.h"
#include "NetworkDialogIPv6.h"
#include "UserDialog.h"
#include "CustomServiceDialog.h"
#include "ICMPServiceDialog.h"
#include "IPServiceDialog.h"
#include "TCPServiceDialog.h"
#include "UDPServiceDialog.h"
#include "GroupObjectDialog.h"
#include "DynamicGroupDialog.h"
#include "HostDialog.h"
#include "FirewallDialog.h"
#include "ClusterDialog.h"
#include "ClusterGroupDialog.h"
#include "InterfaceDialog.h"
#include "TimeDialog.h"
#include "TagServiceDialog.h"

#include "iptAdvancedDialog.h"
#include "ipfAdvancedDialog.h"
#include "ipfwAdvancedDialog.h"
#include "pfAdvancedDialog.h"
#include "pixAdvancedDialog.h"
#include "junosaclAdvancedDialog.h"
#include "nxosaclAdvancedDialog.h"
#include "iosaclAdvancedDialog.h"
#include "ipcopAdvancedDialog.h"
#include "secuwallAdvancedDialog.h"
#include "procurveaclAdvancedDialog.h"

#include "linux24IfaceOptsDialog.h"
#include "secuwallIfaceOptsDialog.h"
#include "vlanOnlyIfaceOptsDialog.h"
#include "bsdIfaceOptsDialog.h"
#include "pixosIfaceOptsDialog.h"

#include "clusterMembersDialog.h"

#include "linux24AdvancedDialog.h"
#include "linksysAdvancedDialog.h"
#include "freebsdAdvancedDialog.h"
#include "openbsdAdvancedDialog.h"
#include "solarisAdvancedDialog.h"
#include "macosxAdvancedDialog.h"
#include "pixosAdvancedDialog.h"
#include "junosAdvancedDialog.h"
#include "nxosAdvancedDialog.h"
#include "iosAdvancedDialog.h"
#include "ipcoposAdvancedDialog.h"
#include "secuwallosAdvancedDialog.h"

#include "RuleOptionsDialog.h"
#include "RoutingRuleOptionsDialog.h"
#include "NATRuleOptionsDialog.h"

#include "vrrpOptionsDialog.h"
#include "carpOptionsDialog.h"
#include "pixFailoverOptionsDialog.h"
#include "conntrackOptionsDialog.h"
#include "heartbeatOptionsDialog.h"
#include "openaisOptionsDialog.h"
#include "pfsyncOptionsDialog.h"

#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"

#include <iostream>
#include "ProjectPanel.h"

using namespace std;
using namespace libfwbuilder;

BaseObjectDialog *DialogFactory::createDialog(QWidget *parent, const QString &objType)
{
    if (objType==UserService::TYPENAME)   return new UserDialog(parent);

    if (objType==Policy::TYPENAME)        return new RuleSetDialog(parent);

    if (objType==NAT::TYPENAME)           return new RuleSetDialog(parent);

    if (objType==Routing::TYPENAME)       return new RuleSetDialog(parent);

    if (objType==Library::TYPENAME)       return new LibraryDialog(parent);

    if (objType==IPv4::TYPENAME)          return new IPv4Dialog(parent);

    if (objType==IPv6::TYPENAME)          return new IPv6Dialog(parent);

    if (objType==physAddress::TYPENAME)   return new PhysicalAddressDialog(parent);

    if (objType==DNSName::TYPENAME)       return new DNSNameDialog(parent);

    if (objType==AddressTable::TYPENAME)  return new AddressTableDialog(parent);

    if (objType==AddressRange::TYPENAME)  return new AddressRangeDialog(parent);

    if (objType==Firewall::TYPENAME)      return new FirewallDialog(parent);

    if (objType==Cluster::TYPENAME)       return new ClusterDialog(parent);

    if (objType==StateSyncClusterGroup::TYPENAME)  return new ClusterGroupDialog(parent);

    if (objType==FailoverClusterGroup::TYPENAME)  return new ClusterGroupDialog(parent);

    if (objType==Host::TYPENAME)          return new HostDialog(parent);

    if (objType==Interface::TYPENAME)     return new InterfaceDialog(parent);

    if (objType==AttachedNetworks::TYPENAME) return new AttachedNetworksDialog(parent);

    if (objType==Network::TYPENAME)       return new NetworkDialog(parent);

    if (objType==NetworkIPv6::TYPENAME)   return new NetworkDialogIPv6(parent);

    if (objType==CustomService::TYPENAME) return new CustomServiceDialog(parent);

    if (objType==IPService::TYPENAME)     return new IPServiceDialog(parent);

    if (objType==ICMPService::TYPENAME)   return new ICMPServiceDialog(parent);

    if (objType==ICMP6Service::TYPENAME)  return new ICMPServiceDialog(parent);

    if (objType==TCPService::TYPENAME)    return new TCPServiceDialog(parent);

    if (objType==UDPService::TYPENAME)    return new UDPServiceDialog(parent);

    if (objType==ObjectGroup::TYPENAME)   return new GroupObjectDialog(parent);

    if (objType==DynamicGroup::TYPENAME)   return new DynamicGroupDialog(parent);

    if (objType==ServiceGroup::TYPENAME)  return new GroupObjectDialog(parent);

    if (objType==TagService::TYPENAME) return new TagServiceDialog(parent);

    if (objType==IntervalGroup::TYPENAME) return new GroupObjectDialog(parent);

    if (objType==Interval::TYPENAME)      return new TimeDialog(parent);

    if (objType==RoutingRule::TYPENAME)   return new RoutingRuleOptionsDialog(parent);
    if (objType==Rule::TYPENAME)          return new RuleOptionsDialog(parent);
    if (objType==PolicyRule::TYPENAME)    return new RuleOptionsDialog(parent);
    if (objType==NATRule::TYPENAME)       return new NATRuleOptionsDialog(parent);

    return nullptr;
}


QWidget *DialogFactory::createFWDialog(QWidget *parent, FWObject *o)
{
    string platform = o->getStr("platform");
    string host_os = o->getStr("host_OS");

    Resources* platform_res = Resources::platform_res[platform];
    if (platform_res==nullptr)
        throw FWException(
            (const char*)(QObject::tr("Support module for %1 is not available").
                          arg(platform.c_str()).toLocal8Bit().constData()));

    Resources* os_res = Resources::os_res[host_os];
    if (os_res==nullptr)
        throw FWException(
            (const char*)(QObject::tr("Support module for %1 is not available").
                          arg(host_os.c_str()).toLocal8Bit().constData()));

    string os_family = os_res->getResourceStr(
        "/FWBuilderResources/Target/family");

    string dlgname = platform_res->Resources::getResourceStr(
        "/FWBuilderResources/Target/dialog");
    if (platform == "iptables" && os_family == "ipcop")
        dlgname = "ipcop";
    if (platform == "iptables" && os_family == "secuwall")
        dlgname = "secuwall";

    if (dlgname=="junosacl")   return new junosaclAdvancedDialog(parent,o);
    if (dlgname=="nxosacl")   return new nxosaclAdvancedDialog(parent,o);
    if (dlgname=="iosacl")   return new iosaclAdvancedDialog(parent,o);
    if (dlgname=="ipcop")    return new ipcopAdvancedDialog(parent,o);
    if (dlgname=="ipf")      return new ipfAdvancedDialog(parent,o);
    if (dlgname=="ipfw")     return new ipfwAdvancedDialog(parent,o);
    if (dlgname=="iptables") return new iptAdvancedDialog(parent,o);
    if (dlgname=="pf")       return new pfAdvancedDialog(parent,o);
    if (dlgname=="pix")      return new pixAdvancedDialog(parent,o);
    if (dlgname=="secuwall") return new secuwallAdvancedDialog(parent,o);
    if (dlgname=="procurveacl") return new procurveaclAdvancedDialog(parent,o);

    cerr << "Firewall settings dialog for " << dlgname
         << " is not implemented" << endl;
    return nullptr;
}


QWidget *DialogFactory::createOSDialog(QWidget *parent,FWObject *o)
{
    string host_os = o->getStr("host_OS");

    Resources *os = Resources::os_res[host_os];
    if (os==nullptr)
        throw FWException(
            (const char*)(QObject::tr("Support module for %1 is not available").
                          arg(host_os.c_str()).toLocal8Bit().constData()));

    string dlgname = os->Resources::getResourceStr(
        "/FWBuilderResources/Target/dialog");

//    string os=o->getStr("host_OS");
    if (dlgname=="linux24")   return new linux24AdvancedDialog(parent, o);
    if (dlgname=="linux317")  return new linux24AdvancedDialog(parent, o);
    if (dlgname=="sveasoft")  return new linksysAdvancedDialog(parent, o);
    if (dlgname=="freebsd")   return new freebsdAdvancedDialog(parent, o);
    if (dlgname=="openbsd")   return new openbsdAdvancedDialog(parent, o);
    if (dlgname=="solaris")   return new solarisAdvancedDialog(parent, o);
    if (dlgname=="macosx")    return new macosxAdvancedDialog(parent, o);
    if (dlgname=="pix_os")    return new pixosAdvancedDialog(parent, o);
    if (dlgname=="junos")       return new junosAdvancedDialog(parent, o);
    if (dlgname=="nxos")       return new nxosAdvancedDialog(parent, o);
    if (dlgname=="ios")       return new iosAdvancedDialog(parent, o);
    if (dlgname=="ipcop")     return new ipcoposAdvancedDialog(parent, o);
    if (dlgname=="secuwall")  return new secuwallosAdvancedDialog(parent, o);

    cerr << "OS settings dialog for " << dlgname
         << " is not implemented" << endl;

    return nullptr;
}

QWidget *DialogFactory::createIfaceDialog(QWidget *parent,FWObject *o)
{
    FWObject *h = Host::getParentHost(o);
    //FWObject *h = Interface::cast(o)->getParentHost();

    string host_OS = h->getStr("host_OS");
    Resources *os = Resources::os_res[host_OS];
    if (os==nullptr)
        throw FWException((const char*)(
                              QObject::tr("Support module for %1 is not available").
                              arg(host_OS.c_str()).toLocal8Bit().constData()));

    string dlgname = os->Resources::getResourceStr(
        "/FWBuilderResources/Target/interface_dialog");

    // add further dlgname support here ...

    if (dlgname=="secuwall")  return new secuwallIfaceOptsDialog(parent, o);
    if (dlgname=="linux24")  return new linux24IfaceOptsDialog(parent, o);
    if (dlgname=="linux317")  return new linux24IfaceOptsDialog(parent, o);
    if (dlgname=="bsd")  return new bsdIfaceOptsDialog(parent, o);
    if (dlgname=="pix_os")  return new pixosIfaceOptsDialog(parent, o);
    if (dlgname=="vlan_only")  return new vlanOnlyIfaceOptsDialog(parent, o);

    cerr << "Interface settings dialog for OS " << host_OS
         << " is not implemented" << endl;

    return nullptr;
}

QWidget *DialogFactory::createClusterConfDialog(QWidget *parent, FWObject *o)
{
    FWObject *objparent = o->getParent();
    while (objparent && objparent->getTypeName()!="Cluster")
        objparent = objparent->getParent();
    assert(objparent);

    string host_OS = objparent->getStr("host_OS");
    Resources *os = Resources::os_res[host_OS];
    string dlgname = os->Resources::getResourceStr("/FWBuilderResources/Target/cluster_dialog");

    // add further dlgname support here ...

    if (dlgname == "basic")  return new clusterMembersDialog(parent, o);

    cerr << "Cluster configuration dialog for OS " << host_OS
         << " is not implemented" << endl;

    return nullptr;
}

/*
 *  Create cluster group options dialog; dialog class depends on the
 *  cluster group type. Argument <o> is FWOptions object which is
 *  a child of ClusterGroup object
 */
QString DialogFactory::getClusterGroupOptionsDialogName(FWObject *o)
{
    FWObject *cluster_group = o->getParent();
    assert(ClusterGroup::cast(cluster_group)!=nullptr);

    string type = ClusterGroup::cast(cluster_group)->getStr("type");

    FWObject *fw = o;
    while (fw && Firewall::cast(fw)==nullptr) fw = fw->getParent();
    if (fw)
    {
        string host_OS = fw->getStr("host_OS");
        Resources *os = Resources::os_res[host_OS];
        return os->Resources::getResourceStr(
            "/FWBuilderResources/Target/protocols/" + type + "/dialog").c_str();
    }
    return QString();
}

QWidget *DialogFactory::createClusterGroupOptionsDialog(
    QWidget *parent, FWObject *o)
{
    QString dlgname = getClusterGroupOptionsDialogName(o);

    if (dlgname == "conntrack")  return new conntrackOptionsDialog(parent, o);
    if (dlgname == "pfsync")  return new pfsyncOptionsDialog(parent, o);
    if (dlgname == "pix_state_sync")  return new pixFailoverOptionsDialog(parent, o);

    if (dlgname == "vrrp")  return new vrrpOptionsDialog(parent, o);
    if (dlgname == "carp")  return new carpOptionsDialog(parent, o);
    if (dlgname == "heartbeat")  return new heartbeatOptionsDialog(parent, o);
    if (dlgname == "openais")  return new openaisOptionsDialog(parent, o);

    // Add more cluster group options dialog here
    return nullptr;
}

string DialogFactory::getActionDialogPageName(Firewall *fw, Rule *rule)
{
    string platform = fw->getStr("platform");
    string act = getRuleAction(rule).toStdString();
    string editor_page;
    try  
    {
        editor_page = Resources::getTargetCapabilityStr(
            platform, "actions/" + act + "/dialog_page");
    } catch (FWException &ex) { }
    return editor_page;
}


