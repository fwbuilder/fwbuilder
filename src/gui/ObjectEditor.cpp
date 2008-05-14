/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectEditor.cpp,v 1.44 2007/04/14 00:18:43 vkurland Exp $

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

#include "ObjectEditor.h"

#include <qobject.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qmenu.h>
#include <qlayout.h>
#include <qstackedwidget.h>
#include <qpushbutton.h>

#include "DialogFactory.h"
#include "FWBTree.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "GroupObjectDialog.h"
#include "ActionsDialog.h"
#include "MetricEditorPanel.h"
#include "CommentEditorPanel.h"
#include "ObjectManipulator.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/TagService.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

#define OBJTREEVIEW_WIDGET_NAME  "ObjTreeView"



ObjectEditor::ObjectEditor( QWidget *parent, ProjectPanel *project):
  QObject(parent), opened(0), visible(-1),
  parentWidget(dynamic_cast<QStackedWidget*>(parent)), closeButton(0), applyButton(0),
    m_project(project), openedOpt(optNone)
{

#if defined(Q_WS_X11)
/* do something that makes sense only on X11 */

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
/* do something that only works on windows */

#elif defined(Q_OS_MAC)

#endif

    QWidget *w;
    w= DialogFactory::createDialog(m_project, parent,Library::TYPENAME);
    stackIds[Library::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Library::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,IPv4::TYPENAME);
    stackIds[IPv4::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPv4::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,IPv6::TYPENAME);
    stackIds[IPv6::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPv6::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,physAddress::TYPENAME);
    stackIds[physAddress::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[physAddress::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,AddressRange::TYPENAME);
    stackIds[AddressRange::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[AddressRange::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Firewall::TYPENAME);
    stackIds[Firewall::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Firewall::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Host::TYPENAME);
    stackIds[Host::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Host::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Interface::TYPENAME);
    stackIds[Interface::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Interface::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Network::TYPENAME);
    stackIds[Network::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Network::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,NetworkIPv6::TYPENAME);
    stackIds[NetworkIPv6::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[NetworkIPv6::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,CustomService::TYPENAME);
    stackIds[CustomService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[CustomService::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,IPService::TYPENAME);
    stackIds[IPService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[IPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,ICMPService::TYPENAME);
    stackIds[ICMPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ICMPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,TCPService::TYPENAME);
    stackIds[TCPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[TCPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,UDPService::TYPENAME);
    stackIds[UDPService::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[UDPService::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,ObjectGroup::TYPENAME);
    stackIds[ObjectGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ObjectGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,ServiceGroup::TYPENAME);
    stackIds[ServiceGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[ServiceGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,IntervalGroup::TYPENAME);
    stackIds[IntervalGroup::TYPENAME] = parentWidget->addWidget(w);
    dialogs[stackIds[IntervalGroup::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Interval::TYPENAME);
    stackIds[Interval::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Interval::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,Rule::TYPENAME);
    stackIds[Rule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[Rule::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,RoutingRule::TYPENAME);
    stackIds[RoutingRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[RoutingRule::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,PolicyRule::TYPENAME);
    stackIds[PolicyRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[PolicyRule::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,NATRule::TYPENAME);
    stackIds[NATRule::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[NATRule::TYPENAME]] = w;


    w= DialogFactory::createDialog(m_project, parent,DNSName::TYPENAME);
    stackIds[DNSName::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[DNSName::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,AddressTable::TYPENAME);
    stackIds[AddressTable::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[AddressTable::TYPENAME]] = w;

    w= DialogFactory::createDialog(m_project, parent,TagService::TYPENAME);
    stackIds[TagService::TYPENAME]  = parentWidget->addWidget(w);
    dialogs[stackIds[TagService::TYPENAME]] = w;


    w= new ActionsDialog(parent);
    stackIds[getOptDialogName(optAction)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optAction)]] = w;

    w= new CommentEditorPanel(parent,false);
    stackIds[getOptDialogName(optComment)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optComment)]] = w;


    w= new MetricEditorPanel(parent);
    stackIds[getOptDialogName(optMetric)]  = parentWidget->addWidget(w);
    dialogs[stackIds[getOptDialogName(optMetric)]] = w;

    w=new QWidget(parent);
    stackIds["BLANK"]  = parentWidget->addWidget(w);
    dialogs[stackIds["BLANK"]] = w;


}

void ObjectEditor::show()
{
    //dialogs[ visible ]->adjustSize();
    //if (st->haveGeometry(dialogs[ visible ]))
    //    st->restoreGeometry(dialogs[ visible ]);
    //if (st->haveScreenPosition("Object Editor"))
    //    dialogs[ visible ]->move(st->getScreenPosition("Object Editor"));
    //dialogs[ visible ]->show();
    parentWidget->setCurrentIndex(visible);
    m_project->openEditorPanel();
}

void ObjectEditor::hide()
{
//    if (visible==-1)
//    {
//        QMap<QString, int>::iterator i;
//        for (i=stackIds.begin(); i!=stackIds.end(); ++i)
//            dialogs[ i.data() ]->hide();
//    } else
//    {
//        st->saveGeometry(dialogs[ visible ]);
////        QPoint p = dialogs[ visible ]->pos();
//        QRect   g = dialogs[ visible ]->geometry();
//        g.moveTopLeft(dialogs[ visible ]->frameGeometry().topLeft());
//
//       if (g.x()!=0 && g.y()!=0)
//            st->saveScreenPosition("Object Editor",g.topLeft());
//
//       dialogs[ visible ]->hide();
//    }
    m_project->closeEditorPanel();
    visible=-1;
}

bool ObjectEditor::isVisible()
{
    //return (visible!=-1 && dialogs[visible]->isVisible());
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

    int wid= stackIds[getOptDialogName(t)];

    visible=wid;

    show();

    connect(this, SIGNAL(loadObject_sign(libfwbuilder::FWObject*)),
            dialogs[ wid ],
            SLOT(loadFWObject(libfwbuilder::FWObject*)));

    connect(this, SIGNAL(validate_sign(bool*)),
            dialogs[ wid ],
            SLOT(validate(bool*)));

    connect(this, SIGNAL(applyChanges_sign()),
            dialogs[ wid ],
            SLOT(applyChanges()));

    connect(this, SIGNAL(discardChanges_sign()),
            dialogs[ wid ],
            SLOT(discardChanges()));

    connect(dialogs[ wid ], SIGNAL(close_sign(QCloseEvent*)),
            this,
            SLOT(validateAndClose(QCloseEvent*)));
    connect(dialogs[ wid ], SIGNAL(changed_sign()),
            this,
            SLOT(changed()));

    emit loadObject_sign(obj);

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

        int wid= stackIds[obj->getTypeName().c_str()];

//        disconnect( SIGNAL(loadObject_sign(libfwbuilder::FWObject*)) );
//        disconnect( SIGNAL(validate_sign(bool*)) );
//        disconnect( SIGNAL(isChanged_sign(bool*)) );
//        disconnect( SIGNAL(applyChanges_sign()) );
//        disconnect( SIGNAL(discardChanges_sign()) );
//        disconnect( SIGNAL(close_sign(QCloseEvent*)) );

        //hide();


        visible=wid;

        show();

        connect(this, SIGNAL(loadObject_sign(libfwbuilder::FWObject*)),
                dialogs[ wid ],
                SLOT(loadFWObject(libfwbuilder::FWObject*)));

        connect(this, SIGNAL(validate_sign(bool*)),
                dialogs[ wid ],
                SLOT(validate(bool*)));

        //connect(this, SIGNAL(isChanged_sign(bool*)),
        //        dialogs[ wid ],
        //        SLOT(isChanged(bool*)));

        connect(this, SIGNAL(applyChanges_sign()),
                dialogs[ wid ],
                SLOT(applyChanges()));

        connect(this, SIGNAL(discardChanges_sign()),
                dialogs[ wid ],
                SLOT(discardChanges()));

        connect(dialogs[ wid ], SIGNAL(close_sign(QCloseEvent*)),
                this,
                SLOT(validateAndClose(QCloseEvent*)));
        connect(dialogs[ wid ], SIGNAL(changed_sign()),
                this,
                SLOT(changed()));

        emit loadObject_sign(obj);
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
    if (visible>=0) dialogs[visible]->disconnect( this );
}

void ObjectEditor::purge()
{
    if (fwbdebug) qDebug("ObjectEditor::purge");

    applyButton->setEnabled(false);
    int wid = stackIds["BLANK"];
    visible = wid;
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
    if (visible==stackIds["BLANK"]) return true;
    bool ischanged=false;
    ischanged = isModified();
    //emit isChanged_sign(&ischanged);

/* if nothing changed or tree is read-only, just close dialog */
    if (!ischanged || !isTreeReadWrite(dialogs[ visible ],m_project->db()))
    {
        if (fwbdebug)
            qDebug("ObjectEditor::validateAndSave: no changes");
        return true;
    }

/* there are changes and the tree is writable */
    bool isgood=true;
    emit validate_sign( &isgood );
    if (!isgood)
    {
        switch ( QMessageBox::warning(dialogs[ visible ],
                                      "Firewall Builder",
                                      tr("Modifications done to this object can not be saved.\nDo you want to continue editing it ?"),
                                      tr("&Continue editing"),
                                      tr("&Discard changes"),
                                      QString::null,
                                      0, 1 ) )
        {
        case 0:
            if (fwbdebug)
                qDebug("ObjectEditor::validateAndSave:  return false, can not switch to another object");
            return false;

        default:
            if (fwbdebug)
                qDebug("ObjectEditor::validateAndSave  return true, discard changes, can switch to another object");
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
        switch ( QMessageBox::warning(dialogs[ visible ],
                                      "Firewall Builder",
                                      tr("This object has been modified but not saved.\nDo you want to save it ?"),
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

void ObjectEditor::setCloseButton(QPushButton * b)
{
    closeButton=b;
    connect((QWidget*)closeButton,SIGNAL(clicked()),this,SLOT(close()));
}

void ObjectEditor::setApplyButton(QPushButton * b)
{
    applyButton=b;
    applyButton->setEnabled(false);
    connect((QWidget*)applyButton,SIGNAL(clicked()),this,SLOT(apply()));

}

void ObjectEditor::close()
{
    if (fwbdebug) qDebug("ObjectEditor::close");

    validateAndClose(NULL);
}

void ObjectEditor::apply()
{
    bool isgood=true;
    emit validate_sign( &isgood );
    if (isgood)
    {
        emit applyChanges_sign();
        applyButton->setEnabled(false);
        m_project->updateRuleSetView( );

        m_project->updateTreeViewItemOrder();
    }
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

void ObjectEditor::selectObject(FWObject *o)
{
    qDebug("ObjectEditor::selectObject");
    if (Group::cast(opened)==NULL || visible==-1) return;
   ((GroupObjectDialog *) dialogs[ visible ])->selectObject(o);
}

void ObjectEditor::selectionChanged(FWObject *o)
{
    /*
    if (visible==-1) return;
    //if (opened==o) return;
    open (o);
    show();
    */
}

void ObjectEditor::actionChanged(FWObject *o)
{
    if (visible==-1)
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
        int wid= stackIds["BLANK"];

        visible=wid;
        opened=NULL;
        show();
    }
}
