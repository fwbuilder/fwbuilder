/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: DialogFactory.cpp,v 1.15 2007/05/09 04:18:18 vkurland Exp $

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



#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"

#include "DialogFactory.h"

#include "LibraryDialog.h"
#include "AddressRangeDialog.h"
#include "IPv4Dialog.h"
#include "IPv6Dialog.h"
#include "PhysicalAddressDialog.h"
#include "DNSNameDialog.h"
#include "AddressTableDialog.h"
#include "NetworkDialog.h"
#include "NetworkDialogIPv6.h"
#include "CustomServiceDialog.h"
#include "ICMPServiceDialog.h"
#include "IPServiceDialog.h"
#include "TCPServiceDialog.h"
#include "UDPServiceDialog.h"
#include "GroupObjectDialog.h"
#include "HostDialog.h"
#include "FirewallDialog.h"
#include "InterfaceDialog.h"
#include "TimeDialog.h"
#include "TagServiceDialog.h"

#include "iptAdvancedDialog.h"
#include "ipfAdvancedDialog.h"
#include "ipfwAdvancedDialog.h"
#include "pfAdvancedDialog.h"
#include "pixAdvancedDialog.h"
#include "iosaclAdvancedDialog.h"

#include "linux24AdvancedDialog.h"
#include "linksysAdvancedDialog.h"
#include "freebsdAdvancedDialog.h"
#include "openbsdAdvancedDialog.h"
#include "solarisAdvancedDialog.h"
#include "macosxAdvancedDialog.h"
#include "pixosAdvancedDialog.h"
#include "iosAdvancedDialog.h"

#include "RuleOptionsDialog.h"
#include "RoutingRuleOptionsDialog.h"
#include "NATRuleOptionsDialog.h"


#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/TagService.h"

#include <iostream>
#include "ProjectPanel.h"

using namespace std;
using namespace libfwbuilder;

QWidget *DialogFactory::createDialog(ProjectPanel *project, QWidget *parent,const QString &objType)
{

    if (objType==Library::TYPENAME)       return new LibraryDialog(project, parent);

    if (objType==IPv4::TYPENAME)          return new IPv4Dialog(project, parent);

    if (objType==IPv6::TYPENAME)          return new IPv6Dialog(project, parent);

    if (objType==physAddress::TYPENAME)   return new PhysicalAddressDialog(project, parent);

    if (objType==DNSName::TYPENAME)       return new DNSNameDialog(project, parent);

    if (objType==AddressTable::TYPENAME)  return new AddressTableDialog(project, parent);

    if (objType==AddressRange::TYPENAME)  return new AddressRangeDialog(project, parent);

    if (objType==Firewall::TYPENAME)      return new FirewallDialog(project, parent);

    if (objType==Host::TYPENAME)          return new HostDialog(project, parent);

    if (objType==Interface::TYPENAME)     return new InterfaceDialog(project, parent);

    if (objType==Network::TYPENAME)       return new NetworkDialog(project, parent);

    if (objType==NetworkIPv6::TYPENAME)   return new NetworkDialogIPv6(project, parent);

    if (objType==CustomService::TYPENAME) return new CustomServiceDialog(project, parent);

    if (objType==IPService::TYPENAME)     return new IPServiceDialog(project, parent);

    if (objType==ICMPService::TYPENAME)   return new ICMPServiceDialog(project, parent);

    if (objType==TCPService::TYPENAME)    return new TCPServiceDialog(project, parent);

    if (objType==UDPService::TYPENAME)    return new UDPServiceDialog(project, parent);

    if (objType==ObjectGroup::TYPENAME)   return new GroupObjectDialog(project, parent);

    if (objType==ServiceGroup::TYPENAME)  return new GroupObjectDialog(project, parent);

    if (objType==TagService::TYPENAME)          return new TagServiceDialog(project, parent);

    if (objType==IntervalGroup::TYPENAME) return new GroupObjectDialog(project, parent);

    if (objType==Interval::TYPENAME)      return new TimeDialog(project, parent);

    if (objType==RoutingRule::TYPENAME)   return new RoutingRuleOptionsDialog(project, parent);
    if (objType==Rule::TYPENAME)          return new RuleOptionsDialog(project, parent);
    if (objType==PolicyRule::TYPENAME)    return new RuleOptionsDialog(project, parent);
    if (objType==NATRule::TYPENAME)       return new NATRuleOptionsDialog(project, parent);

    return NULL;
}


QWidget *DialogFactory::createFWDialog(QWidget *parent,FWObject *o)
    throw(FWException)
{
    Resources* platform = Resources::platform_res[o->getStr("platform")];
    if (platform==NULL)
        throw FWException((const char*)(QObject::tr("Support module for %1 is not available").arg(o->getStr("platform").c_str()).toLocal8Bit().constData()));

    string dlgname=platform->Resources::getResourceStr("/FWBuilderResources/Target/dialog");

//    string pl=o->getStr("platform");
    if (dlgname=="iptables") return new iptAdvancedDialog(parent,o);
    if (dlgname=="ipf")      return new ipfAdvancedDialog(parent,o);
    if (dlgname=="ipfw")     return new ipfwAdvancedDialog(parent,o);
    if (dlgname=="pf")       return new pfAdvancedDialog(parent,o);
    if (dlgname=="pix")      return new pixAdvancedDialog(parent,o);
    if (dlgname=="iosacl")   return new iosaclAdvancedDialog(parent,o);

    cerr << "Firewall settings dialog for " << dlgname
         << " is not implemented" << endl;
    return NULL;
}


QWidget *DialogFactory::createOSDialog(QWidget *parent,FWObject *o)
    throw(FWException)
{
    Resources *os = Resources::os_res[o->getStr("host_OS")];
    if (os==NULL)
        throw FWException((const char*)(QObject::tr("Support module for %1 is not available").arg(o->getStr("host_OS").c_str()).toLocal8Bit().constData()));

    string dlgname=os->Resources::getResourceStr("/FWBuilderResources/Target/dialog");

//    string os=o->getStr("host_OS");
    if (dlgname=="linux24")   return new linux24AdvancedDialog(parent,o);
    if (dlgname=="linksys")   return new linksysAdvancedDialog(parent,o);
    if (dlgname=="freebsd")   return new freebsdAdvancedDialog(parent,o);
    if (dlgname=="openbsd")   return new openbsdAdvancedDialog(parent,o);
    if (dlgname=="solaris")   return new solarisAdvancedDialog(parent,o);
    if (dlgname=="macosx")    return new macosxAdvancedDialog(parent,o);
    if (dlgname=="pix_os")    return new pixosAdvancedDialog(parent,o);
    if (dlgname=="ios")       return new iosAdvancedDialog(parent,o);

    cerr << "OS settings dialog for " << dlgname
         << " is not implemented" << endl;

    return NULL;
}


