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

#include <memory>

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
#include <QtDebug>

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
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

void FirewallDialog::loadFWObject(FWObject *o)
{
    try
    {
        obj = o;
        Firewall *s = dynamic_cast<Firewall*>(obj);
        assert(s!=nullptr);

        init = true;

        QString platform = obj->getStr("platform").c_str();
/* fill in platform */
        setPlatform(m_dialog->platform, platform);

        fillVersion();

/* fill in host OS  */
        setHostOS(m_dialog->hostOS, platform, obj->getStr("host_OS").c_str());

/* ---------------- */
        updateTimeStamps();

#ifndef NDEBUG
        Management *mgmt=s->getManagementObject();
        assert(mgmt!=nullptr);
#endif

//    FWOptions  *opt =s->getOptionsObject();

        m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );

        m_dialog->commentKeywords->loadFWObject(o);

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

//    snmpCommunity->setEnabled(!o->isReadOnly());
//    setDisabledPalette(snmpCommunity);

        m_dialog->inactive->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->inactive);

    } catch (FWException &ex)
    {
        qDebug() << "Caught FWException:" << ex.toString().c_str();
    }

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
    bool found_version = false;
    int cp = 0;
    for (list<QStringPair>::iterator i1=vl.begin(); i1!=vl.end(); i1++,cp++)
    {
        if (fwbdebug)
            qDebug() << "Adding version " << i1->second;

        m_dialog->version->addItem( i1->second );
        if ( v == i1->first ) { found_version = true; m_dialog->version->setCurrentIndex( cp ); }
    }
    if (!found_version && !obj->isReadOnly())
    {
        // version configured in the object does not match any valid
        // version for this platform.
        obj->setStr("version", vl.front().first.toStdString());
        fillVersion();
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
    if (fwbdebug)
        qDebug() << "FirewallDialog::platformChanged()";
    fillVersion();
    QString platform = readPlatform(m_dialog->platform);
    setHostOS( m_dialog->hostOS, platform, "");
    QString pl = readPlatform(m_dialog->platform);
    m_dialog->fwAdvanced->setEnabled( pl!="unknown" );
    //changed();
}

void FirewallDialog::hostOSChanged()
{
    if (fwbdebug)
        qDebug() << "FirewallDialog::hostOSChanged()";
    QString ho = readHostOS(m_dialog->hostOS);
    m_dialog->osAdvanced->setEnabled( ho!="unknown_os" );
    //changed();
}

void FirewallDialog::validate(bool *res)
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
                tr("Character \"/\" is not allowed in firewall object name"),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
            blockSignals(false);
        }
        return;
    }
}



void FirewallDialog::applyChanges()
{
    if (fwbdebug)
        qDebug() << "FirewallDialog::applyChanges()";

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
        /*
         * when we open this warning dialog, FirewallDialog class
         * loses focus and obj_name lineEdit widget sends signal
         * "editingfinished" again.  To the user this looks like the
         * warning dialog popped up twice (in fact two copies of the
         * same warning dialog appear at the same time, one exactly on
         * top of another). To avoid this, block signals for the
         * duration while we show the dialog. Note that documentation
         * does not mention that QObject::blockSignals() affects not
         * only the widget but all its children, but it seems to work
         * that way. Tested with Qt 4.6.1. See #1171
         */
        blockSignals(true);
        autorename_chidren = (QMessageBox::warning(
                                  this,"Firewall Builder", dialog_txt,
                                  tr("&Yes"), tr("&No"), QString::null,
                                  0, 1 )==0 );
        blockSignals(false);
    }

    if (fwbdebug)
        qDebug() << "Sending FWCmdChange  autorename_chidren="
                 << autorename_chidren;

    std::unique_ptr<FWCmdChange> cmd(
        new FWCmdChange(m_project, obj, "", autorename_chidren));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();

    Firewall *s = dynamic_cast<Firewall*>(new_state);

#ifndef NDEBUG
    Management *mgmt = s->getManagementObject();
    assert(mgmt!=nullptr);
#endif

    string old_name = obj->getName();
    string new_name = string(m_dialog->obj_name->text().toUtf8().constData());
    string old_platform = obj->getStr("platform");
    string old_host_os = obj->getStr("host_OS");
    string old_version = obj->getStr("version");

    new_state->setName(new_name);
    m_dialog->commentKeywords->applyChanges(new_state);

    s->setInactive(m_dialog->inactive->isChecked());

    saveVersion(new_state);

    string new_version = new_state->getStr("version");

    string new_platform = readPlatform(m_dialog->platform).toLatin1().constData();
    if (new_platform.empty()) new_platform = "unknown";
    new_state->setStr("platform", new_platform );

    if (old_platform!=new_platform)
    {
        if (fwbdebug)
            qDebug() << "FirewallDialog::applyChanges() platform has changed"
                     << old_platform.c_str() << "->" << new_platform.c_str()
                     << "clearing option 'compiler'";
        platformChanged();
        FWOptions  *opt =s->getOptionsObject();
        opt->setStr("compiler", "");

        // Set default options for the new platform
        Resources::setDefaultTargetOptions(new_platform, s);
    }

    string new_host_os = readHostOS(m_dialog->hostOS).toLatin1().constData();
    if (new_host_os.empty()) new_host_os = "unknown_os";
    new_state->setStr("host_OS", new_host_os);

    if (old_host_os!=new_host_os)
    {
        if (fwbdebug)
            qDebug() << "FirewallDialog::applyChanges() host_OS has changed"
                     << old_host_os.c_str() << "->" << new_host_os.c_str();
        hostOSChanged();
        // Set default options for the new host os
        Resources::setDefaultTargetOptions(new_host_os, s);
    }

    if (new_platform.empty())
    {
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Platform setting can not be empty"),
            tr("&Continue"), nullptr, nullptr,
            0 );
        return;
    }

    if (new_host_os.empty())
    {
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Host OS setting can not be empty"),
            tr("&Continue"), nullptr, nullptr,
            0 );
        return;
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
        m_project->undoStack->push(cmd.release());
    }

    updateTimeStamps();
}

void FirewallDialog::openFWDialog()
{
    try
    {
        QWidget *w = DialogFactory::createFWDialog(mw, obj);
        if (w==nullptr)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=nullptr);
        d->setWindowModality(Qt::WindowModal);
//        d->open();
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
        QWidget *w = DialogFactory::createOSDialog(mw, obj);
        if (w==nullptr)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=nullptr);
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


