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

#include "ObjectEditor.h"

#include <qobject.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qmenu.h>
#include <qlayout.h>
#include <qstackedwidget.h>
#include <qpushbutton.h>

#include "FWWindow.h"
#include "BaseObjectdialog.h"
#include "DialogFactory.h"
#include "FWBTree.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "GroupObjectDialog.h"
#include "ActionsDialog.h"
#include "MetricEditorPanel.h"
#include "CommentEditorPanel.h"
#include "CompilerOutputPanel.h"
#include "ObjectManipulator.h"
#include "Help.h"
#include "events.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

#define OBJTREEVIEW_WIDGET_NAME  "ObjTreeView"



ObjectEditor::ObjectEditor( QWidget *parent):
    QObject(parent), opened(0), current_dialog_idx(-1), current_dialog_name(""),
    parentWidget(dynamic_cast<QStackedWidget*>(parent)),
    applyButton(0),
    helpButton(0),
    m_project(0),
    openedOpt(optNone)
{

#if defined(Q_WS_X11)
/* do something that makes sense only on X11 */

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
/* do something that only works on windows */

#elif defined(Q_OS_MAC)

#endif

    BaseObjectDialog *w;
    w= DialogFactory::createDialog(parent,UserService::TYPENAME);
    stackIds[UserService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[UserService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Policy::TYPENAME);
    stackIds[Policy::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Policy::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,NAT::TYPENAME);
    stackIds[NAT::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[NAT::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Routing::TYPENAME);
    stackIds[Routing::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Routing::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Library::TYPENAME);
    stackIds[Library::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Library::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,IPv4::TYPENAME);
    stackIds[IPv4::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPv4::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,IPv6::TYPENAME);
    stackIds[IPv6::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPv6::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,physAddress::TYPENAME);
    stackIds[physAddress::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[physAddress::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,AddressRange::TYPENAME);
    stackIds[AddressRange::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[AddressRange::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Firewall::TYPENAME);
    stackIds[Firewall::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Firewall::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Cluster::TYPENAME);
    stackIds[Cluster::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Cluster::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent, StateSyncClusterGroup::TYPENAME);
    stackIds[StateSyncClusterGroup::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[StateSyncClusterGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent, FailoverClusterGroup::TYPENAME);
    stackIds[FailoverClusterGroup::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[FailoverClusterGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Host::TYPENAME);
    stackIds[Host::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Host::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Interface::TYPENAME);
    stackIds[Interface::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Interface::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Network::TYPENAME);
    stackIds[Network::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Network::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,NetworkIPv6::TYPENAME);
    stackIds[NetworkIPv6::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[NetworkIPv6::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,CustomService::TYPENAME);
    stackIds[CustomService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[CustomService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,IPService::TYPENAME);
    stackIds[IPService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,ICMPService::TYPENAME);
    stackIds[ICMPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ICMPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,ICMP6Service::TYPENAME);
    stackIds[ICMP6Service::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ICMP6Service::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,TCPService::TYPENAME);
    stackIds[TCPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[TCPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,UDPService::TYPENAME);
    stackIds[UDPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[UDPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,ObjectGroup::TYPENAME);
    stackIds[ObjectGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ObjectGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,ServiceGroup::TYPENAME);
    stackIds[ServiceGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ServiceGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,IntervalGroup::TYPENAME);
    stackIds[IntervalGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[IntervalGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Interval::TYPENAME);
    stackIds[Interval::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Interval::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,Rule::TYPENAME);
    stackIds[Rule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Rule::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,RoutingRule::TYPENAME);
    stackIds[RoutingRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[RoutingRule::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,PolicyRule::TYPENAME);
    stackIds[PolicyRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[PolicyRule::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,NATRule::TYPENAME);
    stackIds[NATRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[NATRule::TYPENAME]] = w;


    w= DialogFactory::createDialog(parent,DNSName::TYPENAME);
    stackIds[DNSName::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[DNSName::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,AddressTable::TYPENAME);
    stackIds[AddressTable::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[AddressTable::TYPENAME]] = w;

    w= DialogFactory::createDialog(parent,TagService::TYPENAME);
    stackIds[TagService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[TagService::TYPENAME]] = w;


    w = new ActionsDialog(parent);
    stackIds[getOptDialogName(optAction)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optAction)]] = w;

    w = new CommentEditorPanel(parent,false);
    stackIds[getOptDialogName(optComment)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optComment)]] = w;


    w = new MetricEditorPanel(parent);
    stackIds[getOptDialogName(optMetric)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optMetric)]] = w;

    w = new CompilerOutputPanel(parent);
    stackIds[getOptDialogName(optRuleCompile)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optRuleCompile)]] = w;

    w = new BaseObjectDialog(parent);
    stackIds["BLANK"]  = parentWidget->addWidget(w);
    dialogs[stackIds["BLANK"]] = w;


}

void ObjectEditor::attachToProjectWindow(ProjectPanel *pp)
{ 
    m_project = pp;

    QMapIterator<int, BaseObjectDialog*> it(dialogs);
    while (it.hasNext())
    {
        it.next();
        it.value()->attachToProjectWindow(pp);
    }
}

QWidget* ObjectEditor::getCurrentObjectDialog()
{
    if (current_dialog_idx >= 0) return dialogs[current_dialog_idx];
    else return NULL;
}

void ObjectEditor::show()
{
}

/*
 * need to call ProjectPanel::closeEditorPanel from here because
 * we connect signal clicked() from the closeButton to the slot in
 * ObjectEditor rather than in ProjectPanel.
 */
void ObjectEditor::hide()
{
    mw->closeEditorPanel();
    current_dialog_idx = -1;
    current_dialog_name = "";
}

bool ObjectEditor::isVisible()
{
    return (parentWidget->isVisible());
}

bool ObjectEditor::isModified()
{
    return applyButton->isEnabled();
}

QString ObjectEditor::getOptDialogName(OptType t)
{
    return QString("OptionDialog_%1").arg(t);
}

void ObjectEditor::openOpt(FWObject *obj,OptType t)
{
    if (Rule::cast(obj)==NULL) return;

    disconnectSignals();

    current_dialog_name = getOptDialogName(t);
    current_dialog_idx = stackIds[current_dialog_name];

    show();

    connect(this, SIGNAL(loadObject_sign(libfwbuilder::FWObject*)),
            dialogs[ current_dialog_idx ],
            SLOT(loadFWObject(libfwbuilder::FWObject*)));

    connect(this, SIGNAL(validate_sign(bool*)),
            dialogs[ current_dialog_idx ],
            SLOT(validate(bool*)));

    connect(this, SIGNAL(applyChanges_sign()),
            dialogs[ current_dialog_idx ],
            SLOT(applyChanges()));

    connect(this, SIGNAL(discardChanges_sign()),
            dialogs[ current_dialog_idx ],
            SLOT(discardChanges()));

    connect(dialogs[ current_dialog_idx ], SIGNAL(close_sign(QCloseEvent*)),
            this,
            SLOT(validateAndClose(QCloseEvent*)));

    connect(dialogs[ current_dialog_idx ], SIGNAL(changed_sign()),
            this,
            SLOT(changed()));

    connect(dialogs[ current_dialog_idx ], SIGNAL(notify_changes_applied_sign()),
            this,
            SLOT(notifyChangesApplied()));

    connect(this, SIGNAL(getHelpName_sign(QString*)),
            dialogs[ current_dialog_idx ],
            SLOT(getHelpName(QString*)));

    emit loadObject_sign(obj);
    findAndLoadHelp();

    opened = obj;
    openedOpt = t;
    applyButton->setEnabled(false);

}

void ObjectEditor::open(FWObject *obj)
{
    openedOpt = optNone;
    if (stackIds.count(obj->getTypeName().c_str())!=0)
    {
        disconnectSignals();

        current_dialog_name = obj->getTypeName().c_str();
        current_dialog_idx = stackIds[current_dialog_name];

//        disconnect( SIGNAL(loadObject_sign(libfwbuilder::FWObject*)) );
//        disconnect( SIGNAL(validate_sign(bool*)) );
//        disconnect( SIGNAL(isChanged_sign(bool*)) );
//        disconnect( SIGNAL(applyChanges_sign()) );
//        disconnect( SIGNAL(discardChanges_sign()) );
//        disconnect( SIGNAL(close_sign(QCloseEvent*)) );

        //hide();



        show();

        connect(this, SIGNAL(loadObject_sign(libfwbuilder::FWObject*)),
                dialogs[ current_dialog_idx ],
                SLOT(loadFWObject(libfwbuilder::FWObject*)));

        connect(this, SIGNAL(validate_sign(bool*)),
                dialogs[ current_dialog_idx ],
                SLOT(validate(bool*)));

        //connect(this, SIGNAL(isChanged_sign(bool*)),
        //        dialogs[ current_dialog_idx ],
        //        SLOT(isChanged(bool*)));

        connect(this, SIGNAL(applyChanges_sign()),
                dialogs[ current_dialog_idx ],
                SLOT(applyChanges()));

        connect(this, SIGNAL(discardChanges_sign()),
                dialogs[ current_dialog_idx ],
                SLOT(discardChanges()));

        connect(dialogs[ current_dialog_idx ], SIGNAL(close_sign(QCloseEvent*)),
                this,
                SLOT(validateAndClose(QCloseEvent*)));

        connect(dialogs[ current_dialog_idx ], SIGNAL(changed_sign()),
                this,
                SLOT(changed()));

        connect(dialogs[ current_dialog_idx ], SIGNAL(notify_changes_applied_sign()),
                this,
                SLOT(notifyChangesApplied()));

        connect(this, SIGNAL(getHelpName_sign(QString*)),
                dialogs[ current_dialog_idx ],
                SLOT(getHelpName(QString*)));

        emit loadObject_sign(obj);
        findAndLoadHelp();
    }

    opened = obj;
    applyButton->setEnabled(false);
}

void ObjectEditor::disconnectSignals()
{
    disconnect( SIGNAL(loadObject_sign(libfwbuilder::FWObject*)) );
    disconnect( SIGNAL(validate_sign(bool*)) );
    //disconnect( SIGNAL(isChanged_sign(bool*)) );
    disconnect( SIGNAL(applyChanges_sign()) );
    disconnect( SIGNAL(discardChanges_sign()) );
    disconnect( SIGNAL(getHelpName_sign(QString*)) );
    if (current_dialog_idx>=0) dialogs[current_dialog_idx]->disconnect( this );
}

void ObjectEditor::purge()
{
    if (fwbdebug) qDebug("ObjectEditor::purge");

    applyButton->setEnabled(false);
    current_dialog_idx = stackIds["BLANK"];
    current_dialog_name = "BLANK";
    findAndLoadHelp();
    opened = NULL;
    openedOpt = optNone;
}

/* editor window needs to close. Check if something changed in it and
 * accept or ignore closing event
 */
void ObjectEditor::validateAndClose(QCloseEvent *e)
{
    if (fwbdebug) qDebug("ObjectEditor::validateAndClose");

    if (validateAndSave())
    {
        if (e) e->accept();
        //disconnectSignals();  // all signals will be disconnected
                                // in next open(...)
        hide();
        return;
    }
    if (e) e->ignore();
}

bool ObjectEditor::validateAndSave()
{
    if (fwbdebug) qDebug("ObjectEditor::validateAndSave");
    if (current_dialog_idx==stackIds["BLANK"]) return true;
    bool ischanged=false;
    ischanged = isModified();
    //emit isChanged_sign(&ischanged);

    if (fwbdebug)
        qDebug("ObjectEditor::validateAndSave  "
               "ischanged=%d db->isReadOnly()=%d",
               ischanged, m_project->db()->isReadOnly());

/* if nothing changed or tree is read-only, just close dialog */
    if (!ischanged || !isTreeReadWrite(dialogs[current_dialog_idx], m_project->db()))
    {
        if (fwbdebug) qDebug("ObjectEditor::validateAndSave: no changes");
        return true;
    }

/* there are changes and the tree is writable */
    bool isgood=true;
    emit validate_sign( &isgood );
    if (!isgood)
    {
        switch (
            QMessageBox::warning(
                dialogs[current_dialog_idx],
                "Firewall Builder",
                tr("Modifications done to this object can not be saved.\n"
                   "Do you want to continue editing it ?"),
                tr("&Continue editing"),
                tr("&Discard changes"),
                QString::null,
                0, 1 ) )
        {
        case 0:
            if (fwbdebug)
                qDebug("ObjectEditor::validateAndSave:  return false, "
                       "can not switch to another object");
            return false;

        default:
            if (fwbdebug)
                qDebug("ObjectEditor::validateAndSave  return true, "
                       "discard changes, can switch to another object");
            discard();
            return true;
        }
        return false;
    }

/* changes have been validated, need to save now */
    if (st->getAutoSave())
    {
        emit applyChanges_sign();
    } else
    {
        switch (
            QMessageBox::warning(
                dialogs[current_dialog_idx],
                "Firewall Builder",
                tr("Object %1 has been modified but not saved.\n"
                   "Do you want to save it ?").arg(opened->getName().c_str()),
                tr("&Save"), tr("&Discard"), tr("&Continue editing"),
                0, 2 ) )
        {
        case 0:
            apply();
            return true;

        case 1:
            discard();
            return true;

        case 2:
            return false;
        }
    }
    return true;
}

void ObjectEditor::setApplyButton(QPushButton * b)
{
    applyButton=b;
    applyButton->setEnabled(false);
    connect((QWidget*)applyButton,SIGNAL(clicked()),this,SLOT(apply()));
}

void ObjectEditor::setHelpButton(QPushButton * b)
{
    helpButton=b;
    helpButton->setEnabled(true);
    connect((QWidget*)helpButton,SIGNAL(clicked()),this,SLOT(help()));
}

// Object dialog emits signal notify_changes_applied_sign() when it
// applies changes to the object
void ObjectEditor::notifyChangesApplied()
{
    applyButton->setEnabled(false);
    // send event so other project panels can reload themselves
    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(opened->getRoot()->getFileName().c_str(),
                                  opened->getId()));
}

// this method is connected to the "Apply" button
void ObjectEditor::apply()
{
    if (fwbdebug) qDebug("ObjectEditor::apply");

    bool isgood = true;
    emit validate_sign( &isgood );

    if (isgood)
        emit applyChanges_sign(); // eventually emits notify_changes_applied_sign()
}

void ObjectEditor::help()
{
    QString help_name;
    emit getHelpName_sign(&help_name);
    if (fwbdebug)
        qDebug("ObjectEditor::help: %s", help_name.toStdString().c_str());
    Help *h = new Help(parentWidget, "");
    h->setSource(QUrl(help_name + ".html"));
    h->show();
}

void ObjectEditor::findAndLoadHelp()
{
    QString help_name;
    emit getHelpName_sign(&help_name);

    // is help available?
    Help *h = new Help(parentWidget, "");
    helpButton->setEnabled(!h->findHelpFile(help_name + ".html").isEmpty());
    delete h;
}

void ObjectEditor::discard()
{
    emit discardChanges_sign();
    applyButton->setEnabled(false);
}

void ObjectEditor::changed()
{
    applyButton->setEnabled(true);
}

/*
 * For groups, select object in the group dialog. Argument @o is the
 * child object that has to be selected in the dialog, <opened> is
 * currently opened object (which should be the parent of @o or hold
 * reference to @o)
 */
void ObjectEditor::selectObject(FWObject *o)
{
    qDebug("ObjectEditor::selectObject");
    // class Library inherits Group but has its own dialog where
    // children objects are not shown.
    if (Library::cast(opened)!=NULL || Group::cast(opened)==NULL || current_dialog_idx==-1)
        return;
    ((GroupObjectDialog *) dialogs[current_dialog_idx])->selectObject(o);
}

void ObjectEditor::selectionChanged(FWObject*)
{
}

void ObjectEditor::actionChanged(FWObject *o)
{
    if (current_dialog_idx==-1)
    {
        purge();
        return;
    }
    //if (opened==o) return;
    openOpt (o,ObjectEditor::optAction);

    show();
}

void ObjectEditor::blank()
{
    if (isVisible())
    {
        applyButton->setEnabled(false);
        current_dialog_idx = stackIds["BLANK"];
        current_dialog_name = "BLANK";
        findAndLoadHelp();
        opened = NULL;
        show();
    }
}
