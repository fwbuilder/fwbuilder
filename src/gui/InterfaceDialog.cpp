/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: InterfaceDialog.cpp,v 1.30 2007/05/10 01:35:52 vkurland Exp $

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

#include "InterfaceDialog.h"
#include "ProjectPanel.h"
#include "FWWindow.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Resources.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include "FWBSettings.h"

using namespace std;
using namespace libfwbuilder;

InterfaceDialog::InterfaceDialog(ProjectPanel *project, QWidget *parent) :
        QWidget(parent), m_project(project)
{
    m_dialog = new Ui::InterfaceDialog_q;
    m_dialog->setupUi(this);
    setFont(st->getUiFont());
/*
    seclevel->hide();    seclevelLabel->hide();
    netzone->hide();     netzoneLabel->hide();
*/
    obj=NULL;
}

InterfaceDialog::~InterfaceDialog()
{
    delete m_dialog;
}

void InterfaceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Interface *s = dynamic_cast<Interface*>(obj);
    assert(s!=NULL);

    init=true;

    fillLibraries(m_dialog->libs,obj);

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->label->setText( QString::fromUtf8(s->getLabel().c_str()) );

    m_dialog->regular->setChecked( ! s->isDyn() &&
                         ! s->isUnnumbered() &&
                         ! s->isBridgePort() );
    m_dialog->dynamic->setChecked( s->isDyn() );
    m_dialog->unnumbered->setChecked( s->isUnnumbered() );
    m_dialog->bridgeport->setChecked( s->isBridgePort() );

    m_dialog->management->setChecked( s->isManagement() );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

/* interface should always belong to the host or firewall so we can't
 * move them from library to library */
    m_dialog->libs->setEnabled( false );

    FWObject *f=obj->getParent();

/* if parent is a host, hide checkbox 'external', security level and netzone */
    if (Host::isA( f ))
    {
        m_dialog->ext->setEnabled( false );
        m_dialog->ext->hide();
        m_dialog->seclevel->hide();
        m_dialog->seclevelLabel->hide();
        m_dialog->netzone->hide();
        m_dialog->netzoneLabel->hide();
    }

    bool supports_security_levels = false;
    bool supports_network_zones   = false;
    bool supports_unprotected     = false;

    try  {
        supports_security_levels =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "security_levels");
        supports_network_zones =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "network_zones");
        supports_unprotected =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "unprotected_interfaces");
    } catch (FWException &ex)  { }

/* if parent is a firewall, it is more complex ... */
    if (Firewall::isA( f ))
    {
        if (supports_security_levels)
        {
            m_dialog->seclevel->show();
            m_dialog->seclevelLabel->show();
            m_dialog->ext->hide();
            m_dialog->seclevel->setValue( obj->getInt("security_level") );
        } else
        {
            m_dialog->seclevel->hide();
            m_dialog->seclevelLabel->hide();
            m_dialog->ext->show();
            m_dialog->ext->setChecked( obj->getInt("security_level")==0 );
        }

        if (supports_unprotected)
        {
            m_dialog->unprotected->show();
            m_dialog->unprotected->setChecked( obj->getBool("unprotected") );
        } else
        {
            m_dialog->unprotected->hide();
        }

        if (supports_network_zones)
        {
            m_dialog->netzone->show();
            m_dialog->netzoneLabel->show();

            netzoneObjectIDs.clear();
            netzoneObjectNos.clear();

            QStringList netzoneObjectNames;

            int n=0;

            netzoneObjectIDs["sysid0"]=n;
            netzoneObjectNos[n]="sysid0";
            netzoneObjectNames.push_back(" Any ");

            ++n;

/* TODO: try to make this widget show object with appropriate icon */

            list<FWObject*> libs=mw->db()->getByType( Library::TYPENAME );
            for (list<FWObject*>::iterator l=libs.begin(); l!=libs.end(); ++l)
            {
                FWObject *library= *l;
                FWObject *o1,*o2;

                if ( library->getId()==DELETED_LIB ) continue;

                o1=library->findObjectByName(ObjectGroup::TYPENAME,"Objects");
                assert(o1!=NULL);
                o2=o1->findObjectByName(ObjectGroup::TYPENAME,"Groups");
                if (o2==NULL)
                {
                    if (fwbdebug)
                        qDebug("InterfaceDialog::loadFWObject  missing Groups group in %s",o1->getId().c_str());
                    continue;
                }
//                assert(o2!=NULL);

                for (FWObject::iterator i=o2->begin(); i!=o2->end(); ++i)
                {
                    netzoneObjectIDs[ (*i)->getId().c_str() ]=n;
                    netzoneObjectNos[n]= (*i)->getId().c_str();
                    netzoneObjectNames.push_back(
                        tr("Group: ")+ (*i)->getName().c_str() );
                    ++n;
                }

                o2=o1->findObjectByName(ObjectGroup::TYPENAME,"Networks");
                if (o2==NULL)
                {
                    if (fwbdebug)
                        qDebug("InterfaceDialog::loadFWObject  missing Networks group in %s",o1->getId().c_str());
                    continue;
                }
//                assert(o2!=NULL);

                for (FWObject::iterator i1=o2->begin(); i1!=o2->end(); ++i1)
                {
                    netzoneObjectIDs[ (*i1)->getId().c_str() ]=n;
                    netzoneObjectNos[n]= (*i1)->getId().c_str();
                    netzoneObjectNames.push_back(
                        tr("Network: ")+ (*i1)->getName().c_str() );
                    ++n;
                }
            }

            m_dialog->netzone->clear();
            m_dialog->netzone->addItems( netzoneObjectNames );

            QString id=obj->getStr("network_zone").c_str();
            if (id=="") id="sysid0";  // any network
            m_dialog->netzone->setCurrentIndex( netzoneObjectIDs[id] );
        }
        else
        {
            m_dialog->netzone->hide();
            m_dialog->netzoneLabel->hide();
        }
    }

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->libs->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->libs);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

    m_dialog->label->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->label);

    m_dialog->regular->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->regular);

    m_dialog->dynamic->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->dynamic);

    m_dialog->unnumbered->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->unnumbered);

    m_dialog->bridgeport->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->bridgeport);

    m_dialog->management->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->management);

    m_dialog->unprotected->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->unprotected);

    m_dialog->ext->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ext);

    m_dialog->seclevel->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->seclevel);

    m_dialog->netzone->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netzone);


    init=false;
}

void InterfaceDialog::changed()
{
    if (fwbdebug)
        qDebug("InterfaceDialog::changed()");

    //apply->setEnabled( true );
    emit changed_sign();
}

void InterfaceDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) *res=false;
    if (!validateName(this,obj,m_dialog->obj_name->text())) *res=false;
}

void InterfaceDialog::isChanged(bool *res)
{
    //*res=(!init && apply->isEnabled());
}

void InterfaceDialog::libChanged()
{
    changed();
}

void InterfaceDialog::applyChanges()
{
    Interface *s = dynamic_cast<Interface*>(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    string oldlabel=s->getLabel();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    s->setLabel( string(m_dialog->label->text().toUtf8().constData()) );
    s->setDyn( m_dialog->dynamic->isChecked() );
    s->setUnnumbered( m_dialog->unnumbered->isChecked() );
    s->setBridgePort( m_dialog->bridgeport->isChecked() );

    FWObject *f = obj->getParent();
    bool supports_security_levels = false;
    bool supports_network_zones   = false;
    bool supports_unprotected     = false;

    try  {
        supports_security_levels=
            Resources::getTargetCapabilityBool(f->getStr("platform"), "security_levels");
        supports_network_zones=
            Resources::getTargetCapabilityBool(f->getStr("platform"), "network_zones");
        supports_unprotected =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "unprotected_interfaces");
    } catch (FWException &ex)  { }


    if (Firewall::isA( f ))
    {
        if (supports_security_levels)
            obj->setInt("security_level", m_dialog->seclevel->value() );
        else
            obj->setInt("security_level", (m_dialog->ext->isChecked()) ? 0 : 100 );

        if (supports_unprotected)
            obj->setBool("unprotected",  m_dialog->unprotected->isChecked() );

        if (supports_network_zones)
            obj->setStr("network_zone",
                        netzoneObjectNos[ m_dialog->netzone->currentIndex() ].toLatin1().constData() );

        s->setManagement( m_dialog->management->isChecked() );

    }

    mw->updateObjName(obj,
                      QString::fromUtf8(oldname.c_str()),
                      QString::fromUtf8(oldlabel.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void InterfaceDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void InterfaceDialog::closeEvent(QCloseEvent *e)
{
    if (fwbdebug)
        qDebug("InterfaceDialog::coseEvent  got close event: %p",e);
    emit close_sign(e);
}

