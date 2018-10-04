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
#include "events.h"
#include "platforms.h"

#include "newFirewallDialog.h"
#include "FWBSettings.h"
#include "FWBTree.h"
#include "InterfaceEditorWidget.h"
#include "InterfacesTabWidget.h"
#include "FWWindow.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"

#include <QtDebug>
#include <QMessageBox>

using namespace libfwbuilder;
using namespace std;


class FindNetwork : public FWObjectFindPredicate
{
    InetAddrMask net;

    public:

    FindNetwork(InetAddrMask _net) { net = _net; }

    virtual bool operator()(FWObject *o) const
    {
        FWObject *obj = FWReference::getObject(o);

        if (Address::cast(obj))
        {
            const InetAddrMask *am = Address::cast(obj)->getInetAddrMaskObjectPtr();
            if (am && (*am) == net) return true;
        }
        return false;
    }
};

void newFirewallDialog::createFirewallFromTemplate()
{
    QListWidgetItem *itm = m_dialog->templateList->currentItem();
    FWObject *template_fw = templates[itm];
    assert (template_fw!=nullptr);

    string platform = readPlatform(m_dialog->platform).toLatin1().constData();
    string host_os = readHostOS(m_dialog->hostOS).toLatin1().constData();

    map<int, int> map_ids;
    FWObject *no = db_copy->recursivelyCopySubtree(parent, template_fw, map_ids);

    no->setName( string( m_dialog->obj_name->text().toUtf8().constData() ) );

    nfw = Firewall::cast(no);

    no->setStr("platform", platform);
    no->setStr("host_OS", host_os);
    
    /*
     * If we set defaults for the platform and host OS, then we lose
     * all settings that were done in the template. See ticket
     * #1340. Not setting defaults fixes #1340 with a caveat: since
     * the name of the same (sematically) option can be different for
     * different firewall platforms, options set in the template
     * generally are only rpeserved if the new firewall object uses
     * the same platform as the template. In practical terms this
     * basically means iptables. If user changes the platform, they
     * need to revisit options and fix them manually
     */

    //Resources::setDefaultTargetOptions(platform , nfw);
    //Resources::setDefaultTargetOptions(host_os , nfw);
}

void newFirewallDialog::changedAddressesInNewFirewall()
{
    // the key in this map is the pointer to interface that used to be
    // part of the template. We do not allow the user to create new or 
    // delete existing interfaces when they edit template interfaces.

    QMap<Interface*, EditedInterfaceData> new_configuration =
        m_dialog->interfaceEditor2->getData();

    list<FWObject*> all_interfaces = nfw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator intiter=all_interfaces.begin();
         intiter != all_interfaces.end(); ++intiter )
    {
        Interface *intf = Interface::cast(*intiter);

        list<FWObject*> old_addr = intf->getByType(IPv4::TYPENAME);
        list<FWObject*> old_ipv6 = intf->getByType(IPv6::TYPENAME);
        old_addr.splice(old_addr.begin(), old_ipv6);

        if (new_configuration.count(intf) == 0)
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Can not find interface %1 in the interface editor data")
                .arg(intf->getName().c_str()),
                "&Continue", QString::null, QString::null, 0, 1 );
        } else
        {
            EditedInterfaceData new_data = new_configuration[intf];

            replaceInterfaceAttributes(nfw, intf, &new_data);

            if (new_data.type == 0)  // regular interface
            {
                /*
                 * Substitute interface addresses.
                 * 
                 * EditedInterfaceData::addresses uses Address* as a
                 * key. If it is not nullptr, then this points to the
                 * updated information for existing Address object
                 * which we should modify. If it is nullptr, then the
                 * user created new address and we need to create new
                 * IPv4 or IPv6 object.
                 * 
                 * If user created more addresses than there used to
                 * be, extra addresses are not added to rules.
                 */
                QMap<libfwbuilder::Address*, AddressInfo >::iterator addrit;
                for (addrit=new_data.addresses.begin(); addrit!=new_data.addresses.end(); ++addrit)
                {
                    Address *old_addr_obj = addrit.key();
                    InetAddrMask old_net;
                    AddressInfo new_addr = addrit.value();

                    if (old_addr_obj)
                    {
                        const InetAddrMask *old_addr_mask =
                            old_addr_obj->getInetAddrMaskObjectPtr();
                        old_net = InetAddrMask(
                            *(old_addr_mask->getAddressPtr()),
                            *(old_addr_mask->getNetmaskPtr()));
                    }

                    Address *oa = replaceInterfaceAddressData(nfw,
                                                              intf,
                                                              old_addr_obj,
                                                              new_addr.address,
                                                              new_addr.netmask,
                                                              new_addr.ipv4);
                    
                    const InetAddrMask *new_addr_mask =
                        oa->getInetAddrMaskObjectPtr();
                    InetAddrMask new_net = InetAddrMask(
                        *(new_addr_mask->getAddressPtr()),
                        *(new_addr_mask->getNetmaskPtr()));

                    if (old_addr_obj)
                    {
                        // User edited address of the interface. Remove it
                        // from old_addr so we won't delete it later.
                        old_addr.remove(old_addr_obj);

                        // Now check if the actually changed the
                        // address and/or netmask. If they did, create
                        // new network object and replace references
                        // to network objects that match old address/
                        // netmask with references pointing to the new
                        // one.
                        if ( ! (old_net == new_net))
                            replaceReferencesToNetworks(nfw, intf,
                                                        old_net, new_net);
                    }
                }
            }

            // Now delete old address objects that are still in the
            // old_addr list. These are the object that were deleted
            // in the editor. Do not forget to remove references to
            // thse objects in rules and groups, if any.
            while (old_addr.size())
            {
                Address *addr = Address::cast(old_addr.front());
                old_addr.pop_front();
                if (addr)
                {
                    nfw->removeAllReferences(addr);
                    intf->remove(addr, false);
                    delete addr;
                }
            }
        }
    }
}

void newFirewallDialog::replaceInterfaceAttributes(Firewall*,
                                                   Interface *intf,
                                                   EditedInterfaceData *new_data)
{
    intf->setName( string(new_data->name.toUtf8().constData()));
    intf->setLabel( string(new_data->label.toUtf8().constData()));
    intf->setComment( string(new_data->comment.toUtf8().constData()));

    if (fwbdebug)
        qDebug() << "Interface" << intf->getName().c_str()
                 << "type=" << new_data->type;

    switch (new_data->type)
    {
    case 1:
        intf->setDyn(true);
        intf->setUnnumbered(false);
        break;
    case 2:
        intf->setDyn(false);
        intf->setUnnumbered(true);
        break;
    default:
        intf->setDyn(false);
        intf->setUnnumbered(false);
        break;
    }
}

Address* newFirewallDialog::replaceInterfaceAddressData(Firewall *fw,
                                                        Interface *intf,
                                                        Address *addr_obj,
                                                        const QString &address,
                                                        const QString &netmask,
                                                        bool ipv4)
{
    Address *oa;
    QString name;
    if (ipv4)
    {
        if (addr_obj) oa = addr_obj;
        else
        {
            oa = IPv4::cast(db_copy->create(IPv4::TYPENAME));
            intf->add(oa);
        }
        name = QString("%1:%2:ipv4")
            .arg(fw->getName().c_str())
            .arg(intf->getName().c_str());
        oa->setAddress(
            InetAddr(address.toStdString()));
        bool ok = false ;
        int inetmask = netmask.toInt(&ok);
        if (ok) oa->setNetmask(InetAddr(inetmask));
        else oa->setNetmask(InetAddr(netmask.toStdString()));
                        
    } else
    {
        if (addr_obj) oa = addr_obj;
        else
        {
            oa = IPv6::cast(db_copy->create(IPv6::TYPENAME));
            intf->add(oa);
        }
        name = QString("%1:%2:ipv6")
            .arg(fw->getName().c_str())
            .arg(intf->getName().c_str());
        oa->setAddress(
            InetAddr(AF_INET6, address.toStdString()) );
        bool ok = false ;
        int inetmask = netmask.toInt(&ok);
        if (ok) oa->setNetmask(InetAddr(AF_INET6, inetmask));
    }
    oa->setName(name.toStdString());
    return oa;
}


/*
 * Find references to NEtwork and NetworkIPv6 objects that match
 * old_net and if there are any, create new Network or NetworkIPv6
 * object with address/netmask defined by new_net and replace
 * references.
 */
void newFirewallDialog::replaceReferencesToNetworks(Firewall *fw,
                                                    Interface *intf,
                                                    InetAddrMask old_net,
                                                    InetAddrMask new_net)
{
    if(old_net.isAny()) return;  // do not replace references to 0/0
    
    FWObject *current_lib = fw->getLibrary();

    // Find all matching Network and NetworkIPv6
    // objects used in the rules
    FindNetwork pred(old_net);
    list<FWObject*> res = fw->findIf(&pred);
    set<FWObject*> old_nets;
    if (res.size())
    {
        // eliminate duplicates
        for (list<FWObject*>::iterator it=res.begin(); it!=res.end(); ++it)
            old_nets.insert(FWObjectReference::getObject(*it));

        string net_type = Network::TYPENAME;
        if (old_net.getAddressPtr()->isV6()) net_type = NetworkIPv6::TYPENAME;
        FWObject *parent = 
            FWBTree().getStandardSlotForObject(
                current_lib, net_type.c_str());
        Address *new_net_obj = Address::cast(db_copy->create(net_type));
        parent->add(new_net_obj);

        QString new_net_name =
            QString("%1:%2:net").arg(fw->getName().c_str()).arg(intf->getName().c_str());
        new_net_obj->setName(new_net_name.toStdString());
        new_net_obj->setAddress(*(new_net.getAddressPtr()));
        new_net_obj->setNetmask(*(new_net.getNetmaskPtr()));

        QString filename = mw->activeProject()->getFileName();
        QCoreApplication::postEvent(
            mw, new insertObjectInTreeEvent(filename,
                                            parent->getId(),
                                            new_net_obj->getId()));

        for (set<FWObject*>::iterator it=old_nets.begin(); it!=old_nets.end(); ++it)
        {
            FWObject *old_obj = FWObjectReference::getObject(*it);
            replaceReferencesToObject(fw, old_obj, new_net_obj);
        }
    }
}

void newFirewallDialog::replaceReferencesToObject(Firewall *fw,
                                                  FWObject *old_obj,
                                                  FWObject *new_obj)
{
    map<int, int> map_ids;
    map_ids[old_obj->getId()] = new_obj->getId();
    db_copy->fixReferences(fw, map_ids);
}

