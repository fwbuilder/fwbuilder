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

#include "definitions.h"
#include "global.h"
#include "utils_no_qt.h"
#include "utils.h"

#include <qobject.h>
#include <qtextstream.h>
#include <qdatetime.h>
#include <qregexp.h>
#include <QtDebug>

#include "FWObjectPropertiesFactory.h"
#include "platforms.h"
#include "DialogFactory.h"
#include "FWBTree.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Cluster.h"

#include <sstream>
#include <iostream>
#include <time.h>


using namespace std;
using namespace libfwbuilder;

/*
 * This method returns brief summary of properties, guaranteed to be 1
 * line of text
 */
QString FWObjectPropertiesFactory::getObjectPropertiesBrief(FWObject *obj)
{
    QString res;
    QTextStream str(&res, QIODevice::WriteOnly);
    FWObject *parent_obj = obj->getParent();

    try
    {
        if (Library::isA(obj))
        {
            if (obj->isReadOnly()) str << "(read only)";

        } else if (IPv4::isA(obj))
        {
            str <<  IPv4::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str << "/";
                str << IPv4::cast(obj)->getNetmaskPtr()->toString().c_str();
            }
        } else if (IPv6::isA(obj))
        {
            str <<  IPv6::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str << "/";
                str << QString("%1").arg(IPv6::cast(obj)->getNetmaskPtr()->getLength());
            }
        } else if (physAddress::isA(obj))
        {
            str <<  physAddress::cast(obj)->getPhysAddress().c_str();
        } else if (DNSName::isA(obj))
        {
            str << DNSName::cast(obj)->getSourceName().c_str();
        } else if (AddressTable::isA(obj))  
        {
            str << AddressTable::cast(obj)->getSourceName().c_str();
        } else if (AddressRange::isA(obj))
        {
            AddressRange *ar=AddressRange::cast(obj);
            str << ar->getRangeStart().toString().c_str();
            str << " - ";
            str << ar->getRangeEnd().toString().c_str();
        } else if (Firewall::cast(obj))
        {
            if (Firewall::cast(obj)->needsCompile()) str << " * ";
            QString platform = obj->getStr("platform").c_str();
            QString version  = obj->getStr("version").c_str();
            QString readableVersion = getVersionString(platform,version);
            QString hostOS = obj->getStr("host_OS").c_str();
            str <<  platform << "(" << readableVersion << ") / " << hostOS;

        } else if (Host::isA(obj))
        {
            const InetAddr *addr = Address::cast(obj)->getAddressPtr();
            if (addr)
                str <<  addr->toString().c_str();
            else
                str << "(no ip address)";
        } else if (Network::isA(obj))
        {
            Network *n=Network::cast(obj);
            str << n->getAddressPtr()->toString().c_str();
            str << "/";
            str << n->getNetmaskPtr()->toString().c_str();

        } else if (NetworkIPv6::isA(obj))
        {
            NetworkIPv6 *n=NetworkIPv6::cast(obj);
            str << n->getAddressPtr()->toString().c_str();
            str << "/";
            str << QString("%1").arg(n->getNetmaskPtr()->getLength());

        } else if (ClusterGroup::cast(obj)!=nullptr)
        {
            ClusterGroup *g = ClusterGroup::cast(obj);
            str << QObject::tr("type: ") << g->getStr("type").c_str();
        } else if (Group::cast(obj)!=nullptr)   // just any group
        {
            Group *g=Group::cast(obj);
            str << g->size() << " " << QObject::tr(" objects");

        } else if (Interface::isA(obj))
        {
            Interface *intf = Interface::cast(obj);
            // trigger late initialization of options object
            // if its read-only or part of the read-only tree, I can't help it.
            if (!obj->isReadOnly()) intf->getOptionsObject();
            str << intf->getLabel().c_str() << " ";

            QStringList q;

            FWObject *parent = Host::getParentHost(intf);
            //FWObject *parent = intf->getParentHost();
            if (parent)
            {
                bool supports_security_levels = false;
                bool supports_network_zones   = false;
                try
                {
                    supports_security_levels =
                        (!parent->getStr("platform").empty() &&
                         Resources::getTargetCapabilityBool(
                             parent->getStr("platform"), "security_levels"));
                    supports_network_zones =
                        (!parent->getStr("platform").empty() &&
                         Resources::getTargetCapabilityBool(
                             parent->getStr("platform"), "network_zones"));
                } catch (FWException &ex)  { }

                if (supports_security_levels)
                {
                    QString str;
                    str.setNum(intf->getSecurityLevel());
                    q.push_back(QString("sec level: %1").arg(str));
                }
                if (supports_network_zones)
                {
                    int id = FWObjectDatabase::getIntId(intf->getStr("network_zone"));
                    if (id > 0)
                    {
                        FWObject *nz_obj = obj->getRoot()->findInIndex(id);
                        if (nz_obj)
                            q.push_back(
                                QString("network zone: %1")
                                .arg(nz_obj->getName().c_str()));
                        else
                            q.push_back(QString("network zone: not configured"));

                    }
                }
            }

            if (intf->isDyn())         q.push_back("dyn");
            if (intf->isUnnumbered())  q.push_back("unnum");
            if (intf->isDedicatedFailover())  q.push_back("failover");
            if (intf->isBridgePort())  q.push_back("bridge port");
            if (intf->isSlave())       q.push_back("slave");
            if (intf->isUnprotected()) q.push_back("unp");
            if (!q.empty()) str << q.join(",");

        } else if (IPService::isA(obj))
        {
            str << QObject::tr("protocol: %1").arg(obj->getStr("protocol_num").c_str());

        } else if (ICMPService::isA(obj) || ICMP6Service::isA(obj))
        {
            str << QObject::tr("type: %1").arg(obj->getStr("type").c_str())
                << "  "
                << QObject::tr("code: %1").arg(obj->getStr("code").c_str());

        } else if (TCPService::isA(obj) || UDPService::isA(obj))
        {
            int sps,spe,dps,dpe;

            sps=TCPUDPService::cast(obj)->getSrcRangeStart();
            spe=TCPUDPService::cast(obj)->getSrcRangeEnd();
            dps=TCPUDPService::cast(obj)->getDstRangeStart();
            dpe=TCPUDPService::cast(obj)->getDstRangeEnd();

            str << sps << ":" << spe << " / ";
            str << dps << ":" << dpe;
        } else if (TagService::isA(obj)) 
        {
            str << "Pattern: \"" << obj->getStr("tagcode").c_str() << "\"" ;
        } else if (UserService::isA(obj)) 
        {
            const UserService* user_srv = UserService::constcast(obj);
            str << "User id: \"" << user_srv->getUserId().c_str() << "\"" ;
        } else if (RuleSet::cast(obj) != nullptr)
        {
            QStringList attrs;
            RuleSet *rs = RuleSet::cast(obj);
            if (rs->isTop()) attrs.push_back("top ruleset");
            if (rs->isDual()) attrs.push_back("ipv4/ipv6");
            else
            {
                if (rs->isV4()) attrs.push_back("ipv4");
                if (rs->isV6()) attrs.push_back("ipv6");
            }
            attrs.push_back(QString("%1 rules").arg(rs->getRuleSetSize()));
            str << attrs.join(" ");
        } else if (Interval::isA(obj))
        {

        }
    } catch (FWException &ex)
    {
        cerr << ex.toString() << endl;
    }

    return QString::fromUtf8(res.toStdString().c_str());
}

/*
 * More detailed list of properties, still one line, no fancy
 * formatting and no HTML. This is used in object group list view and
 * for printing.
 */
QString FWObjectPropertiesFactory::getObjectProperties(FWObject *obj)
{
    QString res;
    QTextStream str(&res, QIODevice::WriteOnly);
    FWObject *parent_obj = obj->getParent();

    try
    {
        if (IPv4::isA(obj))
        {
            str <<  IPv4::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str << "/";
                str << IPv4::cast(obj)->getNetmaskPtr()->toString().c_str();
            }
        } else if (IPv6::isA(obj))
        {
            str <<  IPv6::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str << "/";
                str << QString("%1").arg(IPv6::cast(obj)->getNetmaskPtr()->getLength());
            }
        } else if (physAddress::isA(obj))
        {
            str <<  physAddress::cast(obj)->getPhysAddress().c_str();
        } else if (DNSName::isA(obj))
        {
            str << QObject::tr("DNS record: ")
                << DNSName::cast(obj)->getSourceName().c_str();
        } else if (AddressTable::isA(obj))  
        {
            str << QObject::tr("Address Table: ")
                << AddressTable::cast(obj)->getSourceName().c_str();
        } else if (AddressRange::isA(obj))
        {
            AddressRange *ar=AddressRange::cast(obj);
            str << ar->getRangeStart().toString().c_str();
            str << " - ";
            str << ar->getRangeEnd().toString().c_str();
        } else if (Firewall::cast(obj))
        {
            QString platform = obj->getStr("platform").c_str();
            QString version  = obj->getStr("version").c_str();
            QString readableVersion = getVersionString(platform,version);
            QString hostOS = obj->getStr("host_OS").c_str();

            QDateTime dt;
            time_t t;
            
            t = obj->getInt("lastModified");dt.setTime_t(t);
            QString t_modified  = (t)? dt.toString():"-";
            
            t = obj->getInt("lastCompiled");dt.setTime_t(t);
            QString t_compiled  = (t)? dt.toString():"-";
            
            t = obj->getInt("lastInstalled");dt.setTime_t(t);
            QString t_installed = (t)? dt.toString():"-";
            
            str <<  platform << "(" << readableVersion << ") / " << hostOS;

        } else if (Host::isA(obj))
        {
            const InetAddr *addr = Address::cast(obj)->getAddressPtr();
            if (addr)
                str <<  addr->toString().c_str();
            else
                str << "(no ip address)";

            FWObject *co=obj->getFirstByType("Interface");
            if (co!=nullptr) 
            {
                physAddress *paddr=(Interface::cast(co))->getPhysicalAddress();
                if (paddr!=nullptr) 
                    str << "    " <<  paddr->getPhysAddress().c_str();
            }

        } else if (Network::isA(obj))
        {
            Network *n=Network::cast(obj);
            str << QString("%1/%2")
                .arg(n->getAddressPtr()->toString().c_str())
                .arg(n->getNetmaskPtr()->toString().c_str());

        } else if (NetworkIPv6::isA(obj))
        {
            NetworkIPv6 *n=NetworkIPv6::cast(obj);
            str << QString("%1/%2")
                .arg(n->getAddressPtr()->toString().c_str())
                .arg(n->getNetmaskPtr()->getLength());

        } else if (ClusterGroup::cast(obj)!=nullptr)
        {
            ClusterGroup *g = ClusterGroup::cast(obj);
            str << QObject::tr("Type: ") << g->getStr("type").c_str() << " ";
            QStringList members;
            FWObjectTypedChildIterator j = obj->findByType(FWObjectReference::TYPENAME);
            for ( ; j!=j.end(); ++j)
            {
                FWObject *obj = FWReference::getObject(*j);
                if (Interface::cast(obj))
                {
                    FWObject *fw = obj->getParent();
                    members.push_back(
                        QString("%1:%2")
                        .arg(fw->getName().c_str()).arg(obj->getName().c_str()));
                }
            }
            if (members.size() != 0)
            {
                members.push_front(QObject::tr("Members:"));
                str << members.join(" ");
            }
        } else if (DynamicGroup::cast(obj) != nullptr) {
            DynamicGroup *objGroup = DynamicGroup::cast(obj);
            str << QObject::tr("%1 filters").arg(objGroup->getFilter().size());
        } else if (Group::cast(obj)!=nullptr)   // just any group
        {
            Group *g=Group::cast(obj);
            str << QObject::tr("%1 objects").arg(g->size());
        } else if (Firewall::cast(obj))
        {

        } else if (Interface::isA(obj))
        {
            Interface *intf = Interface::cast(obj);
            QString label = QString::fromUtf8(intf->getLabel().c_str());
            if (label != "")
                str << QObject::tr("Label: %1").arg(label) << " ";

            QString intf_type = intf->getOptionsObject()->getStr("type").c_str();
            if (intf_type != "" && intf_type.toLower() != "ethernet")
                str << QObject::tr("Type: ") << intf_type << " ";

            QStringList addr;
            FWObjectTypedChildIterator j = obj->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j)
            {
                addr << getObjectProperties(*j);
            }
            if (addr.size() != 0)
            {
                if (addr.size() > 1)
                    addr.push_front(QObject::tr("Addresses:"));
                else
                    addr.push_front(QObject::tr("Address:"));
                str << addr.join(" ");
            }
        } else if (IPService::isA(obj))
        {
            str << QObject::tr("protocol: %1").arg(obj->getStr("protocol_num").c_str());

        } else if (ICMPService::isA(obj) || ICMP6Service::isA(obj))
        {
            str << QObject::tr("type: %1").arg(obj->getStr("type").c_str())
                << "  "
                << QObject::tr("code: %1").arg(obj->getStr("code").c_str());

        } else if (TCPService::isA(obj) || UDPService::isA(obj))
        {
            int sps,spe,dps,dpe;

            sps=TCPUDPService::cast(obj)->getSrcRangeStart();
            spe=TCPUDPService::cast(obj)->getSrcRangeEnd();
            dps=TCPUDPService::cast(obj)->getDstRangeStart();
            dpe=TCPUDPService::cast(obj)->getDstRangeEnd();

            str << sps << ":" << spe << " / ";
            str << dps << ":" << dpe;
        } else if (TagService::isA(obj)) 
        {
            str << QObject::tr("Pattern: \"%1\"").arg(obj->getStr("tagcode").c_str());
        } else if (UserService::isA(obj)) 
        {
            const UserService* user_srv = UserService::constcast(obj);
            str << QObject::tr("User id: \"%1\"").arg(user_srv->getUserId().c_str());
        } else if (Interval::isA(obj))
        {

        }
    } catch (FWException &ex)
    {
        cerr << ex.toString() << endl;
    }

    return res;
}


QString FWObjectPropertiesFactory::stripHTML(const QString &str)
{
    // note that str may contain multiple lines
    // separated by <br> and/or '\n'

    QRegExp htmltag1 = QRegExp("<[^>]+>");
    QRegExp htmltag2 = QRegExp("</[^>]+>");
    QRegExp htmltd   = QRegExp("</td><td>");

    QString res = str;
    res = res.replace(htmltd,": ");
    res = res.remove(htmltag1);
    res = res.remove(htmltag2);
    return res;
}

/*
 * Nicely formatted list of properties, HTML. This one is used for the
 * tree tooltips.
 */
QString FWObjectPropertiesFactory::getObjectPropertiesDetailed(FWObject *obj,
                                                               bool showPath,
                                                               bool tooltip,
                                                               bool accentName,
                                                               bool richText)
{
    QString str;
    FWObject *parent_obj = obj->getParent();

    QString path = obj->getPath().c_str();
    path = path.section('/',2,-1);

    if (obj->getId() == FWObjectDatabase::ANY_ADDRESS_ID ||
        obj->getId() == FWObjectDatabase::ANY_SERVICE_ID ||
        obj->getId() == FWObjectDatabase::ANY_INTERVAL_ID)
    {
        return "to modify this field drag and \ndrop an object from the tree here";
    } 

    if (showPath)
    {
        str += QObject::tr("<b>Library:</b> ");
        str += QString::fromUtf8(obj->getLibrary()->getName().c_str()) + "<br>\n";

        if (!tooltip)
        {
            str += QObject::tr("<b>Object Id:</b> ");
            str += QString(FWObjectDatabase::getStringId(obj->getId()).c_str()) + "<br>\n";
        }
    }

    if (FWBTree().isSystem(obj))
    {
        QString object_path = obj->getPath(true).c_str();

        if (object_path == "Objects")
            return QObject::tr("This system folder holds objects that represent <b>IPv4</b> and <b>IPv6</b> addresses and networks");

        if (object_path == "Objects/Addresses")
            return QObject::tr("This system folder holds objects that represent <b>IPv4</b> and <b>IPv6</b> addresses");

        if (object_path == "Objects/DNS Names")
            return QObject::tr("This system folder holds objects that represent <b>DNS A</b> records");

        if (object_path == "Objects/Address Tables")
            return QObject::tr("This system folder holds objects that read <b>IP addresses</b> from external files");

        if (object_path == "Objects/Address Ranges")
            return QObject::tr("This system folder holds objects that represent <b>IPv4</b> and <b>IPv6</b> address ranges");

        if (object_path == "Objects/Groups")
            return QObject::tr("This system folder holds objects that represent groups of <b>IPv4</b> and <b>IPv6</b> addresses, networks and other groups");

        if (object_path == "Objects/Hosts")
            return QObject::tr("This system folder holds objects that represent <b>hosts</b> or <b>servers</b> that have one or more interfaces");

        if (object_path == "Objects/Networks")
            return QObject::tr("This system folder holds objects that represent <b>IPv4</b> and <b>IPv6</b> networks");

        if (object_path == "Services")
            return QObject::tr("This system folder holds objects that represent <b>IP, ICMP, TCP and UDP</b> services");

        if (object_path == "Services/Groups")
            return QObject::tr("This system folder holds objects that represent groups of <b>IP, ICMP, TCP and UDP</b> services");

        if (object_path == "Services/Custom")
            return QObject::tr("This system folder holds objects that represent <b>custom (user-defined)</b> services");

        if (object_path == "Services/IP")
            return QObject::tr("This system folder holds objects that represent <b>IP</b> services");

        if (object_path == "Services/ICMP")
            return QObject::tr("This system folder holds objects that represent <b>ICMP</b> and <b>ICMPv6</b> services");
        
        if (object_path == "Services/TCP")
            return QObject::tr("This system folder holds objects that represent <b>TCP</b> services");

        if (object_path == "Services/UDP")
            return QObject::tr("This system folder holds objects that represent <b>UDP</b> services");

        if (object_path == "Services/Users")
            return QObject::tr("This system folder holds objects that represent <b>user names</b>");

        if (object_path == "Services/TagServices")
            return QObject::tr("This system folder holds objects that represent <b>tags</b>");

        if (object_path == "Firewalls")
            return QObject::tr("This system folder holds objects that represent <b>firewalls</b>");

        if (object_path ==  "Clusters")
            return QObject::tr("This system folder holds objects that represent firewall <b>clusters</b>");

        if (object_path ==  "Time")
            return QObject::tr("This system folder holds objects that represent <b>time intervals</b>");

    }


    if (Library::isA(obj))
    {
        switch (obj->getId())
        {
        case FWObjectDatabase::STANDARD_LIB_ID:
            return QObject::tr("<html>A library of predefined read-only address and service objects that come with the program</html>");
            ;

        case FWObjectDatabase::DELETED_OBJECTS_ID:
            return QObject::tr("<html>This library holds objects that have been deleted. You can undelete them by clicking right mouse button and using menu item 'Move to ...' to move them back to another library</html>");
            ;

        default:
            return QObject::tr("<html>A library of user-defined objects; this is where you create your objects</html>");
        }

    }

    str += QObject::tr("<b>Object Type:</b> ");
    string d = Resources::global_res->getObjResourceStr(obj,"description");
    str += QString(d.c_str()) + "<br>\n";

    str += QObject::tr("<b>Object Name:</b> ");
    if (accentName) str += "<font color=\"red\">";
    str += QString::fromUtf8(obj->getName().c_str());
    if (accentName) str += "</font>";
    str += "<br>\n";

    try
    {
        if (IPv4::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str +=  IPv4::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str += "/";
                str += IPv4::cast(obj)->getNetmaskPtr()->toString().c_str();
            }
        } else if (IPv6::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str +=  IPv6::cast(obj)->getAddressPtr()->toString().c_str();
            if (parent_obj && Interface::isA(parent_obj))
            {
                str += "/";
                str += QString("%1").arg(IPv6::cast(obj)->getNetmaskPtr()->getLength());
            }
        } else if (physAddress::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += physAddress::cast(obj)->getPhysAddress().c_str();
        } else if (DNSName::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += QObject::tr("<b>DNS record:</b>");
            str += MultiAddress::cast(obj)->getSourceName().c_str();
            str += "<br>\n";
            str += (MultiAddress::cast(obj)->isRunTime())?QObject::tr("Run-time"):QObject::tr("Compile-time");

        } else if (AddressTable::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += QObject::tr("<b>Table file:</b>");
            str += MultiAddress::cast(obj)->getSourceName().c_str();
            str += "<br>\n";
            str += (MultiAddress::cast(obj)->isRunTime())?QObject::tr("Run-time"):QObject::tr("Compile-time");
            
        } else if (AddressRange::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            AddressRange *ar=AddressRange::cast(obj);
            str += ar->getRangeStart().toString().c_str();
            str += " - ";
            str += ar->getRangeEnd().toString().c_str();
        } else if (Host::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";

            FWObjectTypedChildIterator j = obj->findByType(
                Interface::TYPENAME);
            for ( ; j!=j.end(); ++j)
            {
                str += (*j)->getName().c_str();
                str += ": ";
                str += getObjectProperties(*j);
                str += "<br>";
            }
        } else if (Network::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            Network *n=Network::cast(obj);
            str += n->getAddressPtr()->toString().c_str();
            str += "/";
            str += n->getNetmaskPtr()->toString().c_str();

        } else if (NetworkIPv6::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            NetworkIPv6 *n=NetworkIPv6::cast(obj);
            str += n->getAddressPtr()->toString().c_str();
            str += "/";
            str += QString("%1").arg(n->getNetmaskPtr()->getLength());
        } else if (ClusterGroup::cast(obj)!=nullptr)
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            ClusterGroup *g = ClusterGroup::cast(obj);
            str += QObject::tr("type: %1<br>").arg(g->getStr("type").c_str());
            FWObjectTypedChildIterator j = obj->findByType(FWObjectReference::TYPENAME);
            for ( ; j!=j.end(); ++j)
            {
                FWObject *obj = FWReference::getObject(*j);
                if (Interface::cast(obj))
                {
                    FWObject *fw = obj->getParent();
                    str += QObject::tr("Group member %1:%2<br>").
                        arg(fw->getName().c_str()).arg(obj->getName().c_str());
                }
            }
        } else if (DynamicGroup::cast(obj) != nullptr) {
            DynamicGroup *objGroup = DynamicGroup::cast(obj);
            str += QObject::tr("%1 filters<br>\n").arg(objGroup->getFilter().size());
        } else if (Group::cast(obj)!=nullptr)   // just any group
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            Group *g = Group::cast(obj);
            str += QObject::tr("%1 objects<br>\n").arg(g->size());
            int n = 0;
            list<FWObject*> ll = *g;
            ll.sort(FWObjectNameCmpPredicate());

            for (FWObject::iterator i=ll.begin(); i!=ll.end(); ++i,++n)
            {
                if (n>20)  // arbitrary number
                {
                    str += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.&nbsp;.&nbsp;.&nbsp;";
                    break;
                } else 
                {
                    FWObject *o1=*i;
                    if (FWReference::cast(o1)!=nullptr)
                        o1=FWReference::cast(o1)->getPointer();
                    str += QString(o1->getTypeName().c_str())
                        + "  <b>" + QString::fromUtf8(o1->getName().c_str()) + "</b><br>\n";
                }
            }
        } else if (Firewall::cast(obj)) 
        {
            // Note: Firewall::cast(obj) matched Firewall and Cluster
            QString platform = obj->getStr("platform").c_str();
            QString version  = obj->getStr("version").c_str();
            QString readableVersion = getVersionString(platform,version);
            QString hostOS = obj->getStr("host_OS").c_str();
            
            QDateTime dt;
            time_t lm=obj->getInt("lastModified");
            time_t lc=obj->getInt("lastCompiled");
            time_t li=obj->getInt("lastInstalled");
            
            dt.setTime_t(lm);
            QString t_modified  = (lm)? dt.toString():"-";
            if (lm>lc && lm>li) t_modified=QString("<b>")+t_modified+"</b>";
                    
            dt.setTime_t(lc);
            QString t_compiled  = (lc)? dt.toString():"-";
            if (lc>lm && lc>li) t_compiled=QString("<b>")+t_compiled+"</b>";
            
            dt.setTime_t(li);
            QString t_installed = (li)? dt.toString():"-";
            if (li>lc && li>lm) t_installed=QString("<b>")+t_installed+"</b>";
            
               

            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += "<table cellspacing=\"0\" cellpadding=\"0\">";
            str += QString("<tr><td>Platform:</td><td>") +
                platform + "</td></tr>\n";
            str += QString("<tr><td>Version:</td><td>")  +
                readableVersion + "</td></tr>\n";
            str += QString("<tr><td>Host OS:</td><td>")  +
                hostOS + "</td></tr>\n";
            
            str += QString("<tr><td>Modified:</td><td>")  +
                t_modified + "</td></tr>\n";
            str += QString("<tr><td>Compiled:</td><td>")  +
                t_compiled + "</td></tr>\n";
            str += QString("<tr><td>Installed:</td><td>")  +
                t_installed + "</td></tr>\n";
            
            str += "</table>";
        } else if (Interface::isA(obj))
        {
            FWObject *parent_host = obj;
            QStringList short_path;
            //short_path.push_front(QString::fromUtf8(obj->getName().c_str()));
            do
            {
                parent_host = parent_host->getParent();
                if (parent_host == nullptr) break;
                short_path.push_front(QString::fromUtf8(parent_host->getName().c_str()));
            }
            while (Host::cast(parent_host) == nullptr);

            str += QString("<b>Parent: </b>%1<br>\n").arg(short_path.join("/"));

            Interface *intf = Interface::cast(obj);
            str += "<b>Label: </b>";
            str += QString::fromUtf8(intf->getLabel().c_str());
            str += "<br>";

            FWObjectTypedChildIterator j = obj->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j)
            {
                str += getObjectProperties(*j);
                str += "<br>";
            }

            string intf_type = intf->getOptionsObject()->getStr("type");
            if (!intf_type.empty())
            {
                str += "<b>Interface Type: </b>";
                str += intf_type.c_str();
                if (intf_type == "8021q")
                {
                    int vlan_id = intf->getOptionsObject()->getInt("vlan_id");
                    str += QString(" VLAN ID=%1").arg(vlan_id);
                }
                str += "<br>";
            }

            physAddress *paddr = intf->getPhysicalAddress();
            if (paddr!=nullptr) 
            {
                str += "MAC: ";
                str +=  paddr->getPhysAddress().c_str() ;
                str += "<br>";
            }

            QString q;
            if (intf->isDyn())        q=" dyn";
            if (intf->isUnnumbered()) q=" unnum";
            if (intf->isBridgePort()) q=" bridge port";
            
            FWObject *p=obj;
            while (p!=nullptr && !Firewall::cast(p)) p=p->getParent();
            if (p!=nullptr && (p->getStr("platform")=="pix" || p->getStr("platform")=="fwsm"))
            {
                int sl = intf->getSecurityLevel();
                q=q+QString("sec.level %1").arg(sl);
            }

            if (intf->isUnprotected())    q=q+" unp";
            
            if (q!="") str += " (" + q + ")";
            str += "<br>\n";
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";

        } else if (CustomService::isA(obj))
        {

            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";

            CustomService *s = dynamic_cast<CustomService*>(obj);
            bool first=true;

            map<string,string> platforms = Resources::getPlatforms();
            for (map<string,string>::iterator i=platforms.begin(); i!=platforms.end(); i++)
            {
                string c=s->getCodeForPlatform( (*i).first );
                if ( c!="" )
                {
                    if (first)
                    {
                        str += "<table cellspacing=\"0\" cellpadding=\"0\">";
                        first=false;
                    }
                    str += QString("<tr><td>%1</td><td>%2</td></tr>\n")
                        .arg((*i).second.c_str()).arg(c.c_str());
                }
            }
            if (!first) str += "</table>";

        } else if (IPService::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += QObject::tr("protocol ") + obj->getStr("protocol_num").c_str();

        } else if (ICMPService::isA(obj) || ICMP6Service::isA(obj))
        {
            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += QObject::tr("type: ") + obj->getStr("type").c_str()
                + "  "
                + QObject::tr("code: ") + obj->getStr("code").c_str();

        } else if (TCPService::isA(obj) || UDPService::isA(obj))
        {
            int sps,spe,dps,dpe;

            sps=TCPUDPService::cast(obj)->getSrcRangeStart();
            spe=TCPUDPService::cast(obj)->getSrcRangeEnd();
            dps=TCPUDPService::cast(obj)->getDstRangeStart();
            dpe=TCPUDPService::cast(obj)->getDstRangeEnd();

            if (showPath && !tooltip) str += "<b>Path: </b>" + path + "<br>\n";
            str += "<table cellspacing=\"0\" cellpadding=\"0\">";
            str += QString("<tr><td>source port range</td><td>%1:%2</td></tr>\n")
                .arg(sps).arg(spe);
            str += QString("<tr><td>destination port range</td><td>%1:%2</td></tr>\n")
                .arg(dps).arg(dpe);
            str += "</table>";
        } else if (TagService::isA(obj)) 
        {
            str += QObject::tr("Pattern: \"%1\"").arg(obj->getStr("tagcode").c_str());
        } else if (UserService::isA(obj)) 
        {
            const UserService* user_srv = UserService::constcast(obj);
            str += QObject::tr("User id: \"%1\"").arg(user_srv->getUserId().c_str());
        } else if (Interval::isA(obj))
        {

        }
    } catch (FWException &ex)
    {
        cerr << ex.toString() << endl;
    }

    if (richText) return str;

    return FWObjectPropertiesFactory::stripHTML(str);
}

/*
 * Do not translate literals 'pipe', 'queue', 'divert' below, these refer
 * to actual ipfw parameters and should not be localized.
 */
QString FWObjectPropertiesFactory::getRuleActionProperties(Rule *rule)
{
    QString par = "";

    if (rule!=nullptr)
    {
        QString act = getRuleAction(rule);
        
        FWObject *o = rule;
        while (o!=nullptr && Firewall::cast(o)==nullptr) o=o->getParent();
        if (o==nullptr) return "";

        Firewall *f=Firewall::cast(o);
        string platform=f->getStr("platform");
        
        FWOptions *ropt = rule->getOptionsObject();
        string editor = DialogFactory::getActionDialogPageName(f, rule);

        if (editor == "None") return "";

        if (PolicyRule::isA(rule))
        {
            switch (PolicyRule::cast(rule)->getAction())
            {
            case PolicyRule::Reject:
                par = ropt->getStr("action_on_reject").c_str();
                break;
            case PolicyRule::Accounting :
                par = ropt->getStr("rule_name_accounting").c_str();
                break;
            case PolicyRule::Custom:
                par = ropt->getStr("custom_str").c_str();
                break;
            case PolicyRule::Branch:
            {
                FWObject *branch_ruleset = rule->getBranch();
                if (branch_ruleset)
                    par = branch_ruleset->getName().c_str();
                // ropt->getStr("branch_name").c_str();
                break;
            }
            case PolicyRule::Pipe :
                if (platform=="ipfw")
                {
                    par = QString("divert ") + 
                        ropt->getStr("ipfw_pipe_port_num").c_str();
                }
                break;

            default : {}
            }
        }

        if (NATRule::isA(rule))
        {
            switch (NATRule::cast(rule)->getAction())
            {
            case NATRule::Translate:
                break;

            case NATRule::Branch:
                FWObject *branch_ruleset = rule->getBranch();
                if (branch_ruleset)
                    par = branch_ruleset->getName().c_str();
                break;
            }
        }        
    }
    
    return par;
}

QString FWObjectPropertiesFactory::getRuleActionPropertiesRich(Rule *rule)
{
    FWObject *p=rule;
    while (p!=nullptr && !Firewall::cast(p)) p=p->getParent();
    if (p==nullptr)
    {
        qDebug() << "FWObjectPropertiesFactory::getRuleActionPropertiesRich(): "
                 << "Can not locate parent firewall for the rule:";
        rule->dump(false, true);
        return "";
    }

    string platform=p->getStr("platform"); 
    QString act = getActionNameForPlatform(Firewall::cast(p), rule);
    QString par = getRuleActionProperties(rule);
    QString res = QObject::tr("<b>Action   :</b> %1<br>").arg(act);
    if (!par.isEmpty())
    {
        res += QObject::tr("<b>Parameter:</b> %1").arg(par);
    }
    return res;
}

QString FWObjectPropertiesFactory::getPolicyRuleOptions(Rule *rule)
{
    if (rule == nullptr) return "";

    QList<QPair<QString,QString> > options;

    PolicyRule *prule = PolicyRule::cast(rule);
    FWObject *o = rule;
    while (o!=nullptr && Firewall::cast(o)==nullptr) o = o->getParent();
    assert(o!=nullptr);
    Firewall *f = Firewall::cast(o);
    string platform = f->getStr("platform");
    FWOptions *ropt = rule->getOptionsObject();

    if (platform!="iosacl" && platform!="procurve_acl")
    {
        if (ropt->getBool("stateless"))
        {
            options << qMakePair(QObject::tr("Stateless"), QString(""));
        } else
        {
            options << qMakePair(QObject::tr("Stateful"), QString(""));
        }
    }

    if (platform=="iptables")
    {
        if (prule)
        {
            if (prule->getTagging())
            {
                options << qMakePair(QObject::tr("tag:"), 
                                     QString(prule->getTagValue().c_str()));
            }

            if (prule->getClassification())
            {
                options << qMakePair(QObject::tr("class:"),
                                     QString(ropt->getStr("classify_str").c_str()));
            }
        }
            
        if (!ropt->getStr("log_prefix").empty())
        {
            options << qMakePair(QObject::tr("Log prefix:"),
                                 QString(ropt->getStr("log_prefix").c_str()));
        }

        if (!ropt->getStr("log_level").empty())
        {
            options << qMakePair(
                QObject::tr("Log Level:"),
                QString(getScreenName(ropt->getStr("log_level").c_str(),
                                      getLogLevels(platform.c_str()))));
        }

        if (ropt->getInt("ulog_nlgroup")>1)
        {
            options << qMakePair(
                QObject::tr("Netlink group:"),
                QString(ropt->getStr("ulog_nlgroup").c_str()));
        }
            
        if (ropt->getInt("limit_value")>0)
        {
            QString arg;
            if (ropt->getBool("limit_value_not")) arg = " ! ";
            arg += QString(ropt->getStr("limit_value").c_str());
            if (!ropt->getStr("limit_suffix").empty())
            {
                arg += getScreenName(ropt->getStr("limit_suffix").c_str(),
                                     getLimitSuffixes(platform.c_str()));
            }
            options << qMakePair(QString("Limit value:"), arg);
        }

        if (ropt->getInt("limit_burst")>0)
        {
            options << qMakePair(QString("Limit burst:"),
                                 QString(ropt->getStr("limit_burst").c_str()));
        }

        if (ropt->getInt("connlimit_value")>0)
        {
            QString arg;

            if (ropt->getBool("connlimit_above_not")) arg = " ! ";
            arg += QString(ropt->getStr("connlimit_value").c_str());

            options << qMakePair(QObject::tr("connlimit value:"), arg);
        }

        if (ropt->getInt("hashlimit_value")>0)
        {
            QString arg;
            if (ropt->getBool("hashlimit_value_not")) arg = " ! ";
            arg += QString(ropt->getStr("hashlimit_value").c_str());
            if (!ropt->getStr("hashlimit_suffix").empty())
            {
                arg += getScreenName(ropt->getStr("limit_suffix").c_str(),
                                     getLimitSuffixes(platform.c_str()));
            }

            options << qMakePair(
                QString("hashlimit name:"),
                QString(ropt->getStr("hashlimit_name").c_str()));
            options << qMakePair(QString("hashlimit value:"), arg);

            if (ropt->getInt("hashlimit_burst")>0)
            {
                options << qMakePair(
                    QString("haslimit burst:"),
                    QString(ropt->getStr("hashlimit_burst").c_str()));
            }
        }
            
        if (ropt->getBool("firewall_is_part_of_any_and_networks"))
        {
            options << qMakePair(QObject::tr("Part of Any"), QString(""));
        }

            
    } else if (platform=="ipf") 
    {
        if (!ropt->getStr("ipf_log_facility").empty())
        {
            options << qMakePair(
                QObject::tr("Log facility:"),
                QString(getScreenName(ropt->getStr("ipf_log_facility").c_str(),
                                      getLogFacilities(platform.c_str()))));
        }
            
        if (!ropt->getStr("log_level").empty())
        {
            options << qMakePair(
                QObject::tr("Log level:"),
                QString(getScreenName(ropt->getStr("log_level").c_str(),
                                      getLogLevels(platform.c_str()))));
        }
            
        if (ropt->getBool("ipf_return_icmp_as_dest"))
        {
            options << qMakePair(
                QObject::tr("Send 'unreachable'"), QString(""));
        }

        if (ropt->getBool("ipf_keep_frags"))
        {
            options << qMakePair(
                QObject::tr("Keep information on fragmented packets"),
                QString(""));
        }
            
    }else if (platform=="pf")
    {

        if (prule)
        {
            if (prule->getTagging())
            {
                options << qMakePair(QObject::tr("tag:"), 
                                     QString(prule->getTagValue().c_str()));
            }

            if (prule->getClassification())
            {
                options << qMakePair(QObject::tr("queue:"),
                                     QString(ropt->getStr("pf_classify_str").c_str()));
            }
        }

        if (!ropt->getStr("log_prefix").empty())
        {
            options << qMakePair(QObject::tr("Log prefix:"), 
                                 QString(ropt->getStr("log_prefix").c_str()));
        }
            
        if (ropt->getInt("pf_rule_max_state")>0)
        {
            options << qMakePair(
                QObject::tr("Max state:"),
                QString(ropt->getStr("pf_rule_max_state").c_str()));
        }
            
        if (ropt->getBool("pf_keep_state"))
        {
            options << qMakePair(
                QObject::tr("Force 'keep-state'"), QString(""));
        }
        if (ropt->getBool("pf_no_sync"))
        {
            options << qMakePair(QString("no-sync"), QString(""));
        }
        if (ropt->getBool("pf_pflow"))
        {
            options << qMakePair(QString("pflow"), QString(""));
        }
        if (ropt->getBool("pf_sloppy_tracker"))
        {
            options << qMakePair(QString("sloppy-tracker"), QString(""));
        }
            
        if (ropt->getBool("pf_source_tracking"))
        {
            options << qMakePair(
                QObject::tr("Source tracking"), QString(""));
                
            options << qMakePair(
                QObject::tr("Max src nodes:"),
                QString(ropt->getStr("pf_max_src_nodes").c_str()));
                
            options << qMakePair(
                QObject::tr("Max src states:"),
                QString(ropt->getStr("pf_max_src_states").c_str()));
        }

        if (ropt->getBool("pf_synproxy"))
        {
            options << qMakePair(QString("synproxy"), QString(""));
        }

        if (ropt->getBool("pf_modulate_state"))
        {
            options << qMakePair(QString("modulate_state"), QString(""));
        }
            
    }else if (platform=="ipfw")
    {
        ;
    }else if (platform == "iosacl" || platform == "procurve_acl")
    {
        if (ropt->getBool("iosacl_add_mirror_rule"))
        {
            options << qMakePair(
                QObject::tr("Add mirrored rule"), QString(""));
        }
            
    }else if (platform=="pix" || platform=="fwsm")
    {
        string vers = "version_"+f->getStr("version");
            
        options << qMakePair(QObject::tr("Version:"), QString(vers.c_str()));
            
        if ( Resources::platform_res[platform]->getResourceBool(
                 "/FWBuilderResources/Target/options/"+vers+"/pix_rule_syslog_settings"))
        {
                
            if (!ropt->getStr("log_level").empty())
            {
                options << qMakePair(
                    QObject::tr("Log level:"),
                    QString(getScreenName(ropt->getStr("log_level").c_str(),
                                          getLogLevels(platform.c_str()))));
            }
            if (ropt->getInt("log_interval")>0)
            {
                options << qMakePair(
                    QObject::tr("Log interval:"),
                    QString(ropt->getStr("log_interval").c_str()));
            }
                
            if (ropt->getBool("disable_logging_for_this_rule"))
            {
                options << qMakePair(
                    QObject::tr("Disable logging for this rule"), QString(""));
            }
                
        } 
    }

    if (prule)
        options << qMakePair(
            QObject::tr("Logging: "),
            (prule->getLogging()) ? QObject::tr("on") : QObject::tr("off"));

    QStringList res;
    res << "<table>";
    QList<QPair<QString,QString> >::iterator it;
    for (it=options.begin(); it!=options.end(); ++it)
    {
        QPair<QString,QString> p = *it;
        res << "<tr><th align='left'>" + p.first + "</th><td>" + p.second + "</td></tr>";
    }
    res << "</table>";
    QString html = res.join("\n");
    if (fwbdebug)
        qDebug() << html;
    return html;
}

QString FWObjectPropertiesFactory::getNATRuleOptions(Rule *rule)
{
    QString res;
    
    if (rule!=nullptr)
    {
        res="";
        FWObject *o = rule;
        while (o!=nullptr && Firewall::cast(o)==nullptr) o=o->getParent();
        assert(o!=nullptr);
        Firewall *f=Firewall::cast(o);
        string platform=f->getStr("platform");
        FWOptions *ropt = rule->getOptionsObject();

        if (fwbdebug)
            qDebug() << "getNATRuleOptions: platform: " << platform.c_str();

        if (platform=="iptables")
        {
            if (ropt->getBool("ipt_use_snat_instead_of_masq"))
                res += QObject::tr("use SNAT instead of MASQ<br>");
            if (ropt->getBool("ipt_use_masq"))
                res += QObject::tr("always use MASQUERADE<br>");
            if (ropt->getBool("ipt_nat_random"))      res += QObject::tr("random<br>");
            if (ropt->getBool("ipt_nat_persistent"))  res += QObject::tr("persistent<br>");
        }
            
        if (platform=="pf")
        {
            if (ropt->getBool("pf_bitmask"))      res+=QObject::tr("bitmask<br>");
            if (ropt->getBool("pf_random"))       res+=QObject::tr("random<br>");
            if (ropt->getBool("pf_source_hash"))  res+=QObject::tr("source-hash<br>");
            if (ropt->getBool("pf_round_robin"))  res+=QObject::tr("round-robin<br>");
            if (!res.isEmpty()) res += ",";
            if (ropt->getBool("pf_static_port"))  res+=QObject::tr("static-port<br>");
        }
    }
    
    return res;
}

QString FWObjectPropertiesFactory::getInterfaceNameExamplesForHostOS(const QString &host_os)
{
    Resources *os_resources = Resources::os_res[host_os.toStdString()];
    if (os_resources == nullptr) return "";
    string os_family = os_resources-> getResourceStr("/FWBuilderResources/Target/family");

    if (os_family == "linux24" ||
        os_family == "ipcop"   ||
        os_family == "openwrt" ||
        os_family == "dd-wrt-nvram" ||
        os_family == "dd-wrt-jffs" ||
        os_family == "sveasoft") return "eth0, eth0.100, vlan100, br0, etc";
    
    if (os_family == "openbsd" ||
        os_family == "freebsd" ||
        os_family == "macosx") return "en0, fxp0, vlan100, etc";
    
    if (os_family == "ios" ||
        os_family == "pix_os") return "FastEthernet0/0, etc";
    
    if (os_family == "procurve") return "vlan 10, a1, b1, etc";

    return "";
}

