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

#include "InterfaceDialog.h"
#include "DialogFactory.h"
#include "ProjectPanel.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

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
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <memory>

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
#include <QUndoStack>
#include <QApplication>

using namespace std;
using namespace libfwbuilder;

InterfaceDialog::InterfaceDialog(QWidget *parent) :
        BaseObjectDialog(parent)
{
    netzone_manager = new NetworkZoneManager();

    m_dialog = new Ui::InterfaceDialog_q;
    m_dialog->setupUi(this);
/*
    seclevel->hide();    seclevelLabel->hide();
    netzone->hide();     netzoneLabel->hide();
*/
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

InterfaceDialog::~InterfaceDialog()
{
    delete netzone_manager;
    delete m_dialog;
}

void InterfaceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Interface *s = dynamic_cast<Interface*>(obj);
    assert(s!=nullptr);

    init = true;

    if (st->getBool("Objects/Interface/autoconfigureInterfaces"))
    {
        /*
         * guessSubInterfaceTypeAndAttributes() changes some attributes of
         * the object (mostly compensating for missing functions in
         * auto-upgrade scripts but also makes some guesses based on the
         * interface name, such as sets its vlan ID if its name looks like
         * it might be a vlan interface). Since we make changes in the
         * object here, do it before loading it into the dialog so that it
         * does not look like it has changed in applyChanges() even if the
         * user hasn't touched it, which causes new undo command to be
         * created out of nowhere.
         *
         * TODO: better way of course is to call
         * guessSubInterfaceTypeAndAttributes in places where user changes
         * something relevant in the interface to complement their changes
         * and right after the interface has been created.
         */
        FWObject *parent_host = Host::getParentHost(s);
        if (parent_host)
        {
            // parent_host may be nullptr if interface object is located
            // in the Deleted Objects library
            interfaceProperties *int_prop =
                interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                    parent_host);
            int_prop->guessSubInterfaceTypeAndAttributes(s);
            delete int_prop;
        }
    }

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->label->setText( QString::fromUtf8(s->getLabel().c_str()) );

    m_dialog->regular->setChecked(! s->isDyn() &&
                                  ! s->isUnnumbered() &&
                                  ! s->isBridgePort() );

    m_dialog->dynamic->setChecked( s->isDyn() );
    m_dialog->unnumbered->setChecked( s->isUnnumbered() );
    m_dialog->dedicated_failover->setChecked( s->isDedicatedFailover() );

    m_dialog->management->setChecked( s->isManagement() );

    m_dialog->commentKeywords->loadFWObject(s);

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

    FWObject *f = Host::getParentHost(s);
    if (f)
    {
        m_dialog->advancedconfig->setEnabled(true);

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
                Resources::getTargetCapabilityBool(
                    f->getStr("platform"), "security_levels");
            supports_network_zones =
                Resources::getTargetCapabilityBool(
                    f->getStr("platform"), "network_zones");
            supports_unprotected =
                Resources::getTargetCapabilityBool(
                    f->getStr("platform"), "unprotected_interfaces");

            // OS specific
            supports_advanced_ifaces =
                Resources::getTargetCapabilityBool(
                    f->getStr("host_OS"), "supports_advanced_interface_options");

            // disable advanced options dialog if this is main interface of a cluster
            if (Cluster::isA(s->getParent())) supports_advanced_ifaces = false;

        } catch (FWException &ex)  { }

        if (fwbdebug)
            qDebug() << "parent=" << f->getName().c_str()
                     << "Firewall::isA(f)=" << Firewall::isA(f)
                     << "host_OS=" << f->getStr("host_OS").c_str()
                     << "supports_advanced_ifaces=" << supports_advanced_ifaces;


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
            m_dialog->interfaceOptionsGroup->setEnabled(!Cluster::isA(f));

            if (supports_network_zones)
            {
                m_dialog->netzone->setEnabled(true);
                m_dialog->netzoneLabel->setEnabled(true);

                netzone_manager->load(m_project->db());

                int id = FWObjectDatabase::getIntId(obj->getStr("network_zone"));
                if (id==-1) id = 0;

                netzone_manager->packComboBox(m_dialog->netzone, id);
            }
            else
            {
                m_dialog->netzone->setEnabled(false);
                m_dialog->netzoneLabel->setEnabled(false);
                m_dialog->netzone->clear();
            }
        }
    }

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

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

    //apply->setEnabled( false );

    init = false;
}

void InterfaceDialog::validate(bool *res)
{
    *res = true;

    QString obj_name = m_dialog->obj_name->text();

    // validateName checks for name duplicates
    if (!validateName(this, obj, obj_name))
    {
        *res = false;
        return;
    }

    FWObject *parent_host = Host::getParentHost(obj);
    interfaceProperties *int_prop =
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            parent_host);
    QString err;

    /*
     * TODO:
     * See if basicValidateInterfaceName() can be rolled into
     * validateInterface()
     */
    if ( ! int_prop->basicValidateInterfaceName(
             Interface::cast(obj), obj_name, err))
    {
        *res = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(
                this,"Firewall Builder",
                err,
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
            blockSignals(false);
        }
        return;
    }

    // validateInterface() checks validity of vlan inetrface name
    if (st->getBool("Objects/Interface/autoconfigureInterfaces") &&
        ! int_prop->validateInterface(obj->getParent(), obj_name, err))
    {
        /*
         * Here is the annoying part: if user entered interface name
         * that does not pass checks in interfaceProperties, this
         * dialog will pop warning dialog to tell them this every time
         * the dialog loses focus even if they did not change
         * anything. This happens when they click on any other part of
         * the GUI or even switch to another application. For example
         * this happens when user adds subinterface to an interface
         * with intention to make the subint a vlan. The new interface
         * object is created with default name, when they change the
         * name to, say, eth0 (and the parent is eth0), this causes
         * the error message to appear when they switch the focus. If
         * they switch to another application while cursor was in the
         * object name field, the pop-up appears as well.
         */
        *res = false;
        // show warning dialog only if app has focus
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(
                this,"Firewall Builder",
                err,
                tr("&Continue"), QString::null,QString::null,
                0, 1 );
            blockSignals(false);
        }
    }

    delete int_prop;
}

void InterfaceDialog::applyChanges()
{
    bool autorename_children = false;
    list<FWObject*> subinterfaces = obj->getByType(Interface::TYPENAME);
    if (obj->getName() != m_dialog->obj_name->text().toUtf8().constData() &&
        (
            obj->getByType(IPv4::TYPENAME).size() ||
            obj->getByType(IPv6::TYPENAME).size() ||
            obj->getByType(physAddress::TYPENAME).size() ||
            subinterfaces.size()
        )
    )
    {
        QString dialog_txt = tr(
            "The name of the interface '%1' has changed. The program can also "
            "rename IP address objects that belong to this interface, "
            "using standard naming scheme 'host_name:interface_name:ip'. "
            "This makes it easier to distinguish what host or a firewall "
            "given IP address object belongs to when it is used in "
            "the policy or NAT rule. The program also renames MAC address "
            "objects using scheme 'host_name:interface_name:mac'. "
            "Do you want to rename child IP and MAC address objects now? "
            "(If you click 'No', names of all address objects that belong to "
            "interface '%2' will stay the same.)")
            .arg(QString::fromUtf8(obj->getName().c_str()))
            .arg(QString::fromUtf8(obj->getName().c_str()));

        /* see comment about this in FirewallDialog */
        blockSignals(true);
        autorename_children = (QMessageBox::warning(
                                   this, "Firewall Builder", dialog_txt,
                                   tr("&Yes"), tr("&No"), QString::null,
                                   0, 1 )==0 );
        blockSignals(false);
    }

    std::unique_ptr<FWCmdChange> cmd(
        new FWCmdChange(m_project, obj, "", autorename_children));
    FWObject* new_state = cmd->getNewState();

    Interface *intf = Interface::cast(new_state);
    assert(intf!=nullptr);

    string oldname = obj->getName();
    string oldlabel = intf->getLabel();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    intf->setLabel( string(m_dialog->label->text().toUtf8().constData()) );
    intf->setDyn( m_dialog->dynamic->isChecked() );
    intf->setUnnumbered( m_dialog->unnumbered->isChecked() );
    intf->setDedicatedFailover( m_dialog->dedicated_failover->isChecked() );

    // NOTE: new_state is a copy of the interface but it is not attached to
    // the tree and therefore has no parent. Need to use original object obj
    // to get the pointer to the parent firewall.
    FWObject *f = Host::getParentHost(obj);
    //FWObject *f = Interface::cast(obj)->getParentHost();
    bool supports_security_levels = false;
    bool supports_network_zones   = false;
    bool supports_unprotected     = false;

    try 
    {
        supports_security_levels=
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "security_levels");
        supports_network_zones=
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "network_zones");
        supports_unprotected =
            Resources::getTargetCapabilityBool(f->getStr("platform"),
                                               "unprotected_interfaces");
    } catch (FWException &ex)  { }


    if (Firewall::isA( f ) || Cluster::isA( f ))
    {
        if (supports_security_levels)
            new_state->setInt("security_level", m_dialog->seclevel->value() );

        if (supports_unprotected)
            new_state->setBool("unprotected",  m_dialog->unprotected->isChecked() );

        if (supports_network_zones)
        {
            new_state->setStr("network_zone",
                        FWObjectDatabase::getStringId(
                            m_dialog->netzone->itemData(
                                m_dialog->netzone->currentIndex(),
                                Qt::UserRole).toInt()));

            // new_state->setStr("network_zone",
            //             FWObjectDatabase::getStringId(
            //                 netzone_manager->getNetzoneIdByListIndex(
            //                     m_dialog->netzone->currentIndex() )
            //             )
            // );
        }

        intf->setManagement( m_dialog->management->isChecked() );
    }

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        // Complement changes made by the user with our guesses, but
        // do this only if user changed something.

        if (st->getBool("Objects/Interface/autoconfigureInterfaces"))
        {
            // ticket #328: automatically assign vlan id to interface based on
            // interface name
            FWObject *parent_host = Host::getParentHost(obj);
            interfaceProperties *int_prop =
                interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                    parent_host);
            int_prop->setPerformVlanChecks(true);
            int_prop->guessSubInterfaceTypeAndAttributes(intf);
            delete int_prop;
        }

        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

void InterfaceDialog::openIfaceDialog()
{
    try
    {
        QWidget *w = DialogFactory::createIfaceDialog(this, obj);
        if (w==nullptr)   return;   // some dialogs may not be implemented yet
        QDialog *d=dynamic_cast<QDialog*>(w);
        assert(d!=nullptr);
        d->exec();
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
