/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"

#include "FWWindow.h"
#include "newClusterDialog.h"
#include "InterfacesTabWidget.h"
#include "platforms.h"
#include "FWBTree.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QtDebug>

using namespace libfwbuilder;
using namespace std;


void newClusterDialog::createNewCluster()
{
    QList<ClusterInterfaceData> cluster_interfaces =
        this->m_dialog->interfaceSelector->getInterfaces();
    
    QList<QPair<Firewall*, bool> > member_firewalls = 
        this->m_dialog->firewallSelector->getSelectedFirewalls();

    typedef QPair<Firewall*, bool> fwpair;
    Firewall *master = NULL;
    foreach(fwpair member, member_firewalls)
    {
        if (member.second) 
        {
            master = member.first;
            break;
        }
    }

    FWObject *o;
    o = db->create(Cluster::TYPENAME);
    o->setName(string(m_dialog->obj_name->text().toUtf8().constData()));

    if (o == NULL)
    {
        QDialog::accept();
        return;
    }

    parent->add(o);

    ncl = Cluster::cast(o);

    o->setStr("platform",
              this->m_dialog->firewallSelector->getSelectedFirewalls().first().first->getStr("platform"));
    o->setStr("host_OS",
              this->m_dialog->firewallSelector->getSelectedFirewalls().first().first->getStr("host_OS"));

    foreach(EditedInterfaceData data, this->m_dialog->interfaceEditor->getNewData())
    {
        Interface *oi = Interface::cast(db->create(Interface::TYPENAME));
        oi->setName(string(data.name.toUtf8().constData()));

        ncl->add(oi);
        oi->setLabel(string(data.label.toUtf8().constData()));

        QList<QPair<Firewall*, Interface*> > member_interfaces;

        foreach(ClusterInterfaceData cid, cluster_interfaces)
        {
            if (cid.name == data.name)
            {
                member_interfaces = cid.interfaces;
                break;
            }
        }

        foreach(AddressInfo address, data.addresses)
        {
            if (address.ipv4)
            {
                QString addrname = QString("%1:%2:ip")
                               .arg(m_dialog->obj_name->text())
                               .arg(data.name);
                IPv4 *oa = IPv4::cast(db->create(IPv4::TYPENAME));
                oa->setName(string(addrname.toUtf8().constData()));
                oi->add(oa);
                oa->setAddress(InetAddr(address.address.toLatin1().constData()));
                bool ok = false ;
                int inetmask = address.netmask.toInt(&ok);
                if (ok)
                {
                    oa->setNetmask(InetAddr(inetmask));
                }
                else
                {
                    oa->setNetmask(InetAddr(address.netmask.toLatin1().constData()));
                }
            }
            else
            {
                QString addrname = QString("%1:%2:ip")
                               .arg(m_dialog->obj_name->text())
                               .arg(data.name);
                IPv6 *oa = IPv6::cast(db->create(IPv6::TYPENAME));
                oa->setName(string(addrname.toUtf8().constData()));
                oi->add(oa);
                oa->setAddress(InetAddr(AF_INET6, address.address.toLatin1().constData()));
                bool ok = false ;
                int inetmask = address.netmask.toInt(&ok);
                if (ok)
                {
                    oa->setNetmask(InetAddr(AF_INET6, inetmask));
                }
                else
                {
                    oa->setNetmask(InetAddr(AF_INET6, address.netmask.toLatin1().constData()));
                }
            }
        }

        FWOptions *ifopt;
        ifopt = oi->getOptionsObject();
        ifopt->setStr("type", "cluster_interface");

        // create failover group for this interface
        QString grpname = QString("%1:%2:members")
                          .arg(m_dialog->obj_name->text())
                          .arg(data.name);

        ClusterGroup *failover_grp = ClusterGroup::cast(
            db->create(FailoverClusterGroup::TYPENAME));
        failover_grp->setName(string(grpname.toUtf8().constData()));
        oi->add(failover_grp);

        QString failover_protocol_name = data.protocol.toLower();

        failover_grp->setStr("type",
                             failover_protocol_name.toAscii().constData());

        typedef QPair<Firewall*, Interface*> intfpair;
        foreach(intfpair intf, member_interfaces)
        {
            Firewall *member_fw = intf.first;
            Interface *member_intf = intf.second;
            failover_grp->addRef(member_intf);
            if (member_fw == master)
            {
                std::string masteriface_id =
                    FWObjectDatabase::getStringId(member_intf->getId());
                failover_grp->setStr("master_iface", masteriface_id);
            }
        }
    }
}

