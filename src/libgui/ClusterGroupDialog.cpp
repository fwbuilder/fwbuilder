/*
 * ClusterGroupDialog.cpp - ClusterGroup view implementation
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

#include "ClusterGroupDialog.h"

#include "utils.h"
#include "platforms.h"
#include "events.h"

#include "ObjectListViewItem.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "DialogFactory.h"
#include "vrrpOptionsDialog.h"
#include "FWCmdChange.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"

#include <memory>

#include <qpixmapcache.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <QCoreApplication>
#include <QtDebug>
#include <QUndoStack>

#include <algorithm>
#include <iostream>

using namespace std;
using namespace libfwbuilder;

ClusterGroupDialog::~ClusterGroupDialog()
{
    delete m_dialog;
}

ClusterGroupDialog::ClusterGroupDialog(QWidget *parent)
    : BaseObjectDialog(parent)
{
    m_dialog = new Ui::ClusterGroupDialog_q;
    m_dialog->setupUi(this);
    obj = nullptr;
    reload = false;

    connectSignalsOfAllWidgetsToSlotChange();
}

void ClusterGroupDialog::loadFWObject(FWObject *o)
{
    obj = o;
    ClusterGroup *g = dynamic_cast<ClusterGroup*>(obj);
    assert(g != nullptr);

    init = true;

    // disable manage members if host OS does not support clustering.
    // Parent is either 'Cluster' or 'Interface', call getParent() approprietly
    FWObject *parent = obj;
    while (parent && !Cluster::isA(parent)) parent = parent->getParent();
    if (parent == nullptr)
    {
        throw FWException("ClusterGroupDialog: parent is nullptr!");
    }
    cluster = Cluster::cast(parent);
    string host_os = cluster->getStr("host_OS");

    // Sanity check
    // Failover type could be wrong if user changed host OS of the cluster
    string type = obj->getStr("type");

    list<QStringPair> possible_cluster_group_types;
    if (StateSyncClusterGroup::isA(o))
        getStateSyncTypesForOS(host_os.c_str(), possible_cluster_group_types);
    if (FailoverClusterGroup::isA(o))
        getFailoverTypesForOS(host_os.c_str(), possible_cluster_group_types);

    enable_master_column = Resources::os_res[host_os]->getResourceBool(
        "/FWBuilderResources/Target/protocols/" + type + "/needs_master");

    if (enable_master_column) m_dialog->fwMemberTree->showColumn(2);
    else m_dialog->fwMemberTree->hideColumn(2);
    
    bool acceptable_failover_type = false;
    for (list<QStringPair>::iterator it=possible_cluster_group_types.begin();
         it!=possible_cluster_group_types.end(); ++it)
    {
        QString t = it->first;
        if (t == QString(type.c_str()))
        {
            acceptable_failover_type = true;
            break;
        }
    }
    if (!acceptable_failover_type && possible_cluster_group_types.size())
        obj->setStr(
            "type", possible_cluster_group_types.front().first.toStdString());

    m_dialog->obj_name->setText(QString::fromUtf8(g->getName().c_str()));
    m_dialog->commentKeywords->loadFWObject(o);
    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    QString grp_type = obj->getStr("type").c_str();
    m_dialog->type->clear();
    int cp = 0;
    for (list<QStringPair>::iterator i1=possible_cluster_group_types.begin();
         i1!=possible_cluster_group_types.end(); i1++,cp++)
    {
        m_dialog->type->addItem( i1->second );
        if ( grp_type == i1->first ) m_dialog->type->setCurrentIndex(cp);
    }

    // init link icons, master firewall is colored
    m_dialog->fwMemberTree->clear();

    string master_iface = g->getStr("master_iface");
    for (FWObject::iterator it = g->begin(); it != g->end(); it++)
    {
        FWObject *o = FWObjectReference::getObject(*it);
        if (Interface::isA(o))
        {
            if (master_iface == FWObjectDatabase::getStringId(o->getId()))
            {
                addIcon(o, true);
            }
            else
            {
                addIcon(o);
            }
        }
    }

    if (!Resources::getTargetCapabilityBool(host_os, "supports_cluster"))
    {
        m_dialog->manageMembers->setEnabled(false);
        m_dialog->manageMembers->setToolTip(
            QObject::tr("Feature not supported by host OS '%1'").arg(host_os.c_str()));
    }
    else
    {
        m_dialog->manageMembers->setEnabled(true);
        m_dialog->manageMembers->setToolTip(
            QObject::tr("Click here to manage member firewalls of this cluster group."));
    }

    m_dialog->fwMemberTree->resizeColumnToContents(0);
    m_dialog->fwMemberTree->resizeColumnToContents(1);
    m_dialog->fwMemberTree->resizeColumnToContents(2);
    m_dialog->fwMemberTree->resizeColumnToContents(3);

    QString dlgname = DialogFactory::getClusterGroupOptionsDialogName(
        ClusterGroup::cast(obj)->getOptionsObject());

    if (fwbdebug)
        qDebug() << "ClusterGroupDialog::loadFWObject dlgname=" << dlgname;

    m_dialog->editParameters->setEnabled(!dlgname.isEmpty());

    init = false;
}

void ClusterGroupDialog::saveGroupType(FWObject *group)
{
    QString host_os = cluster->getStr("host_OS").c_str();
    list<QStringPair> possible_cluster_group_types;
    if (StateSyncClusterGroup::isA(obj))
        getStateSyncTypesForOS(host_os, possible_cluster_group_types);
    if (FailoverClusterGroup::isA(obj))
        getFailoverTypesForOS(host_os, possible_cluster_group_types);

    QString  grp_type = m_dialog->type->currentText();
    list<QStringPair>::iterator li =
        std::find_if(possible_cluster_group_types.begin(),
                     possible_cluster_group_types.end(),
                     findSecondInQStringPair(grp_type));
    if (li != possible_cluster_group_types.end())
        group->setStr("type", li->first.toLatin1().constData() );
}

void ClusterGroupDialog::addIcon(FWObject *o, bool master)
{
    FWObject *iface = o;
    assert(Interface::cast(iface)!=nullptr);
    FWObject *fw = Host::getParentHost(iface);
//    FWObject *fw = Interface::cast(iface)->getParentHost(); // because iface can be subinterface
    bool valid = cluster->validateMember(Firewall::cast(fw));
    QString iface_name = QString::fromUtf8(iface->getName().c_str());
    QString fw_name = QString::fromUtf8(fw->getName().c_str());

    QString iface_icn_file = (":/Icons/" + iface->getTypeName() +
                              "/icon-ref").c_str();
    QString fw_icn_file = (":/Icons/" + fw->getTypeName() +
                           "/icon-ref").c_str();

    QPixmap iface_pm;
    if (!QPixmapCache::find(iface_icn_file, iface_pm))
    {
        iface_pm.load(iface_icn_file);
        QPixmapCache::insert(iface_icn_file, iface_pm);
    }
    QPixmap fw_pm;
    if (!QPixmapCache::find(fw_icn_file, fw_pm))
    {
        fw_pm.load(fw_icn_file);
        QPixmapCache::insert(fw_icn_file, fw_pm);
    }

    ObjectListViewItem *item = new ObjectListViewItem(m_dialog->fwMemberTree);
    int col = 0;

    item->setText(col, fw_name);
    item->setIcon(col, QIcon(fw_pm));
    col++;

    item->setText(col, iface_name);
    item->setIcon(col, QIcon(iface_pm));
    col++;

    // note that if enable_master_column==false, this column is hidden
    // but we still need to create an item in this column.
    if (master)  item->setText(col, tr("Master"));
    else item->setText(col, tr(""));
    col++;

    if (valid)
    {
        item->setText(col, "OK");
        item->setToolTip(
            col, tr("Firewall %1 can be used as a member of this cluster").arg(fw->getName().c_str()));
    } else
    {
        item->setText(col, tr("Invalid"));
        item->setToolTip(
            col, tr("Firewall %1 can not be used as a member of this cluster\n because its host OS or platform does not match those of the cluster.").arg(fw->getName().c_str()));
        item->setBackgroundColor(col, QColor(255, 0, 0, 100));
    }

    item->setProperty("type", iface->getTypeName().c_str());
    item->setFWObject(iface);
}

void ClusterGroupDialog::changed()
{
    if (fwbdebug)
        qDebug() << "ClusterGroupDialog::changed()";
    if (!reload) BaseObjectDialog::changed();
}

void ClusterGroupDialog::validate(bool *res)
{
    *res = true;
    if (!validateName(this, obj, m_dialog->obj_name->text()))
    {
        *res = false;
    }
}

void ClusterGroupDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

#ifndef NDEBUG
    ClusterGroup *g = dynamic_cast<ClusterGroup*>(new_state);
    assert(g != nullptr);
#endif

    QString oldname = obj->getName().c_str();
    new_state->setName(string(m_dialog->obj_name->text().toUtf8().constData()));
    m_dialog->commentKeywords->applyChanges(new_state);

    saveGroupType(new_state);

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

/*
 * This method is connected to the "Edit members" button and opens dialog
 * where user chooses cluster member firewalls and interfaces
 */
void ClusterGroupDialog::openClusterConfDialog()
{
    try
    {
        QWidget *w = DialogFactory::createClusterConfDialog(this, obj);
        if (w == nullptr)
        {
            return;   // some dialogs may not be implemented yet
        }
        QDialog *d = dynamic_cast<QDialog*>(w);
        assert(d != nullptr);

        // connect obj changed signal
        //connect(d, SIGNAL(membersChanged()), this, SLOT(objectChanged()));

        if (d->exec() == QDialog::Accepted)
        {
            // modal dialog, dialog saves data into the object
            
            // update object tree (if members have changed, the object
            // properties summary text may have to change too)
            mw->activeProject()->updateObjectInTree(obj, true);

            // reload object to reflect changes in members
            loadFWObject(obj);

            // mark as modified
            changed();
        }
        delete d;
    }
    catch (FWException &ex)
    {
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("FWBuilder API error: %1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null, QString::null, 0, 1);
        return;
    }
}

void ClusterGroupDialog::openObject(QTreeWidgetItem *item)
{
    ObjectListViewItem *otvi = dynamic_cast<ObjectListViewItem*>(item);
    assert(otvi != nullptr);

    FWObject *o = otvi->getFWObject();
    if (o != nullptr)
    {
        QCoreApplication::postEvent(
            mw, new showObjectInTreeEvent(o->getRoot()->getFileName().c_str(),
                                          o->getId()));
    }
}

void ClusterGroupDialog::objectChanged()
{
    reload = true;
    loadFWObject(obj);
    reload = false;
}

/*
 * this method is connected to the "Edit protocol parameters" button
 * and opens dialog where user edits state sync and failover
 * (heartbeat/openais/vrrp/carp/conntrack/etc) protocol parameters.
 */
void ClusterGroupDialog::openParametersEditor()
{
    FWOptions *gr_opt = ClusterGroup::cast(obj)->getOptionsObject();

    QDialog *dlg = dynamic_cast<QDialog*>(
        DialogFactory::createClusterGroupOptionsDialog(this, gr_opt));

    if (dlg)
    {
        if (dlg->exec() == QDialog::Accepted)
        {
            // modal dialog, dialog saves data into the object
            
            // update object tree (if protocol type has changed, the
            // object properties summary text may have to change too)
            mw->activeProject()->updateObjectInTree(obj, true);
            changed();
        }
        delete dlg;
    }
}

