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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "ProjectPanel.h"

#include "FWBTree.h"
#include "FirewallDialog.h"
#include "DialogFactory.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Resources.h"

#include <vector>
#include <map>
#include <algorithm>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qtimer.h>
#include <QDateTime>
#include <QUndoStack>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

FirewallDialog::~FirewallDialog()
{
    delete m_dialog;
}

FirewallDialog::FirewallDialog(QWidget *parent) :
    BaseObjectDialog(parent)
{
    m_dialog = new Ui::FirewallDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

void FirewallDialog::getHelpName(QString *str)
{
    *str = "FirewallDialog";
}

void FirewallDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Firewall *s = dynamic_cast<Firewall*>(obj);
    assert(s!=NULL);

    init = true;

    QString platform = obj->getStr("platform").c_str();
/* fill in platform */
    setPlatform(m_dialog->platform, platform);

    fillVersion();

/* fill in host OS  */
    setHostOS(m_dialog->hostOS, platform, obj->getStr("host_OS").c_str());

/* ---------------- */
    updateTimeStamps();

    Management *mgmt=s->getManagementObject();
    assert(mgmt!=NULL);

//    FWOptions  *opt =s->getOptionsObject();

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    m_dialog->inactive->setChecked(s->getInactive());

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->platform->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->platform);

    m_dialog->version->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->version);

    m_dialog->hostOS->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->hostOS);

    m_dialog->fwAdvanced->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->fwAdvanced);

    m_dialog->osAdvanced->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->osAdvanced);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

//    snmpCommunity->setEnabled(!o->isReadOnly());
//    setDisabledPalette(snmpCommunity);

    m_dialog->inactive->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->inactive);


    init=false;
}

void FirewallDialog::updateTimeStamps()
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

/* fill in version */
void FirewallDialog::fillVersion()
{
    m_dialog->version->clear();

    list<QStringPair> vl;
    getVersionsForPlatform(readPlatform(m_dialog->platform), vl);
    QString v = obj->getStr("version").c_str();
    int cp = 0;
    for (list<QStringPair>::iterator i1=vl.begin(); i1!=vl.end(); i1++,cp++)
    {
        if (fwbdebug)
            qDebug(QString("Adding version %1").arg(
                       i1->second).toAscii().constData());

        m_dialog->version->addItem( i1->second );
        if ( v == i1->first ) m_dialog->version->setCurrentIndex( cp );
    }
}

void FirewallDialog::saveVersion(FWObject *o)
{
    QString pl = readPlatform(m_dialog->platform);

    list<QStringPair> vl;
    getVersionsForPlatform( pl.toLatin1().constData(), vl);
    QString v = m_dialog->version->currentText();
    list<QStringPair>::iterator li =
        std::find_if(vl.begin(),vl.end(),findSecondInQStringPair(v));
    if (li!=vl.end())
        o->setStr("version", li->first.toLatin1().constData() );
}

void FirewallDialog::platformChanged()
{
    fillVersion();
    changed();

    QString platform = readPlatform(m_dialog->platform);

    setHostOS( m_dialog->hostOS, platform, "");

    QString pl = readPlatform(m_dialog->platform);
    m_dialog->fwAdvanced->setEnabled( pl!="unknown" );
}

void FirewallDialog::hostOSChanged()
{
    changed();
    QString ho = readHostOS(m_dialog->hostOS);
    m_dialog->osAdvanced->setEnabled( ho!="unknown_os" );
}

void FirewallDialog::validate(bool *res)
{
    *res = true;

    QString platform = readPlatform(m_dialog->platform);
    if (platform.isEmpty())
    {
        *res=false;
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Platform setting can not be empty"),
            tr("&Continue"), 0, 0,
            0 );
        return;
    }

    QString ho = readHostOS(m_dialog->hostOS);
    if (ho.isEmpty())
    {
        *res=false;
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Host OS setting can not be empty"),
            tr("&Continue"), 0, 0,
            0 );
        return;
    }

    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *res = false;
        return;
    }
}



void FirewallDialog::applyChanges()
{
    FWCmdChange* cmd = new FWCmdChange(m_project, obj);

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();

    Firewall *s = dynamic_cast<Firewall*>(new_state);
    Management *mgmt = s->getManagementObject();
    assert(mgmt!=NULL);

    string old_name = obj->getName();
    string new_name = string(m_dialog->obj_name->text().toUtf8().constData());
    string old_platform = obj->getStr("platform");
    string old_host_os = obj->getStr("host_OS");
    string old_version = obj->getStr("version");

    new_state->setName(new_name);
    new_state->setComment(string(m_dialog->comment->toPlainText().toUtf8().constData()));

    string new_platform = readPlatform(m_dialog->platform).toLatin1().constData();
    new_state->setStr("platform", new_platform );

    string new_host_os = readHostOS(m_dialog->hostOS).toLatin1().constData();
    new_state->setStr("host_OS", new_host_os);

    s->setInactive(m_dialog->inactive->isChecked());

    saveVersion(new_state);

    string new_version = new_state->getStr("version");

    if (old_platform!=new_platform)
    {
        if (fwbdebug)
            qDebug("FirewallDialog::applyChanges() platform has changed to %s - "
                   "clear option 'compiler'", new_platform.c_str());
        FWOptions  *opt =s->getOptionsObject();
        opt->setStr("compiler", "");

        // Set default options for the new platform
        Resources::setDefaultTargetOptions(new_platform, s);
    }

    if (old_host_os!=new_host_os)
    {
        if (fwbdebug)
            qDebug("FirewallDialog::applyChanges() host_OS has changed to %s",
                   new_host_os.c_str());
        // Set default options for the new host os
        Resources::setDefaultTargetOptions(new_host_os, s);
    }

    if (old_platform!=new_platform || old_host_os!=new_host_os ||
        old_name!=new_name || old_version!=new_version)
    {
        if (fwbdebug)
            qDebug("FirewallDialog::applyChanges() scheduling call "
                   "to reopenFirewall()");
        m_project->registerRuleSetRedrawRequest();
    }

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd);
    }

    updateTimeStamps();
}

void FirewallDialog::openFWDialog()
{
    try
    {
        QWidget *w = DialogFactory::createFWDialog(this,obj);
        if (w==NULL)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=NULL);
        d->exec();
        delete d;
    }
    catch (FWException &ex)
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("FWBuilder API error: %1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return;
    }
}


void FirewallDialog::openOSDialog()
{
    try
    {
        QWidget *w = DialogFactory::createOSDialog(this,obj);
        if (w==NULL)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=NULL);
        d->exec();
        delete d;
    }
    catch (FWException &ex)
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("FWBuilder API error: %1").arg(ex.toString().c_str()),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return;
    }
}


