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

#include <qmessagebox.h>
#include <QDateTime>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;

ClusterDialog::~ClusterDialog()
{
    delete m_dialog;
}

void ClusterDialog::getHelpName(QString *str)
{
    *str = "ClusterDialog";
}

ClusterDialog::ClusterDialog(QWidget *parent)
    : BaseObjectDialog(parent)
{
    m_dialog = new Ui::ClusterDialog_q;
    m_dialog->setupUi(this);
    obj = NULL;
}

void ClusterDialog::loadFWObject(FWObject *o)
{
    obj = o;
    Cluster *s = dynamic_cast<Cluster*>(obj);
    assert(s != NULL);

    QString platform = obj->getStr("platform").c_str();
    // fill in platform
    setPlatform(m_dialog->platform, platform);

    // fill in host OS
    setHostOS(m_dialog->hostOS, platform, obj->getStr("host_OS").c_str());

    updateTimeStamps();

    /*
      Management *mgmt = s->getManagementObject();
      assert(mgmt != NULL);
    */

    m_dialog->obj_name->setText(QString::fromUtf8(s->getName().c_str()));

    m_dialog->comment->setText(QString::fromUtf8(s->getComment().c_str()));

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

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

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
    changed();

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
        changed();
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
}


void ClusterDialog::applyChanges()
{
    FWCmdChange* cmd = new FWCmdChange(m_project, obj);
    FWObject* new_state = cmd->getNewState();

    Cluster *s = dynamic_cast<Cluster*>(new_state);
    assert(s != NULL);

    string oldname = obj->getName();
    string newname = string(m_dialog->obj_name->text().toUtf8().constData());
    string oldplatform = obj->getStr("platform");

    new_state->setName(newname);
    new_state->setComment(string(m_dialog->comment->toPlainText().toUtf8().constData()));

    string pl = readPlatform(m_dialog->platform).toLatin1().constData();
    new_state->setStr("platform", pl);
    new_state->setStr("host_OS", readHostOS(m_dialog->hostOS).toLatin1().constData());

    s->setInactive(m_dialog->inactive->isChecked());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd);
    }    

    updateTimeStamps();
}

