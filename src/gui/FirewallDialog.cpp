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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "ProjectPanel.h"

#include "FWBTree.h"
#include "FirewallDialog.h"
#include "DialogFactory.h"
#include "FWWindow.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Resources.h"
#include "FWBSettings.h"

#include <vector>
#include <map>

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

#include <iostream>

using namespace std;
using namespace libfwbuilder;

FirewallDialog::~FirewallDialog()
{
    delete m_dialog;
}

FirewallDialog::FirewallDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::FirewallDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

void FirewallDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Firewall *s = dynamic_cast<Firewall*>(obj);
    assert(s!=NULL);

    init=true;

/* fill in platform */
    setPlatform(m_dialog->platform, obj->getStr("platform").c_str() );

    fillVersion();

/* fill in host OS  */
    setHostOS(m_dialog->hostOS, obj->getStr("host_OS").c_str() );

/* ---------------- */

    Management *mgmt=s->getManagementObject();
    assert(mgmt!=NULL);

//    FWOptions  *opt =s->getOptionsObject();

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
//    snmpCommunity->setText( mgmt->getSNMPManagement()->getReadCommunity().c_str() );

    //comment->setTextFormat(QTextEdit::PlainText);

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    m_dialog->inactive->setChecked(s->getInactive());

    //apply->setEnabled( false );


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

/* fill in version */
void FirewallDialog::fillVersion()
{
    m_dialog->version->clear();

    list<QStringPair> vl=getVersionsForPlatform( readPlatform(m_dialog->platform) );
    QString            v=obj->getStr("version").c_str();
    int cp=0;
    for (list<QStringPair>::iterator i1=vl.begin(); i1!=vl.end(); i1++,cp++)
    {
        if (fwbdebug)
            qDebug(QString("Adding version %1").arg(i1->second).toAscii().constData());

        m_dialog->version->addItem( i1->second );
        if ( v == i1->first ) m_dialog->version->setCurrentIndex( cp );
    }
}

void FirewallDialog::saveVersion()
{
    QString pl = readPlatform(m_dialog->platform);

    list<QStringPair> vl=getVersionsForPlatform( pl.toLatin1().constData() );
    QString           v = m_dialog->version->currentText();
    list<QStringPair>::iterator li =
        std::find_if(vl.begin(),vl.end(),findSecondInQStringPair(v));
    if (li!=vl.end())
        obj->setStr("version", li->first.toLatin1().constData() );

#if 0
    int cp=0;
    for (list<QStringPair>::iterator i1=vl.begin(); i1!=vl.end(); i1++,cp++)
    {
        if ( v == i1.data() )
        {
            obj->setStr("version", i1.key().toLatin1().constData() );
            break;
        }
    }
#endif
}

void FirewallDialog::platformChanged()
{
    fillVersion();
    changed();

    QString so=Resources::platform_res[readPlatform(m_dialog->platform).toLatin1().constData()]->getResourceStr("/FWBuilderResources/Target/supported_os").c_str();
    if (so.isEmpty()) return;

    QString ho=so.section(",",0);
    setHostOS( m_dialog->hostOS, ho.toLatin1().constData() );

    QString pl = readPlatform(m_dialog->platform);
    m_dialog->fwAdvanced->setEnabled( pl!="unknown" );
}

void FirewallDialog::hostOSChanged()
{
    changed();
    QString ho = readHostOS(m_dialog->hostOS);
    m_dialog->osAdvanced->setEnabled( ho!="unknown_os" );
}

void FirewallDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void FirewallDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void FirewallDialog::isChanged(bool*)
{
    if (fwbdebug)
        qDebug("FirewallDialog::isChanged");
    //*res=(!init && apply->isEnabled());
}

void FirewallDialog::libChanged()
{
    changed();
}

void FirewallDialog::applyChanges()
{
    Firewall *s = dynamic_cast<Firewall*>(obj);
    Management *mgmt=s->getManagementObject();
    assert(mgmt!=NULL);

//    FWOptions  *opt =s->getOptionsObject();

    assert(s!=NULL);

    string oldname=obj->getName();
    string newname=string(m_dialog->obj_name->text().toUtf8().constData());
    string oldplatform=obj->getStr("platform");

    string oldVer=obj->getStr("version");

    obj->setName( newname );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
//    mgmt->getSNMPManagement()->setReadCommunity( snmpCommunity->text().toLatin1().constData() );

    string pl = readPlatform(m_dialog->platform).toLatin1().constData();
    obj->setStr("platform", pl );

    obj->setStr("host_OS", readHostOS(m_dialog->hostOS).toLatin1().constData() );

    s->setInactive(m_dialog->inactive->isChecked());

    saveVersion();

    string newVer=obj->getStr("version");

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    if (oldplatform!=pl || oldname!=newname || oldVer!=newVer)
    {
        if (fwbdebug)
            qDebug("FirewallDialog::applyChanges() scheduling call to reopenFirewall()");
        //mw->reopenFirewall();
        //QTimer::singleShot( 0, mw, SLOT(reopenFirewall()) );
        mw->scheduleRuleSetRedraw();
    }

    if (oldplatform!=pl)
    {
        if (fwbdebug)
            qDebug("FirewallDialog::applyChanges() platform has changed - clear option 'compiler'");
        Firewall *s = Firewall::cast(obj);
        assert(s!=NULL);
        FWOptions  *opt =s->getOptionsObject();
        opt->setStr("compiler","");
    }

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(s);

}

void FirewallDialog::discardChanges()
{
    loadFWObject(obj);
}

void FirewallDialog::openFWDialog()
{
    /*if (apply->isEnabled())*/ applyChanges();

    if (obj->getStr("version").empty()) saveVersion();

    try
    {
        QWidget *w = DialogFactory::createFWDialog(this,obj);
        if (w==NULL)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=NULL);

        d->exec();   // modal dialog, dialog saves data into the object
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
    /*if (apply->isEnabled())*/ applyChanges();

    try
    {
        QWidget *w = DialogFactory::createOSDialog(this,obj);
        if (w==NULL)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=NULL);

        d->exec();   // modal dialog, dialog saves data into the object
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

/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void FirewallDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

