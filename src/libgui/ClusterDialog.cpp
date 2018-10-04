/*
 * ClusterDialog.cpp - Cluster view implementation
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

#include "ClusterDialog.h"

#include "utils.h"
#include "platforms.h"
#include "DialogFactory.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"

#include <memory>

#include <qmessagebox.h>
#include <QDateTime>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;

ClusterDialog::~ClusterDialog()
{
    delete m_dialog;
}

ClusterDialog::ClusterDialog(QWidget *parent)
    : BaseObjectDialog(parent)
{
    m_dialog = new Ui::ClusterDialog_q;
    m_dialog->setupUi(this);
    obj = nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

void ClusterDialog::loadFWObject(FWObject *o)
{
    obj = o;
    Cluster *s = dynamic_cast<Cluster*>(obj);
    assert(s != nullptr);

    QString platform = obj->getStr("platform").c_str();
    // fill in platform
    setPlatform(m_dialog->platform, platform);

    // fill in host OS
    setHostOS(m_dialog->hostOS, platform, obj->getStr("host_OS").c_str());

    updateTimeStamps();

    /*
      Management *mgmt = s->getManagementObject();
      assert(mgmt != nullptr);
    */

    m_dialog->obj_name->setText(QString::fromUtf8(s->getName().c_str()));

    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->inactive->setChecked(s->getInactive());

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->platform->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->platform);

    m_dialog->hostOS->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->hostOS);

    /*
      m_dialog->fwAdvanced->setEnabled(!o->isReadOnly());
      setDisabledPalette(m_dialog->fwAdvanced);

      m_dialog->osAdvanced->setEnabled(!o->isReadOnly());
      setDisabledPalette(m_dialog->osAdvanced);
    */

    m_dialog->inactive->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->inactive);
}

void ClusterDialog::updateTimeStamps()
{
    QDateTime dt;
    time_t t;
            
    t = obj->getInt("lastModified");
    dt.setTime_t(t);
    m_dialog->last_modified->setText((t)? dt.toString():"-");
            
    t = obj->getInt("lastCompiled");
    dt.setTime_t(t);
    m_dialog->last_compiled->setText((t)? dt.toString():"-");
            
    t = obj->getInt("lastInstalled");
    dt.setTime_t(t);
    m_dialog->last_installed->setText((t)? dt.toString():"-");
}

void ClusterDialog::platformChanged()
{
    config_changed = true;
    emit changed();

    QString platform = readPlatform(m_dialog->platform);
    setHostOS(m_dialog->hostOS, platform, "");

    QString pl = readPlatform(m_dialog->platform);

    resetClusterGroupTypes();
}

void ClusterDialog::hostOSChanged()
{
    if (readHostOS(m_dialog->hostOS).toLatin1().constData() !=
        obj->getStr("host_OS"))
    {
        config_changed = true;
        resetClusterGroupTypes();
        emit changed();
    }
}

/*
 * Check if type of failover and state sync groups matches current
 * platform/host os configuration and if not, fix it.
 */
void ClusterDialog::resetClusterGroupTypes()
{
    QString host_os = readHostOS(m_dialog->hostOS);
    list<QStringPair> state_sync_types;
    getStateSyncTypesForOS(host_os, state_sync_types);

    for (FWObjectTypedChildIterator it = obj->findByType(StateSyncClusterGroup::TYPENAME);
         it != it.end(); ++it)
        resetSingleClusterGroupType(*it, state_sync_types);

    list<QStringPair> failover_types;
    getFailoverTypesForOS(host_os, failover_types);
    list<FWObject*> failover_groups = obj->getByTypeDeep(FailoverClusterGroup::TYPENAME);
    for (list<FWObject*>::iterator it = failover_groups.begin(); it != failover_groups.end(); ++it)
        resetSingleClusterGroupType(*it, failover_types);

}

void ClusterDialog::resetSingleClusterGroupType(FWObject *grp,
                                                list<QStringPair> &allowed_types)
{
    string first_allowed_type;
    bool match = false;
    foreach(QStringPair p, allowed_types)
    {
        if (first_allowed_type.empty()) first_allowed_type = p.first.toStdString();
        if (grp->getStr("type") == p.first.toStdString())
            match = true;
    }
    if (!match) grp->setStr("type", first_allowed_type);
}

void ClusterDialog::validate(bool *res)
{
    *res = true;
    if (!validateName(this, obj, m_dialog->obj_name->text()))
    {
        *res = false;
        return;
    }
    // see #2011 - do not allow "/" in firewall object name
    if (m_dialog->obj_name->text().contains("/"))
    {
        *res = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(
                this,"Firewall Builder",
                tr("Character \"/\" is not allowed in cluster object name"),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
            blockSignals(false);
        }
        return;
    }
}


void ClusterDialog::applyChanges()
{
    bool autorename_chidren = false;
    QString dialog_txt = tr(
        "The name of the object '%1' has changed. The program can also "
        "rename IP address objects that belong to this object, "
        "using standard naming scheme 'host_name:interface_name:ip'. "
        "This makes it easier to distinguish what host or a firewall "
        "given IP address object belongs to when it is used in "
        "the policy or NAT rule. The program also renames MAC address "
        "objects using scheme 'host_name:interface_name:mac'. "
        "Do you want to rename child IP and MAC address objects now? "
        "(If you click 'No', names of all address objects that belong to "
        "%2 will stay the same.)")
        .arg(QString::fromUtf8(obj->getName().c_str()))
        .arg(QString::fromUtf8(obj->getName().c_str()));

    if (obj->getName() != m_dialog->obj_name->text().toUtf8().constData())
    {
        /* see comment about this in FirewallDialog */
        blockSignals(true);
        autorename_chidren = (QMessageBox::warning(
                                  this,"Firewall Builder", dialog_txt,
                                  tr("&Yes"), tr("&No"), QString::null,
                                  0, 1 )==0 );
        blockSignals(false);
    }

    std::unique_ptr<FWCmdChange> cmd(
        new FWCmdChange(m_project, obj, "", autorename_chidren));
    FWObject* new_state = cmd->getNewState();

    Cluster *s = dynamic_cast<Cluster*>(new_state);
    assert(s != nullptr);

    string oldname = obj->getName();
    string newname = string(m_dialog->obj_name->text().toUtf8().constData());
    string oldplatform = obj->getStr("platform");

    new_state->setName(newname);
    m_dialog->commentKeywords->applyChanges(new_state);

    string pl = readPlatform(m_dialog->platform).toLatin1().constData();
    new_state->setStr("platform", pl);
    new_state->setStr("host_OS", readHostOS(m_dialog->hostOS).toLatin1().constData());

    s->setInactive(m_dialog->inactive->isChecked());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }    

    updateTimeStamps();
}

