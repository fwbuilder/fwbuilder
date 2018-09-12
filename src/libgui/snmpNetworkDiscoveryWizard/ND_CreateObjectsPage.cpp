/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "events.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "platforms.h"

#include "ND_CreateObjectsPage.h"
#include "SNMPNetworkDiscoveryWizard.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/Firewall.h"

#include <QStringList>
#include <QMessageBox>
#include <QtDebug>

#include <memory>

using namespace std;
using namespace libfwbuilder;


ND_CreateObjectsPage::ND_CreateObjectsPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_CreateObjectsPage_q;
    m_dialog->setupUi(this);

}

void ND_CreateObjectsPage::initializePage()
{
    if (fwbdebug) qDebug() << "ND_CreateObjectsPage::initializePage()";

    ObjectDescriptorList *objects =
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjects();
    QStringList *objectsToUse =
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjectsToUse();
    ObjectDescriptorList *networks = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getNetworks();

    int lib_index = field("libIndex").toInt();
    QStringList libraries = field("libraries").toStringList();

    if (fwbdebug)
        qDebug() << "libraries=" << libraries;

    m_dialog->progressBar->setFormat("%v / %m");
    m_dialog->progressBar->setMaximum(objectsToUse->size() / 2 + networks->size());
    FWObject *last_object = nullptr;
    string type, name, a;
    int counter = 0;

    foreach(ObjectDescriptor od, *networks)
    {
        type = od.type; // Network or NetworkIPv6
        name = od.sysname;
        a = od.addr.toString().c_str();
        
        Address *net = Address::cast(
            mw->createObject(type.c_str(), name.c_str()));

        assert(net!=nullptr);

        net->setName(name);
        net->setAddress(od.addr);
        net->setNetmask(od.netmask);
        
        mw->moveObject(libraries[lib_index], net);

        last_object = net;

        m_dialog->progressBar->setValue(counter);
        qApp->processEvents();

        counter++;
    }

    foreach(ObjectDescriptor od, *objects)
    {
        if (objectsToUse->contains(QString::fromUtf8(od.sysname.c_str())))
        {
            type = od.type;

            name = od.sysname;

            QString platform;
            QString os;
            QString version;
            guessOSAndPlatformFromSysDescr(od.descr.c_str(), platform, os, version);

            a = od.addr.toString();

            if (type==Host::TYPENAME || type==Firewall::TYPENAME)
            {
                FWObject *o=nullptr;

                o = mw->createObject(type.c_str(), name.c_str());
                o->setName(name);

                if (type==Firewall::TYPENAME)
                {
                    if (os == "linux")
                    {
                        o->setStr("platform", "iptables");
                        o->setStr("host_OS", "linux24");
                    }
                    if (os == "freebsd")
                    {
                        o->setStr("platform", "pf");
                        o->setStr("host_OS", "freebsd");
                    }
                    if (os == "openbsd")
                    {
                        o->setStr("platform", "pf");
                        o->setStr("host_OS", "openbsd");
                    }
                    if (os == "ios")
                    {
                        o->setStr("platform", "iosacl");
                        o->setStr("host_OS", "ios");
                    }
                    if (os == "pix" || os == "fwsm")
                    {
                        o->setStr("platform", "pix");
                        o->setStr("host_OS", "pix_os");
                    }
                    if (os == "apple")
                    {
                        o->setStr("platform", "ipfw");
                        o->setStr("host_OS", "macosx");
                    }
                    if (os == "solaris")
                    {
                        o->setStr("platform", "ipf");
                        o->setStr("host_OS", "solaris");
                    }

                    Resources::setDefaultTargetOptions( o->getStr("platform"),
                                                        Firewall::cast(o) );
                    Resources::setDefaultTargetOptions( o->getStr("host_OS"),
                                                        Firewall::cast(o) );
                }

                if (od.interfaces.size()==0)
                {
                    Interface *itf= Interface::cast(
                        mw->createObject(o,Interface::TYPENAME,"nic1")
                    );

                    if (od.addr.isV4())
                    {
                        IPv4 *ipv4= IPv4::cast(
                            mw->createObject(itf, IPv4::TYPENAME, a.c_str())
                        );
                        ipv4->setAddress(od.addr);
                        ipv4->setNetmask(InetAddr());
                    }

                    if (od.addr.isV6())
                    {
                        IPv6 *ipv6 = IPv6::cast(
                            mw->createObject(itf, IPv6::TYPENAME, a.c_str())
                        );
                        ipv6->setAddress(od.addr);
                        ipv6->setNetmask(InetAddr());
                    }

                } else
                {
                    if (fwbdebug)
                    {
                        map<int,InterfaceData>::iterator i;
                        for (i=od.interfaces.begin(); i!=od.interfaces.end(); ++i)
                        {
                            InterfaceData *intf = &(i->second);
                            QString str("Discovered interface %1: %2");
                            qDebug() <<
                                str.arg(intf->name.c_str()).arg(intf->mac_addr.c_str());

                        }
                    }

                    list<InterfaceData*> interface_tree;
                    std::unique_ptr<interfaceProperties> int_prop(
                        interfacePropertiesObjectFactory::getInterfacePropertiesObject(o));
                    int_prop->rearrangeInterfaces(od.interfaces, interface_tree);

                    if (interface_tree.size() != od.interfaces.size())
                    {
                        // Some interfaces have been converted to subinterfaces
                        // Show warning

                        QMessageBox::warning(
                            this, "Firewall Builder",
                            tr(
"Some discovered interfaces have been rearranged in "
"fwbuilder objects and recreated as subinterfaces to "
"reflect VLANs, bonding and bridging configurations. "
"The algorithm used to guess correct relationship "
"between interfaces and subinterfaces is imperfect "
"because of the limited information provided by SNMP "
"daemon. Pelase review created objects to make sure "
"generated configuration is accurate. "
"\n"
"\n"
"The program expects MAC addresses of bonding, bridge "
"and vlan interfaces to be the same. It is especially "
"important to review and fix generated objects if you "
"use MAC address spoofing."
),
                            tr("&Continue"), nullptr, nullptr,
                            0 );


                    }

                    list<InterfaceData*>::iterator it;
                    for (it=interface_tree.begin(); it!=interface_tree.end(); ++it)
                    {
                        InterfaceData *in = *it;
                        // if this interface has subinterfaces, add even if it
                        // has no ip address (last arg)

                        FWObject *intf = addInterface(
                            o, in, in->subinterfaces.size()!=0);
                        if (intf == nullptr) continue;

                        list<InterfaceData*>::iterator sit;
                        for (sit=in->subinterfaces.begin();
                             sit!=in->subinterfaces.end(); ++sit)
                        {
                            InterfaceData *subint = *sit;
                            addInterface(intf, subint, true);
                        }
                    }
                }

                if (!od.descr.empty())
                {
                    FWOptions* opt=(dynamic_cast<Host*>(o))->getOptionsObject();
                    opt->setStr("snmp_description",od.descr);
                    opt->setStr("snmp_location",   od.location);
                    opt->setStr("snmp_contact",    od.contact);
                }

                mw->moveObject(libraries[lib_index], o);

            } else if (type==Network::TYPENAME)
            {
                Network *net=dynamic_cast<Network*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(net!=nullptr);
                net->setName(name);
                net->setAddress(InetAddr(a));
                net->setNetmask(InetAddr(InetAddr(a)));
                mw->moveObject(libraries[lib_index], net);
            } else if (type==IPv4::TYPENAME)
            {
                IPv4 *obj=dynamic_cast<IPv4*>(
                    mw->createObject(type.c_str(),name.c_str())
                );
                assert(obj!=nullptr);
                obj->setName(name);
                obj->setAddress(InetAddr(a));
                obj->setNetmask(InetAddr(InetAddr::getAllOnes()));
                mw->moveObject(libraries[lib_index], obj);
            }

            m_dialog->progressBar->setValue(counter);
            qApp->processEvents();

            counter++;
        }
    }

    ProjectPanel *pp = mw->activeProject();
    QString filename = pp->getFileName();
    QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));
    QCoreApplication::postEvent(
        mw->activeProject(), new openLibraryForObjectEvent(
            filename, last_object->getId()));
}

FWObject* ND_CreateObjectsPage::addInterface(FWObject *parent, InterfaceData *in,
                                             bool skip_ip_address_check)
{
    ObjectManipulator *om = mw->activeProject()->m_panel->om;

    bool includeUnnumbered = field("snmpIncludeUnnumbered").toBool();

    if ( ! includeUnnumbered && ! skip_ip_address_check)
    {
        if (in->addr_mask.size()==0) return nullptr;
        if (in->addr_mask.front()->getAddressPtr()->isAny())
            return nullptr;
    }

    QString obj_name = in->name.c_str();
    Interface *itf = nullptr;
    itf = Interface::cast(
        mw->createObject(parent,
                         QString(Interface::TYPENAME), obj_name));
    
    QString iname = om->getStandardName(itf, physAddress::TYPENAME, "mac");
    iname = om->makeNameUnique(itf, iname, physAddress::TYPENAME);

    physAddress *paddr = physAddress::cast(
        mw->createObject(itf, physAddress::TYPENAME, iname)
    );
    paddr->setPhysAddress(in->mac_addr);

    itf->setLabel(in->label);
    itf->setSecurityLevel(in->securityLevel);

    if (fwbdebug)
        qDebug() << "Interface=" << obj_name
                 << "type=" << in->interface_type.c_str();

    if (!in->interface_type.empty())
    {
        itf->getOptionsObject()->setStr("type", in->interface_type);
        if (in->interface_type == "8021q")
            itf->getOptionsObject()->setInt("vlan_id", in->vlan_id);
    } else
    {
        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(parent));
        if (int_prop->looksLikeVlanInterface(obj_name))
        {
            QString base_name;
            int vlan_id;
            int_prop->parseVlan(obj_name, &base_name, &vlan_id);

            itf->getOptionsObject()->setStr("type", "8021q");
            itf->getOptionsObject()->setInt("vlan_id", vlan_id);
        }
    }

    if (in->addr_mask.size()==0 ||
        in->addr_mask.front()->getAddressPtr()->isAny())
    {
        itf->setUnnumbered(true);
    } else
    {
        list<InetAddrMask*>::iterator n;
        for (n=in->addr_mask.begin(); n!=in->addr_mask.end(); ++n)
        {
            const InetAddr *addr = (*n)->getAddressPtr();
            const InetAddr *netm = (*n)->getNetmaskPtr();

            if (addr->isV4())
            {
                try
                {
                    QString iname = om->getStandardName(itf, IPv4::TYPENAME, "ip");
                    iname = om->makeNameUnique(itf, iname, IPv4::TYPENAME);

                    IPv4 *ipv4= IPv4::cast(
                        om->createObject(itf, IPv4::TYPENAME, iname)
                    );
                    ipv4->setAddress(*addr);
                    ipv4->setNetmask(*netm);
                } catch (FWException &ex)
                {
                    cerr << "FWException: " << ex.toString() << endl;
                }
            }

            if (addr->isV6())
            {
                try
                {
                    QString iname = om->getStandardName(itf, IPv6::TYPENAME, "ip");
                    iname = om->makeNameUnique(itf, iname, IPv6::TYPENAME);

                    IPv6 *ipv6 = IPv6::cast(
                        om->createObject(itf, IPv6::TYPENAME, iname)
                    );
                    ipv6->setAddress(*addr);
                    ipv6->setNetmask(*netm);
                } catch (FWException &ex)
                {
                    cerr << "FWException: " << ex.toString() << endl;
                }
            }
        }
    }
    return itf;
}

