/*
 * cluster members dialog implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include "clusterMembersDialog.h"
#include "global.h"
#include "utils_no_qt.h"

#include "FWWindow.h"
#include "Help.h"
#include "FWCmdChange.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <algorithm>

#include <qdebug.h>
#include <qpixmapcache.h>

#include <QHeaderView>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;

typedef std::list<libfwbuilder::Firewall *> t_fwList;

clusterMembersDialog::clusterMembersDialog(QWidget *parent, FWObject *o)
    : QDialog(parent), table_update(false)
{
    m_dialog = new Ui::clusterMembersDialog_q;
    m_dialog->setupUi(this);
    setWindowModality(Qt::WindowModal);

    // assign clustergroup object
    obj = o;
    FWObject *parent_host = Host::getParentHost(obj);
    host_os = parent_host->getStr("host_OS").c_str();
    platform = parent_host->getStr("platform").c_str();

    // if empty, retry with parent of parent (interface level)
    if (host_os.isEmpty())
    {
        FWObject *parent = nullptr;
        parent = obj->getParent();
        if (parent == nullptr)
        {
            throw FWException("clusterMembersDialog: parent is nullptr!");
        }
        parent = parent->getParent();
        if (parent == nullptr)
        {
            throw FWException("clusterMembersDialog: parent is nullptr!");
        }
        host_os = parent->getStr("host_OS").c_str();
        platform = parent->getStr("platform").c_str();
    }

    string type = obj->getStr("type");
    enable_master_column = Resources::os_res[host_os.toStdString()]->getResourceBool(
        "/FWBuilderResources/Target/protocols/" + type + "/needs_master");
    if (!enable_master_column) m_dialog->fwSelectedTable->hideColumn(2);

    // prepare lists of firewalls (selected, available)
    getSelectedMembers();
    getPossibleMembers();

    // init views
    updateAvailableTree();
    updateSelectedTable();
}

clusterMembersDialog::~clusterMembersDialog()
{
    // clear member lists
    for (t_memberList::iterator it = available.begin();
        it != available.end(); it++)
    {
        delete *it;
    }
    available.clear();

    for (t_memberList::iterator it = selected.begin();
        it != selected.end(); it++)
    {
        delete *it;
    }
    selected.clear();

    delete m_dialog;
}

void clusterMembersDialog::getSelectedMembers()
{
    // read in master interface id
    std::string master_iface = obj->getStr("master_iface");

    for (FWObjectTypedChildIterator it =
        obj->findByType(FWObjectReference::TYPENAME);
        it != it.end(); ++it)
    {

        // get fw and interface pointer from interface reference
        Interface *iface = nullptr;
        iface = Interface::cast(FWReference::cast((*it))->getPointer());
        assert(iface != nullptr);
        Firewall *fw = Firewall::cast(Host::getParentHost(iface));
        //Firewall *fw = Firewall::cast(iface->getParentHost());

        // determine master
        std::string iface_id = FWObjectDatabase::getStringId(iface->getId());
        bool master = false;
        if (iface_id == master_iface)
        {
            master = true;
        }
        // create ClusterMember object
        ClusterMember *new_member = createMember(fw, iface, master);
        if (new_member == nullptr)
        {
            qWarning() << "clusterMembersDialog: could not create new "
                "cluster member";
            return;
        }
        // attach to selected list
        selected.push_back(new_member);
    }
}

void clusterMembersDialog::getPossibleMembers()
{
    t_fwList fwlist;

    mw->findAllFirewalls(fwlist);

    Firewall *fw;
    for (t_fwList::iterator it = fwlist.begin(); it != fwlist.end(); it++)
    {
        // does host_OS and platform match?
        fw = *it;
        if (fw->getStr("host_OS").c_str() != host_os ||
            fw->getStr("platform").c_str() != platform)
        {
            continue;
        }

        // does the firewall provide at least one phys. interface?
        FWObjectTypedChildIterator iface_i = fw->findByType(Interface::TYPENAME);
        if (iface_i == iface_i.end())
        {
            continue;
        }
        else
        {
            // previously selected? skip
            PredFindFw pred;
            pred.setSearchString(fw->getName().c_str());
            t_memberList::iterator it = find_if(selected.begin(),
                                                selected.end(), pred);
            if (it != selected.end())
            {
                continue;
            }

            // valid member, add to member list
            ClusterMember *new_member = createMember(fw);
            if (new_member == nullptr)
            {
                qWarning() << "clusterMembersDialog: could not create new "
                    "cluster member";
                return;
            }
            available.push_back(new_member);
        }
    }
    fwlist.sort(FWObjectNameCmpPredicate());
}

void clusterMembersDialog::updateSelectedTable()
{
    table_update = true;
    m_dialog->fwSelectedTable->setRowCount(selected.size());

    QTableWidgetItem *item = nullptr;
    int row = 0;
    for (t_memberList::const_iterator it = selected.begin();
            it != selected.end(); it++)
    {

        // only insert new QTableWidgetItems if none has been set, update text
        // if selected member text changed.

        item = m_dialog->fwSelectedTable->item(row, 0);
        const char *new_text = (*it)->fwobj->getName().c_str();
        if (item == nullptr)
        {
            item = new QTableWidgetItem;
            item->setText(new_text);
            item->setIcon(QIcon(getIcon((*it)->fwobj)));
            m_dialog->fwSelectedTable->setItem(row, 0, item);
        }
        else if (item->text() != new_text)
        {
            item->setText(new_text);
        }

        // Column "Interface"
        item = m_dialog->fwSelectedTable->item(row, 1);
        new_text = (*it)->iface_cluster->getName().c_str();
        if (item == nullptr)
        {
            item = new QTableWidgetItem;
            item->setText(new_text);
            item->setIcon(QIcon(getIcon((*it)->iface_cluster)));
            m_dialog->fwSelectedTable->setItem(row, 1, item);
        }
        else if (item->text() != new_text)
        {
            item->setText(new_text);
        }

        // Column "Master"
        item = m_dialog->fwSelectedTable->item(row, 2);
        Qt::CheckState state = (*it)->is_master ? Qt::Checked : Qt::Unchecked;
        if (item == nullptr)
        {
            item = new QTableWidgetItem;
            item->setCheckState(state);
            m_dialog->fwSelectedTable->setItem(row, 2, item);
        }
        else if (item->checkState() != state)
        {
            item->setCheckState(state);
        }

        row++;
    }

    m_dialog->fwSelectedTable->resizeColumnsToContents();
    m_dialog->fwSelectedTable->horizontalHeader()->setStretchLastSection(true);

    table_update = false;
}

void clusterMembersDialog::updateAvailableTree()
{
    QTreeWidgetItem *fwitem;

    m_dialog->fwAvailableTree->clear();
    for (t_memberList::const_iterator it = available.begin();
        it != available.end(); it++)
    {

        ClusterMember *member = *it;
        fwitem = new QTreeWidgetItem;
        fwitem->setFlags(Qt::ItemIsEnabled);
        fwitem->setText(0, member->fwobj->getName().c_str());
        fwitem->setIcon(0, QIcon(getIcon(member->fwobj)));

        // add interfaces
        for (t_ifaceList::const_iterator it = member->iface_list.begin();
            it != member->iface_list.end(); it ++)
        {
            QTreeWidgetItem *ifitem;
            ifitem = new QTreeWidgetItem(fwitem);

            ifitem->setText(1, (*it)->getName().c_str());
            ifitem->setIcon(1, QIcon(getIcon(*it)));

            // add label (if non empty)
            string label = (*it)->getLabel();
            if (!label.empty())
            {
                ifitem->setText(2, label.c_str());
            }
        }
        m_dialog->fwAvailableTree->insertTopLevelItem(0, fwitem);
    }

    m_dialog->fwAvailableTree->resizeColumnToContents(0);
    m_dialog->fwAvailableTree->sortByColumn(0, Qt::AscendingOrder);
    m_dialog->fwAvailableTree->expandAll();
}

ClusterMember*
clusterMembersDialog::createMember(Firewall *fw,
                                   Interface *cluster_iface, bool master)
{
    if (fw == nullptr)
    {
        return nullptr;
    }

    ClusterMember *new_member = new ClusterMember;
    new_member->fwobj = fw;
    new_member->is_master = master;
    if (cluster_iface != nullptr)
    {
        new_member->iface_cluster = cluster_iface;
    }

    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    interfaces.sort(FWObjectNameCmpPredicate());

    list<FWObject*>::iterator iface_i;
    for (iface_i=interfaces.begin(); iface_i != interfaces.end(); ++iface_i)
    {
        Interface *iface = Interface::cast(*iface_i);
        new_member->iface_list.push_back(iface);
        // init interface mapping table
        new_member->iface_map[iface->getName().c_str()] = iface;
    }
    return new_member;
}

bool clusterMembersDialog::swap(t_memberList &from, t_memberList &to,
                                     const QString fwname, const QString iface, bool master)
{
    // move selected fw from 'from' to 'to' member list
    PredFindFw pred;
    pred.setSearchString(fwname);
    t_memberList::iterator it = find_if(from.begin(), from.end(), pred);
    if (it == from.end())
    {
        // not found
        return false;
    }

    ClusterMember *member = *it;
    from.erase(it);

    member->iface_cluster = member->iface_map[iface];
    member->is_master = master;
    to.push_back(member);

    return true;
}

void clusterMembersDialog::setMaster(QString fw, bool checked)
{
    for (t_memberList::const_iterator it = selected.begin();
        it != selected.end(); it++)
    {
        if (QString((*it)->fwobj->getName().c_str()) == fw)
        {
            (*it)->is_master = checked;
        }
        else
        {
            (*it)->is_master = false;
        }
    }
    updateSelectedTable();
}

QPixmap clusterMembersDialog::getIcon(FWObject *o)
{
    QString icn_file = (":/Icons/" + o->getTypeName() +
                        "/icon").c_str();

    QPixmap pm;
    if (!QPixmapCache::find(icn_file, pm))
    {
        pm.load(icn_file);
        QPixmapCache::insert(icn_file, pm);
    }
    return pm;
}

void clusterMembersDialog::invalidate()
{
    // update views
    updateAvailableTree();
    updateSelectedTable();

    // disable <--> buttons, user needs to re-select
    m_dialog->buttonAdd->setEnabled(false);
    m_dialog->buttonRemove->setEnabled(false);
}

void clusterMembersDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));
    FWObject* new_state = cmd->getNewState();

    bool master_found = false;
    t_memberList::const_iterator it = selected.begin();

    // remoive all existing references and add new ones
    list<FWObject*> all_refs = new_state->getByType(FWObjectReference::TYPENAME);
    for (list<FWObject*>::iterator it=all_refs.begin(); it!=all_refs.end(); ++it)
        new_state->remove(*it);

    // add selected interfaces as objref to cluster member group
    for (it = selected.begin(); it != selected.end(); it++)
    {
        new_state->addRef((*it)->iface_cluster);
        // set master interface ref id
        if ((*it)->is_master)
        {
            master_found = true;
            std::string masteriface_id =
                FWObjectDatabase::getStringId((*it)->iface_cluster->getId());
            new_state->setStr("master_iface", masteriface_id);
        }
    }
    if (!master_found)
    {
        new_state->remStr("master_iface");
    }
    emit membersChanged();

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void clusterMembersDialog::reject()
{
    QDialog::reject();
}

void clusterMembersDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Cluster-Member Management");
    h->setSource(QUrl("clusterMembersDialog.html#" + anchor));
    h->raise();
    h->show();
}

void clusterMembersDialog::availableClicked(QTreeWidgetItem *item, int)
{
    // activate addButton if a specific interface has been selected
    if (item->text(1).isEmpty())
    {
        m_dialog->buttonAdd->setEnabled(false);
    }
    else if (!m_dialog->buttonAdd->isEnabled())
    {
        m_dialog->buttonAdd->setEnabled(true);
    }
}

void clusterMembersDialog::selectedClicked(int row, int column)
{
    if (fwbdebug)
    {
        qDebug() << "clusterMembersDialog: selected (" << row << ", "
                 << column << ")";
    }

    // activate removeButton
    if (!m_dialog->buttonRemove->isEnabled())
    {
        m_dialog->buttonRemove->setEnabled(true);
    }
}

void clusterMembersDialog::masterSelected(int row, int column)
{
    if (!table_update)
    {
        if (fwbdebug)
        {
            qDebug() << "clusterMembersDialog: master is (" << row
                     << ", " << column << ")";
        }

        QList<QTableWidgetItem *> itemlist;
        itemlist = m_dialog->fwSelectedTable->selectedItems();

        if (itemlist[2]->checkState() == Qt::Checked)
        {
            setMaster(itemlist[0]->text());
        }
        else
        {
            setMaster(itemlist[0]->text(), false);
        }
    }
}

void clusterMembersDialog::firewallAdd()
{
    // get selected firewall / interface
    QList<QTreeWidgetItem *> itemlist;
    itemlist = m_dialog->fwAvailableTree->selectedItems();

    // interface should not be empty
    if (itemlist[0]->text(1).isEmpty())
    {
        qWarning() << "clusterMembersDialog: iface is empty, not adding";
        return;
    }

    foreach(QTreeWidgetItem *itm, itemlist)
    {
        // move selected fw to selected member list
        QString fwname = itm->parent()->text(0);
        QString iface_cluster = itm->text(1);
        if (!swap(available, selected, fwname, iface_cluster))
        {
            // swap failed, this should not happen!
            qWarning() << "clusterMembersDialog: swap failed for firewall "
                       << fwname << ", interface: " << iface_cluster;
            return;
        }
    }
    // invalidate view
    invalidate();
}

void clusterMembersDialog::firewallRemove()
{
    // get selected firewall / interface
    QList<QTableWidgetItem *> itemlist;
    itemlist = m_dialog->fwSelectedTable->selectedItems();

    // move selected fw to available member list
    QString fwname = itemlist[0]->text();
    if (!swap(selected, available, fwname, "", false))
    {
        // swap failed, this should not happen!
        qWarning() << "clusterMembersDialog: swap failed for firewall "
                   << fwname;
        return;
    }
    // invalidate view
    invalidate();
}

