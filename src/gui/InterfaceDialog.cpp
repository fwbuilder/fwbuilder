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

#include "InterfaceDialog.h"
#include "DialogFactory.h"
#include "ProjectPanel.h"
#include "FWWindow.h"
#include "FWBSettings.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/InterfaceData.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
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
#include <QtDebug>
#include <QTime>

using namespace std;
using namespace libfwbuilder;

InterfaceDialog::InterfaceDialog(QWidget *parent) :
        BaseObjectDialog(parent)
{
    m_dialog = new Ui::InterfaceDialog_q;
    m_dialog->setupUi(this);
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

void InterfaceDialog::getHelpName(QString *str)
{
    *str = "InterfaceDialog";
}

void InterfaceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Interface *s = dynamic_cast<Interface*>(obj);
    assert(s!=NULL);

    init = true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->label->setText( QString::fromUtf8(s->getLabel().c_str()) );

    m_dialog->regular->setChecked(! s->isDyn() &&
                                  ! s->isUnnumbered() &&
                                  ! s->isBridgePort() );

    m_dialog->dynamic->setChecked( s->isDyn() );
    m_dialog->unnumbered->setChecked( s->isUnnumbered() );
    m_dialog->dedicated_failover->setChecked( s->isDedicatedFailover() );

    m_dialog->management->setChecked( s->isManagement() );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

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

    m_dialog->management->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->management);

    m_dialog->unprotected->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->unprotected);

    m_dialog->dedicated_failover->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->dedicated_failover);

    m_dialog->seclevel->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->seclevel);

    m_dialog->netzone->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netzone);

    if (s->isBridgePort())
    {
        m_dialog->regular->hide();
        m_dialog->dynamic->hide();
        m_dialog->unnumbered->hide();
        m_dialog->management->hide();
        m_dialog->unprotected->hide();
        m_dialog->dedicated_failover->hide();
        m_dialog->bridge_port_label->show();
    } else
    {
        m_dialog->regular->show();
        m_dialog->dynamic->show();
        m_dialog->unnumbered->show();
        m_dialog->management->show();
        m_dialog->unprotected->show();
        m_dialog->dedicated_failover->show();
        m_dialog->bridge_port_label->hide();
    }

    FWObject *f = s->getParentHost();

/* if parent is a host, hide firewall related settings */
    if (Host::isA(f))
    {
        m_dialog->management->setEnabled(false);
        m_dialog->unprotected->setEnabled(false);
        m_dialog->dedicated_failover->setEnabled(false);
        m_dialog->seclevel->setEnabled(false);
        m_dialog->seclevelLabel->setEnabled(false);
        m_dialog->netzone->setEnabled(false);
        m_dialog->netzoneLabel->setEnabled(false);
        // Can;t let user try to open "advanced interface settings"
        // dialog because Host does not have "platform" and "host_OS"
        // attributes but that dialog depends on them.
        m_dialog->advancedconfig->setEnabled(false);
    }

    bool supports_security_levels = false;
    bool supports_network_zones   = false;
    bool supports_unprotected     = false;
    bool supports_advanced_ifaces = false;

    try 
    {
        // platform specific
        supports_security_levels =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "security_levels");
        supports_network_zones =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "network_zones");
        supports_unprotected =
            Resources::getTargetCapabilityBool(f->getStr("platform"), "unprotected_interfaces");

        // OS specific
        supports_advanced_ifaces =
            Resources::getTargetCapabilityBool(f->getStr("host_OS"),
                                               "supports_subinterfaces");
        // disable advanced options dialog if this is main interface of a cluster
        if (Cluster::isA(s->getParent()))
            supports_advanced_ifaces = false;

        if (s->isDedicatedFailover())
        {
            supports_network_zones = false;
        }

    } catch (FWException &ex)  { }

/* if parent is a firewall or a fw cluster, it is more complex ... */
    if (Firewall::isA(f) || Cluster::isA(f))
    {
        if (supports_security_levels)
        {
            m_dialog->seclevel->setEnabled(true);
            m_dialog->seclevelLabel->setEnabled(true);
            m_dialog->seclevel->setValue( obj->getInt("security_level") );
        } else
        {
            m_dialog->seclevel->setEnabled(false);
            m_dialog->seclevelLabel->setEnabled(false);
            m_dialog->seclevel->setValue(0);
        }

        if (supports_unprotected)
        {
            m_dialog->unprotected->setEnabled(true);
            m_dialog->unprotected->setChecked( obj->getBool("unprotected") );
        } else
        {
            m_dialog->unprotected->setEnabled(false);
        }

        if (supports_advanced_ifaces)
        {
            m_dialog->advancedconfig->setEnabled(!o->isReadOnly());
        } else
        {
            m_dialog->advancedconfig->setEnabled(false);
        }

        // disable interface options group if this is main interface
        // of a cluster. This applies to subinterfaces as
        // well. Current implementation can not generate configuration
        // code for interfaces and subinterfaces of member firewalls
        // from cluster interface or subinterface objects
        m_dialog->interfaceOptionsGroup->setEnabled(
            !Cluster::isA(s->getParentHost()));

        if (supports_network_zones)
        {
            m_dialog->netzone->setEnabled(true);
            m_dialog->netzoneLabel->setEnabled(true);

            netzoneObjectIDs.clear();
            netzoneObjectNos.clear();

            QStringList netzoneObjectNames;

            int n=0;

            netzoneObjectIDs[FWObjectDatabase::ANY_ADDRESS_ID] = n;
            netzoneObjectNos[n] = FWObjectDatabase::ANY_ADDRESS_ID;
            netzoneObjectNames.push_back(" Any ");

            ++n;

/* TODO: try to make this widget show object with appropriate icon */

            list<FWObject*> libs = m_project->db()->getByType( Library::TYPENAME );
            for (list<FWObject*>::iterator l=libs.begin(); l!=libs.end(); ++l)
            {
                FWObject *library= *l;
                FWObject *o1,*o2;

                if ( library->getId()==FWObjectDatabase::DELETED_OBJECTS_ID ) continue;

                o1=library->findObjectByName(ObjectGroup::TYPENAME,"Objects");
                assert(o1!=NULL);
                o2=o1->findObjectByName(ObjectGroup::TYPENAME,"Groups");
                if (o2==NULL)
                {
                    if (fwbdebug)
                        qDebug("InterfaceDialog::loadFWObject  missing Groups group in %s", FWObjectDatabase::getStringId(o1->getId()).c_str());
                    continue;
                }
//                assert(o2!=NULL);

                for (FWObject::iterator i=o2->begin(); i!=o2->end(); ++i)
                {
                    netzoneObjectIDs[(*i)->getId()] = n;
                    netzoneObjectNos[n] =(*i)->getId();
                    netzoneObjectNames.push_back(
                        tr("Group: ")+ (*i)->getName().c_str() );
                    ++n;
                }

                o2=o1->findObjectByName(ObjectGroup::TYPENAME,"Networks");
                if (o2==NULL)
                {
                    if (fwbdebug)
                        qDebug("InterfaceDialog::loadFWObject  missing Networks group in %s",
                               FWObjectDatabase::getStringId(o1->getId()).c_str());
                    continue;
                }
//                assert(o2!=NULL);

                for (FWObject::iterator i1=o2->begin(); i1!=o2->end(); ++i1)
                {
                    netzoneObjectIDs[(*i1)->getId()] = n;
                    netzoneObjectNos[n] = (*i1)->getId();
                    netzoneObjectNames.push_back(
                        tr("Network: ")+ (*i1)->getName().c_str() );
                    ++n;
                }
            }

            m_dialog->netzone->clear();
            m_dialog->netzone->addItems( netzoneObjectNames );

            int id = FWObjectDatabase::getIntId(obj->getStr("network_zone"));
            if (id==-1) id = FWObjectDatabase::ANY_ADDRESS_ID;  // any network
            m_dialog->netzone->setCurrentIndex( netzoneObjectIDs[id] );
        }
        else
        {
            m_dialog->netzone->setEnabled(false);
            m_dialog->netzoneLabel->setEnabled(false);
            m_dialog->netzone->clear();
        }
    }

    //apply->setEnabled( false );

    init = false;
}

void InterfaceDialog::validate(bool *res)
{
    *res = true;
    if (!isTreeReadWrite(this, obj)) *res = false;

    QString obj_name = m_dialog->obj_name->text();

    // validateName checks for name duplicates
    if (!validateName(this, obj, obj_name))
    {
        *res = false;
        return;
    }

    if (obj_name.indexOf(' ') != -1 || obj_name.indexOf('-') != -1)
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            tr("Interface name can not contain white space and '-'"),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        *res = false;
        return;
    }

    FWObject *f = Interface::cast(obj)->getParentHost();

    Resources* os_res = Resources::os_res[f->getStr("host_OS")];
    string os_family = f->getStr("host_OS");
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties *int_prop =
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);
    QString err;
    if ( ! int_prop->validateInterface(obj->getParent(), obj_name, err))
    {
        *res = false;
        QMessageBox::critical(
            this,"Firewall Builder",
            err,
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
    }

    delete int_prop;
}

void InterfaceDialog::applyChanges()
{
    Interface *s = Interface::cast(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    string oldlabel=s->getLabel();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    s->setLabel( string(m_dialog->label->text().toUtf8().constData()) );
    s->setDyn( m_dialog->dynamic->isChecked() );
    s->setUnnumbered( m_dialog->unnumbered->isChecked() );
    s->setDedicatedFailover( m_dialog->dedicated_failover->isChecked() );

    FWObject *f = s->getParentHost();
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


    if (Firewall::isA( f ) || Cluster::isA( f ))
    {
        if (supports_security_levels)
            obj->setInt("security_level", m_dialog->seclevel->value() );

        if (supports_unprotected)
            obj->setBool("unprotected",  m_dialog->unprotected->isChecked() );

        if (supports_network_zones)
            obj->setStr("network_zone",
                        FWObjectDatabase::getStringId(
                            netzoneObjectNos[
                                m_dialog->netzone->currentIndex() ]));

        s->setManagement( m_dialog->management->isChecked() );
    }

    m_project->updateObjName(obj,
                      QString::fromUtf8(oldname.c_str()),
                      QString::fromUtf8(oldlabel.c_str()));

    // ticket #328: automatically assign vlan id to interface based on
    // interface name
    mw->activeProject()->m_panel->om->guessSubInterfaceTypeAndAttributes(s);

    BaseObjectDialog::applyChanges();
}

void InterfaceDialog::discardChanges()
{
    loadFWObject(obj);
}

void InterfaceDialog::openIfaceDialog()
{
    if (isDataChanged()) applyChanges();

    try
    {
        QWidget *w = DialogFactory::createIfaceDialog(this, obj);
        if (w==NULL)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=NULL);

        if (d->exec() == QDialog::Accepted)
        {
            // modal dialog, dialog saves data into the object
            
            // update object tree (if interface type has changed, the
            // object properties summary text may have to change too)
            mw->activeProject()->updateObjectInTree(obj, true);
            changed();
            BaseObjectDialog::applyChanges();
        }
        delete w;
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
