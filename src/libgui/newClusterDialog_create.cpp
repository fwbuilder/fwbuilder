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

#include "global.h"
#include "events.h"
#include "FWWindow.h"
#include "newClusterDialog.h"
#include "InterfacesTabWidget.h"
#include "platforms.h"
#include "FWBTree.h"
#include "FWCmdAddObject.h"
#include "RuleSetModel.h"
#include "RuleSetView.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QtDebug>

using namespace libfwbuilder;
using namespace std;


void newClusterDialog::createNewCluster()
{
    if (fwbdebug) qDebug() << "newClusterDialog::createNewCluster()";

    map<int, int> id_mapping;

    QList<ClusterInterfaceData> cluster_interfaces =
        this->m_dialog->interfaceSelector->getInterfaces();
    
    typedef QPair<Firewall*, bool> fwpair;
    Firewall *master = nullptr;
    QList<QPair<Firewall*, bool> > member_firewalls = 
        this->m_dialog->firewallSelector->getSelectedFirewalls();
    foreach(fwpair member, member_firewalls)
    {
        if (member.second) { master = member.first; break; }
    }

    FWObject *o;
    o = db->create(Cluster::TYPENAME);

    if (o == nullptr)
    {
        QDialog::accept();
        return;
    }

    o->setName(string(m_dialog->obj_name->text().toUtf8().constData()));

    parent->add(o);

    ncl = Cluster::cast(o);

    o->setStr("platform",
              this->m_dialog->firewallSelector->getSelectedFirewalls().first().first->getStr("platform"));
    o->setStr("host_OS",
              this->m_dialog->firewallSelector->getSelectedFirewalls().first().first->getStr("host_OS"));

    if (fwbdebug) qDebug() << "newClusterDialog::createNewCluster()"
                           << "Creating interfaces";

    foreach(EditedInterfaceData data, this->m_dialog->interfaceEditor->getNewData())
    {
        Interface *oi = Interface::cast(db->create(Interface::TYPENAME));
        oi->setName(string(data.name.toUtf8().constData()));

        if (fwbdebug)
            qDebug() << "newClusterDialog::createNewCluster()"
                     << "Interface" << data.name;

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

        if (fwbdebug)
            qDebug() << "newClusterDialog::createNewCluster()"
                     << "Setting up failover group"
                     << "master=" << master;

        FWOptions *ifopt;
        ifopt = oi->getOptionsObject();
        ifopt->setStr("type", "cluster_interface");

        // create failover group for this interface
        QString grpname = QString("%1:%2:members")
                          .arg(m_dialog->obj_name->text())
                          .arg(data.name);

        FailoverClusterGroup *failover_grp = FailoverClusterGroup::cast(
            db->create(FailoverClusterGroup::TYPENAME));
        failover_grp->setName(string(grpname.toUtf8().constData()));
        oi->add(failover_grp);

        QString failover_protocol_name = data.protocol.toLower();
        failover_grp->setStr("type",
                             failover_protocol_name.toLatin1().constData());

        typedef QPair<Firewall*, Interface*> intfpair;
        foreach(intfpair intf, member_interfaces)
        {
            Firewall *member_fw = intf.first;
            Interface *member_intf = intf.second;

            if (fwbdebug)
                qDebug() << "Adding"
                         << "member_fw=" << member_fw
                         << "member_intf=" << member_intf->getName().c_str();

            id_mapping[member_intf->getId()] = oi->getId();

            failover_grp->addRef(member_intf);

            if (master!=nullptr && member_fw == master)
            {
                std::string masteriface_id =
                    FWObjectDatabase::getStringId(member_intf->getId());
                failover_grp->setStr("master_iface", masteriface_id);
            }
        }

        // need to populate failover group with some reasonable
        // default values.  If this is not done, parameters such as
        // CARP vhid remain blank and that leads to incomplete
        // generated configurations
        setDefaultFailoverGroupAttributes(failover_grp);
    }

    // Set correct type of the state sync group (the StateSyncGroup object is
    // created in Cluster::init()

    FWObject *state_sync_members =
        ncl->getFirstByType(StateSyncClusterGroup::TYPENAME);
    setDefaultStateSyncGroupAttributes(
        StateSyncClusterGroup::cast(state_sync_members));

    // Copy rule sets if requested
    Firewall *source = nullptr;
    foreach (QRadioButton* btn, copy_rules_from_buttons.keys())
    {
        if (btn->isChecked() && btn != noPolicy)
        {
            source = copy_rules_from_buttons[btn];
            break;
        }
    }

    if (fwbdebug) qDebug() << "newClusterDialog::createNewCluster() checkpoint 4";

    ProjectPanel *pp = mw->activeProject();
    QString filename = pp->getFileName();


    if (source == nullptr)
    {
        if (fwbdebug)
            qDebug() << "newClusterDialog::createNewCluster() checkpoint 5";

        FWObject *first_policy = ncl->getFirstByType(Policy::TYPENAME);

        if (fwbdebug)
            qDebug() << "newClusterDialog::createNewCluster() checkpoint 6"
                     << "first_policy=" << first_policy;

        QCoreApplication::postEvent(
            mw, new openRulesetEvent(filename, first_policy->getId()));
        return;
    }

    // See #1622 If rule set view shows rules of the firewall
    // <source>, need to close it because we are about to delete that
    // rule set object

    RuleSet* current_ruleset = nullptr;
    RuleSetView* rsv = pp->getCurrentRuleSetView();
    RuleSetModel* md = nullptr;
    if (rsv)
    {
        md = (RuleSetModel*)rsv->model();
        current_ruleset = md->getRuleSet();
    }

    if (current_ruleset && current_ruleset->isChildOf(source))
    {
        pp->closeRuleSet(current_ruleset);
    }

    db->setIgnoreReadOnlyFlag(true);

    FWObject *fwgroup =
        FWBTree().getStandardSlotForObject(parent->getLibrary(),
                                           Firewall::TYPENAME);

    foreach(fwpair member, member_firewalls)
    {
        Firewall *fw = member.first;
        id_mapping[fw->getId()] = ncl->getId();

        string name_bak = fw->getName() + "-bak";
        FWCmdAddObject *cmd = new FWCmdAddObject(
            mw->activeProject(), fwgroup, nullptr,
            QString("Create new Firewall %1")
            .arg(QString::fromUtf8(name_bak.c_str())));
        cmd->setNeedTreeReload(true);
        FWObject *new_state = cmd->getNewState();
        Firewall *bakfw = Firewall::cast(new_state->addCopyOf(fw));
        bakfw->setName(name_bak);
        bakfw->setInactive(true);
        mw->activeProject()->undoStack->push(cmd);
    }

    copyRuleSets(Policy::TYPENAME, source, id_mapping);
    copyRuleSets(NAT::TYPENAME, source, id_mapping);
    copyRuleSets(Routing::TYPENAME, source, id_mapping);

    //ncl->getRoot()->fixReferences(ncl, id_mapping);

    foreach(fwpair member, member_firewalls)
    {
        Firewall *fw = member.first;
        deleteRuleSets(Policy::TYPENAME, fw);
        deleteRuleSets(NAT::TYPENAME, fw);
        deleteRuleSets(Routing::TYPENAME, fw);
    }

    db->setIgnoreReadOnlyFlag(false);

    FWObject *first_policy = ncl->getFirstByType(Policy::TYPENAME);
    QCoreApplication::postEvent(
        mw, new openRulesetEvent(filename, first_policy->getId()));
}

void newClusterDialog::deleteRuleSets(const string &type, Firewall *fw)
{
    list<FWObject*> rule_sets = fw->getByType(type);
    foreach(FWObject *rs, rule_sets) fw->remove(rs);
    fw->add(db->create(type));
}

void newClusterDialog::copyRuleSets(const string &type, Firewall *source,
                                    map<int, int> &id_mapping)
{
    list<FWObject*> old_ones = ncl->getByType(type);
    foreach(FWObject *old, old_ones)
        ncl->remove(old);
    FWObjectDatabase *db = ncl->getRoot();
    FWObjectTypedChildIterator it = source->findByType(type);
    for (; it != it.end(); ++it)
    {
        FWObject *new_ruleset = ncl->addCopyOf(*it);
        id_mapping[(*it)->getId()] = new_ruleset->getId();
        db->fixReferences(new_ruleset, id_mapping);
    }

    /*
     * since the order in which we copy rule sets is undefined and
     * because they may have references to each other via branching
     * rules, we need to fix references in them after we create all of
     * them. This fixes SF bug #3106168 "Branch destinations lost when adding to cluster"
     */

    it = ncl->findByType(type);
    for (; it != it.end(); ++it)
    {
        db->fixReferences(*it, id_mapping);
    }

}


