/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectManipulator.cpp,v 1.164 2007/07/07 05:39:34 vkurland Exp $

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
#include "utils_no_qt.h"

#include "ObjectManipulator.h"
#include "ObjectEditor.h"
#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"
#include "FWObjectClipboard.h"
#include "FWObjectPropertiesFactory.h"
#include "FWBSettings.h"
#include "listOfLibraries.h"
#include "newFirewallDialog.h"
#include "newHostDialog.h"
#include "findDialog.h"
#include "newGroupDialog.h"
#include "FindObjectWidget.h"

#include <QTextEdit>
#include <QTime>
#include <qobject.h>
#include <qobject.h>
#include <qimage.h>
#include <qpixmapcache.h>
#include <qheaderview.h>
#include <QStackedWidget>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qaction.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qmenu.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstatusbar.h>
#include <qeventloop.h>
#include <QPixmap>


#include "DialogFactory.h"
#include "FWBTree.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ConfirmDeleteObjectDialog.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/RuleElement.h"

#ifdef USE_INTERFACE_POLICY
#  include "fwbuilder/InterfacePolicy.h"
#endif

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/TagService.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Management.h"

#include <iostream>
#include <algorithm>
#include "AskLibForCopyDialog.h"

using namespace std;
using namespace libfwbuilder;

#define OBJTREEVIEW_WIDGET_NAME  "ObjTreeView"


HistoryItem::~HistoryItem() {}

ObjectManipulator::~ObjectManipulator()
{
    delete m_objectManipulator;
}

ObjectManipulator::ObjectManipulator( QWidget *parent):
  QWidget(parent), currentObj(0), current_tree_view(0)
{
    m_objectManipulator = new Ui::ObjectManipulator_q;
    m_objectManipulator->setupUi(this);
    setObjectName(tr("Object Manipulator"));
    QObject* par = parent;
    while(! (m_project = dynamic_cast<ProjectPanel*>(par)))
    {
        par = par->parent();
    }
  
    treeWidth    = -1;
    treeHeight   = -1;
    currentObj   = NULL;
    active       = false;
    current_tree_view=NULL;
    
//    setFocusPolicy( QWidget::StrongFocus  );

/* Adding pop-down menu to the button "New" */

    QString icon_path=":/Icons/";

    QMenu* newObjectPopup = new QMenu( this );

    newObjectPopup->addAction(QIcon(icon_path+Library::TYPENAME+"/icon-tree"), tr( "New &Library" ), this, SLOT( newLibrary() ));

    newObjectPopup->addSeparator();

    newObjectPopup->addAction(QIcon(icon_path+Firewall::TYPENAME+"/icon-tree"), tr( "New &Firewall" ), this, SLOT( newFirewall() ));
    newObjectPopup->addAction(QIcon(icon_path+Host::TYPENAME+"/icon-tree"), tr( "New &Host" ), this, SLOT( newHost() ));
    newObjectPopup->addAction(QIcon(icon_path+Interface::TYPENAME+"/icon-tree"), tr( "New &Interface" ), this, SLOT( newInterface() ));
    newObjectPopup->addAction(QIcon(icon_path+Network::TYPENAME+"/icon-tree"), tr( "New &Network" ), this, SLOT( newNetwork() ));
    newObjectPopup->addAction(QIcon(icon_path+IPv4::TYPENAME+"/icon-tree"), tr( "New &Address" ), this, SLOT( newAddress() ));
    newObjectPopup->addAction(QIcon(icon_path+DNSName::TYPENAME+"/icon-tree"), tr( "New &DNS Name" ), this, SLOT( newDNSName() ));
    newObjectPopup->addAction(QIcon(icon_path+AddressTable::TYPENAME+"/icon-tree"), tr( "New A&ddress Table" ), this, SLOT( newAddressTable() ));
    newObjectPopup->addAction(QIcon(icon_path+AddressRange::TYPENAME+"/icon-tree"), tr( "New Address &Range" ), this, SLOT( newAddressRange() ));
    newObjectPopup->addAction(QIcon(icon_path+ObjectGroup::TYPENAME+"/icon-tree"), tr( "New &Object Group" ), this, SLOT( newObjectGroup() ));
    newObjectPopup->addSeparator();
    newObjectPopup->addAction(QIcon(icon_path+CustomService::TYPENAME+"/icon-tree"), tr( "New &Custom Service" ), this, SLOT( newCustom() ));
    newObjectPopup->addAction(QIcon(icon_path+IPService::TYPENAME+"/icon-tree"), tr( "New &IP Service" ), this, SLOT( newIP() ));
    newObjectPopup->addAction(QIcon(icon_path+ICMPService::TYPENAME+"/icon-tree"), tr( "New IC&MP Service" ), this, SLOT( newICMP() ));
    newObjectPopup->addAction(QIcon(icon_path+TCPService::TYPENAME+"/icon-tree"), tr( "New &TCP Serivce" ), this, SLOT( newTCP() ));
    newObjectPopup->addAction(QIcon(icon_path+UDPService::TYPENAME+"/icon-tree"), tr( "New &UDP Service" ), this, SLOT( newUDP() ));
    newObjectPopup->addAction(QIcon(icon_path+TagService::TYPENAME+"/icon-tree"), tr( "New &TagService" ), this, SLOT( newTagService() ));
    newObjectPopup->addAction(QIcon(icon_path+ServiceGroup::TYPENAME+"/icon-tree"), tr( "New &Service Group" ), this, SLOT( newServiceGroup() ));
    newObjectPopup->addSeparator();
    newObjectPopup->addAction(QIcon(icon_path+Interval::TYPENAME+"/icon-tree"), tr( "New Ti&me Interval" ), this, SLOT( newInterval() ));

//    QToolButton *btn = (QToolButton*)toolBar->child("newObjectAction_action_button");

    m_objectManipulator->newButton->setMenu( newObjectPopup );

#if defined(Q_WS_X11)
/* do something that makes sense only on X11 */

#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
/* do something that only works on windows */

#elif defined(Q_OS_MAC)

#endif

//    backwardAction->setEnabled( false );

//    setMinimumSize( QSize( 0, 174 ) );
//    splitter3->setMinimumSize( QSize( 0, 118 ) );
//    treeFrame->setMinimumSize( QSize( 200, 0 ) );
//    splitter3->setResizeMode( treeFrame, QSplitter::KeepSize );
}


QString ObjectManipulator::getTreeLabel( FWObject *obj )
{
    QString name;

    if (Interface::isA(obj))
    {
        name=Interface::constcast(obj)->getLabel().c_str();
        if (name=="")  name=QString::fromUtf8(obj->getName().c_str());
        QString q;
        if (Interface::constcast(obj)->isDyn())        q=" dyn";
        if (Interface::constcast(obj)->isUnnumbered()) q=" unnum";
        if (Interface::constcast(obj)->isBridgePort()) q=" bridge port";
        if (Interface::constcast(obj)->isExt())        q=q+" ext";
        if (Interface::constcast(obj)->isUnprotected())q=q+" unp";
        if (q!="") name=name+" ("+q+")";
    }
    else
    {
        name=QString::fromUtf8(obj->getName().c_str());
        if (Library::isA(obj) && obj->isReadOnly())
	    name=name+QObject::tr(" ( read only )");
    }

#if 0
    if (name=="")
    {  // no name, use type description string instead
        name= Resources::global_res->getObjResourceStr(obj,"description").c_str();
    }
#endif
    return name;
}

ObjectTreeViewItem* ObjectManipulator::insertObject( ObjectTreeViewItem *itm,
                                                     FWObject *obj )
{
    if (FWReference::cast(obj)!=NULL) return NULL;
    if (Resources::global_res->getObjResourceBool(obj,"hidden") ) return NULL;
    if (RuleSet::cast(obj)!=NULL) return NULL;

    ObjectTreeViewItem *nitm=NULL;

    QString icn_filename;

    if (m_project->isSystem(obj))  icn_filename=":/Icons/folder1.png";
    else
        icn_filename=(":/Icons/"+obj->getTypeName()+"/icon-tree").c_str();
        //icn_filename=Resources::global_res->getObjResourceStr(obj, "icon-tree").c_str();

    if (obj->getBool("ro")) icn_filename = ":/Icons/lock.png";

    if (Resources::global_res->getResourceBool(
            string("/FWBuilderResources/Type/") +
            obj->getTypeName() + "/hidden") ) return NULL;

    nitm=new ObjectTreeViewItem( itm );
    nitm->setLib("");
    nitm->setText( 0, getTreeLabel(obj) );
    QPixmap pm;
    if ( ! QPixmapCache::find( icn_filename, pm) )
    {
        pm.load( icn_filename );
        QPixmapCache::insert( icn_filename, pm);
    }
    nitm->setIcon( 0, QIcon(pm) );
    nitm->setIcon( 1, QIcon(pm) );
    nitm->setFlags(nitm->flags() | Qt::ItemIsDragEnabled);

    nitm->setProperty("id",   obj->getId().c_str()   );
    nitm->setProperty("type", obj->getTypeName().c_str() );
    nitm->setFWObject( obj );

    allItems[obj] = nitm;

    return nitm;
}


void ObjectManipulator::insertSubtree( ObjectTreeViewItem *itm,
                                       FWObject *obj )
{
    ObjectTreeViewItem *nitm = insertObject(itm, obj);

    if (nitm==NULL) return;
    if ( m_project->isSystem(obj) ) nitm->setExpanded( st->getExpandTree() );

    for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
    {
        FWObject *o1=*m;
        if (FWReference::cast(o1)!=NULL) continue;
        insertSubtree( nitm, o1 );
    }
}

void ObjectManipulator::showDeletedObjects(bool f)
{
    try
    {
        FWObject *dobj = m_project->db()->findInIndex( FWObjectDatabase::getDeletedObjectsId());

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects f=%d  dobj=%p",f, dobj);

        if (dobj==NULL)
        {
            dobj=m_project->db()->create(Library::TYPENAME);
            dobj->setId(m_project->db()->getDeletedObjectsId());
            dobj->setName("Deleted Objects");
            dobj->setReadOnly(false);
            m_project->db()->add(dobj);
        }

        int idx = getIdxForLib(dobj);

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects idx=%d",idx);

        if (f)
        {
            if (idx>=0) return;
            addTreePage( dobj );
            openLib( dobj );
        } else
        {
            if (idx<0) return;

            QTreeWidget *otv = idxToTrees[idx];

            if (fwbdebug)
                qDebug("ObjectManipulator::showDeletedObjects otv=%p",otv);

            assert(otv!=NULL);
            m_objectManipulator->widgetStack->removeWidget( otv );
            removeLib(idx);
        }
    }
    catch(FWException &ex)
    {
/* we get exception if file is opened read-only and there is no "deleted
 * objects" library yet
 */
    }
}

void ObjectManipulator::removeObjectFromTreeView(FWObject *obj )
{
    QTreeWidget *objTreeView = idxToTrees[ getIdxForLib(getCurrentLib()) ];
    dynamic_cast<ObjectTreeView*>(objTreeView)->clearLastSelected();

    ObjectTreeViewItem *itm = allItems[obj];
    allItems[obj]=NULL;
//    allItems.erase(obj);

    itm->parent()->takeChild( itm->parent()->indexOfChild(itm) );
    delete itm;
}

void ObjectManipulator::updateLibColor(FWObject *lib)
{
    QTreeWidget *objTreeView = idxToTrees[ getIdxForLib(lib) ];

    QString clr=lib->getStr("color").c_str();
    if (clr=="" || clr=="#000000" || clr=="black") clr="#FFFFFF";

    QPalette palette = objTreeView->palette();
    palette.setColor(QPalette::Active, QPalette::Base, QColor( clr ));
    objTreeView->setPalette(palette);
}

int ObjectManipulator::getIdxForLib(FWObject* lib)
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
        if ( idxToLibs[i]->getId() == lib->getId() ) return i;

    return -1;
}

void ObjectManipulator::updateLibName(FWObject *lib)
{
    int              oldidx = getIdxForLib(lib);
    QTreeWidget  *objTreeView = idxToTrees[oldidx];
    QString      newlibname = QString::fromUtf8(lib->getName().c_str());

    if (m_objectManipulator->libs->itemText(oldidx)!=newlibname)
    {
        removeLib(oldidx);
//        libs->removeItem( oldidx );
//        idxToLibs.erase(oldidx);
//        idxToTrees.erase(oldidx);

        addLib(lib,objTreeView);

    }
}


/*
 * TODO: make this signal/slot. Dialogs just emit signal
 * 'updateObject_sign', which objectManipulator should have connected
 * to its slot which would do what updateObjName does now, and more.
 */
void ObjectManipulator::updateObjName(FWObject *obj,
                                      const QString &oldName,
                                      bool  askForAutorename)
{
    info();  // need to update info in case user edited comments and other attributes.

    if (oldName == obj->getName().c_str()) return;

    if (obj!=currentObj) openObject(obj);

    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=NULL);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::updateObjName  changing name %s -> %s",
               oldName.toLatin1().constData(), QString::fromUtf8(obj->getName().c_str()).toLatin1().constData());
    }

    if ((QString::fromUtf8(obj->getName().c_str())!=oldName) &&
         (Host::isA(obj) || Firewall::isA(obj) || Interface::isA(obj)))
    {
        if (fwbdebug)
            qDebug("ObjectManipulator::updateObjName  autorename");
        autorename(obj,askForAutorename);
        if (fwbdebug)
            qDebug("ObjectManipulator::updateObjName  autorename done");
    }

    itm->setText(0, getTreeLabel( obj ) );

    if (!Library::isA(obj)) itm->parent()->sortChildren(0, Qt::AscendingOrder);

/* need to update name of the firewall in the drop-down list */
    if (Firewall::isA(obj))
    {
        m_project->updateFirewallName(obj,oldName);
    }

    // reopenFirewalls is called from FirewallDialog::applyChanges()
    //if (QString::fromUtf8(obj->getName().c_str())!=oldName)
    //{
    //  QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );
    //}


}

/*
 * variant specifically used for interfaces that have name and a label
 */
void ObjectManipulator::updateObjName(FWObject *obj,
                                      const QString &oldName,
                                      const QString &oldLabel,
                                      bool  askForAutorename)
{
    if (obj!=currentObj) openObject(obj);

    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=NULL);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::updateObjName  changing name %s -> %s",
               oldName.toLatin1().constData(), QString::fromUtf8(obj->getName().c_str()).toLatin1().constData());
    }

    if ((QString::fromUtf8(obj->getName().c_str())!=oldName) && Interface::isA(obj))
        autorename(obj,askForAutorename);

    itm->setText(0, getTreeLabel( obj ) );
    itm->parent()->sortChildren(0, Qt::AscendingOrder);//();

    Interface *i = Interface::cast(obj);
    if  ((i!=NULL && i->getLabel()!=oldLabel.toLatin1().constData()) ||
         (QString::fromUtf8(obj->getName().c_str())!=oldName))
    {
        //m_project->reopenFirewall();
        m_project->scheduleRuleSetRedraw();
    }

    info();  // need to update info in case user edited comments and other attributes.
}

void ObjectManipulator::autorename(FWObject *obj,bool ask)
{
    if (Host::isA(obj) || Firewall::isA(obj))
    {
        if (!ask || QMessageBox::warning(
                this,"Firewall Builder",
                tr(
"The name of the object '%1' has changed. The program can also\n"
"rename IP address objects that belong to this object,\n"
"using standard naming scheme 'host_name:interface_name:ip'.\n"
"This makes it easier to distinguish what host or a firewall\n"
"given IP address object belongs to when it is used in \n"
"the policy or NAT rule. The program also renames MAC address\n"
"objects using scheme 'host_name:interface_name:mac'.\n"
"Do you want to rename child IP and MAC address objects now?\n"
"(If you click 'No', names of all address objects that belong to\n"
"%1 will stay the same.)")
                .arg(QString::fromUtf8(obj->getName().c_str()))
                .arg(QString::fromUtf8(obj->getName().c_str())),
                tr("&Yes"), tr("&No"), QString::null,
                0, 1 )==0 )
        {
            list<FWObject*> il = obj->getByType(Interface::TYPENAME);
            for (list<FWObject*>::iterator i=il.begin(); i!=il.end(); ++i)
            {
                autorename(*i,IPv4::TYPENAME,"ip");
                autorename(*i,physAddress::TYPENAME,"mac");
            }
        }
    }

    if (Interface::isA(obj))
    {
        if (!ask || QMessageBox::warning(
                this,"Firewall Builder",
                tr(
"The name of the interface '%1' has changed. The program can also\n"
"rename IP address objects that belong to this interface,\n"
"using standard naming scheme 'host_name:interface_name:ip'.\n"
"This makes it easier to distinguish what host or a firewall\n"
"given IP address object belongs to when it is used in \n"
"the policy or NAT rule. The program also renames MAC address\n"
"objects using scheme 'host_name:interface_name:mac'.\n"
"Do you want to rename child IP and MAC address objects now?\n"
"(If you click 'No', names of all address objects that belong to\n"
"%1 will stay the same.)")
                .arg(QString::fromUtf8(obj->getName().c_str()))
                .arg(QString::fromUtf8(obj->getName().c_str())),
                tr("&Yes"), tr("&No"), QString::null,
                0, 1 )==0 )
        {
            autorename(obj,IPv4::TYPENAME,"ip");
            autorename(obj,physAddress::TYPENAME,"mac");
        }
    }
}

void ObjectManipulator::autorename(FWObject *obj,
                                   const string &objtype,
                                   const string &namesuffix)
{
    FWObject      *hst = obj->getParent();
    list<FWObject*> ol = obj->getByType(objtype);
    int           sfxn = 1;

    for (list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); ++j,sfxn++)
    {
        QString sfx;
        if (ol.size()==1) sfx="";
        else              sfx.setNum(sfxn);
        QString nn = QString("%1:%2:%3%4")
            .arg(QString::fromUtf8(hst->getName().c_str()))
            .arg(QString::fromUtf8(obj->getName().c_str()))
            .arg(namesuffix.c_str())
            .arg(sfx);

        (*j)->setName(string(nn.toUtf8()));
        QTreeWidgetItem *itm1 = allItems[ *j ];
        assert(itm1!=NULL);
        itm1->setText(0, getTreeLabel( *j ) );
        itm1->parent()->sortChildren(0, Qt::AscendingOrder);//();
    }
    ol.clear();
}

void ObjectManipulator::clearObjects()
{
    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  start");

    invalidateDialog();
    while (history.size()!=0) history.pop();

    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  history size: %d",
                         history.size());

    int N=m_objectManipulator->libs->count();

    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  %d libs", N);

    for (int i=N-1; i>=0; i--)
    {
        QTreeWidget *otv = idxToTrees[i];
        assert(otv!=NULL);
        m_objectManipulator->widgetStack->removeWidget( otv );
        delete otv;
        removeLib(i);
    }

    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  idxToLibs size: %d",
                         idxToLibs.size());
    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  idxToTrees size: %d",
                         idxToTrees.size());

    idxToLibs.clear();
    idxToTrees.clear();

    if (fwbdebug) qDebug("ObjectManipulator::clearObjects  done");
}

void ObjectManipulator::loadObjects()
{
    loadObjects( m_project->db() );
}

void ObjectManipulator::loadObjects(FWObjectDatabase *)
{
    if (fwbdebug) qDebug("ObjectManipulator::loadObjects  start");

    if (m_objectManipulator->libs->count()!=0) clearObjects();

    FWObject *firstUserLib=NULL;
    list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );

//    ll.sort(FWObjectNameCmpPredicate());

    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib = (*i);

        if (fwbdebug)
            qDebug("ObjectManipulator::loadObjects  lib %p %s %s",
                   lib, lib->getId().c_str(), lib->getName().c_str() );

        if ( lib->getId()==DELETED_LIB &&
             ! st->getBool("UI/ShowDeletedObjects")) continue;

        if ( lib->getId()!=STANDARD_LIB &&
             lib->getId()!=TEMPLATE_LIB &&
             firstUserLib==NULL) firstUserLib=*i;

        addTreePage( lib );

        if (fwbdebug) qDebug("ObjectManipulator::loadObjects  added lib %s",
                             lib->getName().c_str());
    }

    if (firstUserLib==NULL) firstUserLib=ll.front();
    openLib( firstUserLib );
}

void ObjectManipulator::addLib( FWObject *lib,QTreeWidget* otv)
{
    QString newlibname = QString::fromUtf8(lib->getName().c_str());
    int              N = m_objectManipulator->libs->count();
    int            idx = 0;
    vector<FWObject*>::iterator  i1=idxToLibs.begin();
    vector<QTreeWidget*>::iterator i2=idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1,++i2)
        if ( m_objectManipulator->libs->itemText(idx) > newlibname ) break;

    string icn=":/Icons/"+lib->getTypeName()+"/icon-tree";
            //Resources::global_res->getObjResourceStr(lib,"icon-tree").c_str();
    QPixmap pm;
    if ( ! QPixmapCache::find( icn.c_str(), pm) )
    {
        pm.load( icn.c_str() );
        QPixmapCache::insert( icn.c_str(), pm);
    }
    m_objectManipulator->libs->insertItem( idx, pm, newlibname);
//    idx=libs->count()-1;

    m_objectManipulator->libs->setCurrentIndex(idx);

    idxToLibs.insert(i1,lib);
    if (otv!=NULL) idxToTrees.insert(i2,otv);

}

void ObjectManipulator::addTreePage( FWObject *lib)
{
    if (fwbdebug) qDebug("Object Manipulator::addTreePage");

    ObjectTreeView *objTreeView = new ObjectTreeView(m_project, m_objectManipulator->widgetStack,
                                                      OBJTREEVIEW_WIDGET_NAME );

    addLib(lib,objTreeView);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(objTreeView->sizePolicy().hasHeightForWidth());

    objTreeView->setSizePolicy(policy);

    m_objectManipulator->widgetStack->addWidget( objTreeView );
    m_objectManipulator->widgetStack->show();
    objTreeView->show();

//    objTreeView->setSelectionMode( QListView::Extended );

    updateLibColor( lib );
//    updateLibName( lib );

    //objTreeView->setContextMenuPolicy( Qt::CustomContextMenu );

    connect(m_objectManipulator->widgetStack, SIGNAL( currentChanged(int) ),
             this,        SLOT( currentTreePageChanged(int) ) );

    connect(objTreeView,SIGNAL( editCurrentObject_sign() ),
            this,        SLOT( editSelectedObject()) );

    connect(objTreeView,SIGNAL( editCurrentObject_sign() ),
             this,        SLOT( editSelectedObject()) );

    connect(objTreeView,SIGNAL( switchObjectInEditor_sign(libfwbuilder::FWObject*) ),
             this,        SLOT( switchObjectInEditor(libfwbuilder::FWObject*)) );

    connect( objTreeView, SIGNAL( deleteObject_sign(libfwbuilder::FWObject*) ),
             this,        SLOT( deleteObj() ) );

    connect( objTreeView, SIGNAL( objectDropped_sign(libfwbuilder::FWObject*) ),
             this,        SLOT( openObject(libfwbuilder::FWObject*) ) );

    connect( objTreeView, SIGNAL( contextMenuRequested_sign(const QPoint&) ),
             this,        SLOT( contextMenuRequested(const QPoint&) ) );

    connect( objTreeView, SIGNAL( currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ),
             this,        SLOT( selectionChanged(QTreeWidgetItem*) ) );


    ObjectTreeViewItem *itm1=new ObjectTreeViewItem( objTreeView );

    itm1->setLib("");
    itm1->setExpanded(TRUE);

/* need to enable dragging in order to avoid object highlighting in
 * the tree when user drags mouse cursor */

    itm1->setFlags(itm1->flags() | Qt::ItemIsDragEnabled);

    itm1->setText( 0 , getTreeLabel( lib ) );
    if (lib->isReadOnly())
    {
        QPixmap pm;
        if ( ! QPixmapCache::find( ":/Icons/lock.png", pm) )
        {
            pm.load( ":/Icons/lock.png" );
            QPixmapCache::insert( ":/Icons/lock.png", pm);
        }
        itm1->setIcon(0, pm );
    } else
    {
        string icn=":/Icons/"+lib->getTypeName()+"/icon-tree";
                //Resources::global_res->getObjResourceStr(lib,"icon-tree").c_str();
        QPixmap pm;
        if ( ! QPixmapCache::find( icn.c_str(), pm) )
        {
            pm.load( icn.c_str() );
            QPixmapCache::insert( icn.c_str(), pm);
        }
        itm1->setIcon( 0, pm);
    }

    itm1->setProperty("id",   lib->getId().c_str()   );
    itm1->setProperty("type", lib->getTypeName().c_str() );
    itm1->setFWObject( lib );
    allItems[lib] = itm1;

//    objTreeView->setSelected( itm1, true );

    for (list<FWObject*>::iterator m=lib->begin(); m!=lib->end(); m++)
        insertSubtree( itm1, (*m) );
    objTreeView->updateTreeItems();
    objTreeView->sortByColumn(0,Qt::AscendingOrder);
}

void ObjectManipulator::removeLib(FWObject* lib)
{
    removeLib( getIdxForLib(lib) );
}

void ObjectManipulator::removeLib(int id)
{
    int              N = m_objectManipulator->libs->count();
    int            idx = 0;
    vector<FWObject*>::iterator  i1=idxToLibs.begin();
    vector<QTreeWidget*>::iterator i2=idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1,++i2)
    {
        if ( idx==id )
        {
            m_objectManipulator->libs->removeItem( idx );
            idxToLibs.erase(i1);
            idxToTrees.erase(i2);
        }
    }
}

void ObjectManipulator::currentTreePageChanged(int i)
{
    QWidget *w = m_objectManipulator->widgetStack->widget(i);
    switchingTrees(w);
}

void ObjectManipulator::switchingTrees(QWidget* w)
{
    ObjectTreeView *new_otv = dynamic_cast<ObjectTreeView*>(w);

    if (fwbdebug)
        qDebug("ObjectManipulator::switchingTrees  current_otv=%p  new_otv=%p",
               (void*)(current_tree_view),(void*)(new_otv));

    if (!new_otv)
        return;//assert(new_otv)


    if (current_tree_view!=NULL) current_tree_view->becomingHidden();
    new_otv->becomingVisible();
    current_tree_view = new_otv;

}

void ObjectManipulator::makeNameUnique(FWObject* parent,FWObject* obj)
{
    int      suffix=1;
    QString  basename=QString::fromUtf8(obj->getName().c_str());
    QString  newname=basename;

/* check if there is another object with the same name */
    while (parent->findObjectByName(obj->getTypeName(),newname.toLatin1().constData())!=NULL)
    {
/* there is a duplicate */
        newname=QString("%1-%2").arg(basename).arg(suffix);
        suffix++;
    }
    obj->setName(string(newname.toUtf8()));
}

void ObjectManipulator::contextMenuRequested(const QPoint &pos)
{
/* in extended selection mode there may be several selected items */

    QTreeWidget *objTreeView = getCurrentObjectTree();
    QTreeWidgetItem *item = objTreeView->itemAt(pos);//clicked item

    if (fwbdebug)
        qDebug("ObjectManipulator::contextMenu  selectedObjects.size=%d",
               getCurrentObjectTree()->getNumSelected());

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    if (otvi==NULL)  return;  // happens when user clicks outside an item

    if (!getCurrentObjectTree()->isSelected(otvi->getFWObject()))
        openObject( otvi , true );

    if (currentObj==NULL)  currentObj=otvi->getFWObject();

    QMenu *popup=new QMenu(this);

    QAction *edtID =popup->addAction( tr("Edit"), this, SLOT( editSelectedObject()));

    QMenu *duptargets  = new QMenu(popup);
    QMenu *movetargets = new QMenu(popup);

    connect ( movetargets, SIGNAL ( triggered(QAction*) ),
              this, SLOT( moveObj(QAction*) ) );
    connect ( duptargets, SIGNAL ( triggered(QAction*) ),
              this, SLOT( duplicateObj(QAction*) ) );

/* we add " ... to library ..." submenu to the "Move " menu item only
 * if user did not select a library, or if they selected several
 * objects. Method moveObj knows that library should not be moved
 * into another library.
 */
    bool libSelected =
        (getCurrentObjectTree()->getNumSelected()==1 &&
         Library::isA(getCurrentObjectTree()->getSelectedObjects().front()));

    int libid = 0;

    FWObject *cl=getCurrentLib();
    int moveTargets=0;
    vector<FWObject*>::iterator i;
    for (i=idxToLibs.begin(); i!=idxToLibs.end(); ++i,++libid)
    {
        FWObject *lib   = *i;

        /* can't move to the same library. Will use menu item 'create
         * here' to duplicate to the same library
         */
        if (lib==cl) continue;

        if ( lib->getId()==STANDARD_LIB ||
             lib->getId()==TEMPLATE_LIB ||
             lib->getId()==DELETED_LIB  ||
             lib->isReadOnly())
            continue;
        QAction* dact=duptargets->addAction(
                tr("place in library %1").arg(QString::fromUtf8(lib->getName().c_str()))/*,this, SLOT( duplicateObj(libid))*/
        );

        //duptargets->connectItem( did, this, SLOT( duplicateObj(int)) ); replaced with preprevious string
        //duptargets->setItemParameter(did, libid ); replaced with next:
        dact->setData(libid);

        if (!libSelected)
        {
            QAction* mact=movetargets->addAction(
                    tr("to library %1").arg(QString::fromUtf8(lib->getName().c_str())));

            mact->setData(libid);

            moveTargets++;
        }
    }

    duptargets->addAction(tr("place here"), this, SLOT( duplicateObjUnderSameParent()));

    //QAction *dupID = duptargets->addAction( tr("Duplicate ...") ); // BUGFIX-2346
QAction *dupID = popup->addAction( tr("Duplicate ...") ,this, SLOT (duplicateObjUnderSameParent()));//popup->    
QAction *movID;

    if (moveTargets!=0)
    {
        movID=movetargets->addAction( tr("Move ...") );
    } else
    {
        movID=popup->addAction( tr("Move ...") );
        movID->setData(-1);
    }


    popup->addSeparator();

    QAction *copyID = popup->addAction( tr("Copy") ,      this ,
                                  SLOT( copyObj() ) );
    QAction *cutID =popup->addAction( tr("Cut") ,       this ,
                                  SLOT( cutObj() ) );
    QAction *pasteID =popup->addAction( tr("Paste") ,     this ,
                                    SLOT( pasteObj() ) );

    popup->addSeparator();

    QAction * delID =popup->addAction( tr("Delete") ,    this ,
                                  SLOT( deleteObj() ) );

    QAction *newID1=NULL;
    QAction *newID2=NULL;

    if (getCurrentObjectTree()->getNumSelected()==1)
    {
        popup->addSeparator();

        if ( (Firewall::isA(currentObj) || Host::isA(currentObj)) &&
             ! currentObj->isReadOnly() )
            newID1=popup->addAction( tr("Add Interface"),     this ,
                               SLOT( newInterface() ) );

        if (Interface::isA(currentObj) && ! currentObj->isReadOnly())
        {
            newID1=popup->addAction( tr("Add IP Address"),    this ,
                               SLOT( newInterfaceAddress() ) );
            newID2=popup->addAction( tr("Add MAC Address"),   this ,
                               SLOT( newPhysicalAddress() ) );
        }

        if (currentObj->getPath(true)=="Firewalls")
            newID1=popup->addAction( tr("New Firewall"),      this ,
                               SLOT( newFirewall() ) );

        if (currentObj->getPath(true)=="Objects/Addresses")
        {
            newID1=popup->addAction( tr("New Address"),       this ,
                               SLOT( newAddress() ) );
        }
        if (currentObj->getPath(true)=="Objects/DNS Names")
        {
            newID1=popup->addAction( tr("New DNS Name"),       this ,
                               SLOT( newDNSName() ) );
        }

        if (currentObj->getPath(true)=="Objects/Address Tables")
        {
            newID1=popup->addAction( tr("New Address Table"),  this ,
                               SLOT( newAddressTable() ) );
        }

        if (currentObj->getPath(true)=="Objects/Address Ranges")
            newID1=popup->addAction( tr("New Address Range"), this ,
                               SLOT( newAddressRange() ) );

        if (currentObj->getPath(true)=="Objects/Hosts")
            newID1=popup->addAction( tr("New Host"),          this ,
                               SLOT( newHost() ) );

        if (currentObj->getPath(true)=="Objects/Networks")
            newID1=popup->addAction( tr("New Network"),       this ,
                               SLOT( newNetwork() ) );

        if (currentObj->getPath(true)=="Objects/Groups")
            newID1=popup->addAction( tr("New Group"),         this ,
                               SLOT( newObjectGroup() ) );

        if (currentObj->getPath(true)=="Services/Custom")
            newID1=popup->addAction( tr("New Custom Service"),this ,
                               SLOT( newCustom() ) );

        if (currentObj->getPath(true)=="Services/IP")
            newID1=popup->addAction( tr("New IP Service"),    this ,
                               SLOT( newIP() ) );

        if (currentObj->getPath(true)=="Services/ICMP")
            newID1=popup->addAction( tr("New ICMP Service"),  this ,
                               SLOT( newICMP() ) );

        if (currentObj->getPath(true)=="Services/TCP")
            newID1=popup->addAction( tr("New TCP Service"),   this ,
                               SLOT( newTCP() ) );

        if (currentObj->getPath(true)=="Services/UDP")
            newID1=popup->addAction( tr("New UDP Service"),   this ,
                               SLOT( newUDP() ) );

        if (currentObj->getPath(true)=="Services/TagServices")
            newID1=popup->addAction( tr("New TagService"),   this ,
                               SLOT( newTagService() ) );

        if (currentObj->getPath(true)=="Services/Groups")
            newID1=popup->addAction( tr("New Group"),         this ,
                               SLOT( newServiceGroup() ) );

        if (currentObj->getPath(true)=="Time")
            newID1=popup->addAction( tr("New Time Interval"), this ,
                               SLOT( newInterval() ) );

        popup->addSeparator();
        popup->addAction( tr("Find") , this , SLOT( findObject()));
        popup->addAction( tr("Where used") , this , SLOT( findWhereUsedSlot()));
/*
        if (Firewall::cast(currentObj)!=NULL)
        {
            popup->addSeparator();
            popup->addAction( tr("Compile") , this , SLOT( compile()));
            popup->addAction( tr("Install") , this , SLOT( install()));
        }
        */
    } else
    {

        popup->addAction( tr("Group"), this ,
                           SLOT( groupObjects() ) );

    }

    if (Firewall::cast(currentObj)!=NULL || ObjectGroup::cast(currentObj)!=NULL)
    {
        popup->addSeparator();
        popup->addAction( tr("Compile") , this , SLOT( compile()));
        popup->addAction( tr("Install") , this , SLOT( install()));

//        popup->addSeparator();
//        popup->addAction( tr("Simulate install") , this , SLOT( simulateInstall()));
    }

    popup->addSeparator();
    QAction* lcID=popup->addAction( tr("Lock"), this ,
                       SLOT( lockObject() ) );
    QAction* unlcID=popup->addAction( tr("Unlock"), this ,
                       SLOT( unlockObject() ) );
    lcID->setEnabled(getCurrentObjectTree()->isLockable());
    unlcID->setEnabled(getCurrentObjectTree()->isUnlockable());

    if (fwbdebug)
    {
/* keep this for debugging  */
        popup->addSeparator();
        popup->addAction( tr("dump") , this , SLOT( dumpObj()));
    }

    if (getCurrentObjectTree()->getNumSelected()==1)
    {
        edtID->setEnabled(! m_project->isSystem(currentObj) );
    } else
        edtID->setEnabled(false);

    bool dupMenuItem=true;
    bool moveMenuItem=true;
    bool copyMenuItem=true;
    bool pasteMenuItem=true;
    bool delMenuItem=true;
    bool newMenuItem=true;
    bool inDeletedObjects = false;

    getMenuState( (moveTargets>0),
                  dupMenuItem,moveMenuItem,copyMenuItem,pasteMenuItem,
                  delMenuItem,newMenuItem,inDeletedObjects);

    dupID->setEnabled(dupMenuItem);
    movID->setEnabled(moveMenuItem);
    copyID->setEnabled(copyMenuItem);
    pasteID->setEnabled(pasteMenuItem);

    cutID->setEnabled(copyMenuItem);
    delID->setEnabled(delMenuItem);

    if (newID1)
        newID1->setEnabled(newMenuItem);
    if (newID2)
        newID2->setEnabled(newMenuItem);


    if (inDeletedObjects) movID->setText( tr("Undelete...") );

    popup->exec( objTreeView->mapToGlobal( pos ) );
}

void ObjectManipulator::getMenuState(bool haveMoveTargets,
                                     bool &dupMenuItem,
                                     bool &moveMenuItem,
                                     bool &copyMenuItem,
                                     bool &pasteMenuItem,
                                     bool &delMenuItem,
                                     bool &newMenuItem,
                                     bool &inDeletedObjects)
{
    dupMenuItem=true;
    moveMenuItem=true;
    copyMenuItem=true;
    pasteMenuItem=true;
    delMenuItem=true;
    newMenuItem=true;

    inDeletedObjects = false;

    FWObject *delObjLib =
        m_project->db()->findInIndex( FWObjectDatabase::getDeletedObjectsId());

    vector<FWObject*> so = getCurrentObjectTree()->getSelectedObjects();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        FWObject *obj= *i;

        QString objPath = obj->getPath(true).c_str();

        copyMenuItem  = copyMenuItem && m_project->getCopyMenuState(objPath);
        pasteMenuItem = pasteMenuItem &&
            m_project->getPasteMenuState(objPath) &&
            (FWObjectClipboard::obj_clipboard->size()!=0);
        delMenuItem   = delMenuItem && m_project->getDeleteMenuState(objPath);

        if (pasteMenuItem)
        {
            /*
             *  enable Paste menu item only if object can be pasted
             */
            vector<string>::iterator i;
            for (i= FWObjectClipboard::obj_clipboard->begin();
                 i!=FWObjectClipboard::obj_clipboard->end(); ++i)
            {
                FWObject *co= m_project->db()->findInIndex(*i);
				if (co==NULL)
				{
					continue ;
					//QString s2 = obj->getTypeName().c_str();
				}
QString s3 = obj->getTypeName().c_str();
                FWObject *nobj=pasteTo( obj , co , false, true);
                pasteMenuItem = pasteMenuItem && (nobj!=NULL);
            }
        }

        dupMenuItem=
            (dupMenuItem && ! m_project->isSystem(obj) && ! Library::isA(obj) );

        inDeletedObjects = (delObjLib!=NULL && obj->isChildOf(delObjLib));
        dupMenuItem = dupMenuItem && !inDeletedObjects;

// can't move system objects
        moveMenuItem = moveMenuItem && ! m_project->isSystem(obj);

// can't move interfaces unless parent host object is also selected
        if ( Interface::isA(obj) &&
             std::find(so.begin(),so.end(),obj->getParent())==so.end())
            moveMenuItem = false;

// can't move ip addresses if parent is interface
        if (IPv4::isA(obj) && Interface::isA(obj->getParent()))
            moveMenuItem = false;

// can't move physAddress objects
        moveMenuItem = moveMenuItem && ! physAddress::isA(obj);

// can't move read-only objects
        moveMenuItem = moveMenuItem && ! obj->isReadOnly();

// can't move libraries unless in deleted objects
        if (Library::isA(obj) && ! inDeletedObjects) moveMenuItem = false;

// can't move if there is only one user-defined library in the tree
// but we dont care about number of libraries if this will become
// 'undelete' operation
        if (!haveMoveTargets && ! inDeletedObjects) moveMenuItem = false;

//        copyMenuItem= (copyMenuItem &&
//                       ! m_project->isSystem(currentObj) &&
//                       ! Library::isA(currentObj));
//        delMenuItem= (delMenuItem && ! m_project->isSystem(currentObj));

        newMenuItem= (newMenuItem && ! obj->isReadOnly() );
        Interface *intf = Interface::cast(obj);
        if (intf &&
            (intf->isDyn() ||
             intf->isUnnumbered() ||
             intf->isBridgePort())
        )
            newMenuItem = false;

    }
}

void ObjectManipulator::find()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    m_project->setFDObject(obj);
}
void ObjectManipulator::findObject()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    m_project->findObject( obj );
}

void ObjectManipulator::dumpObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    obj->dump(false,false);
}

void ObjectManipulator::compile()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

    set<Firewall*> fo;
    filterFirewallsFromSelection(so,fo);

    //FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    //if (obj==NULL) return;
    //m_project->showFirewall(obj);
    if (fwbdebug)
        qDebug("ObjectManipulator::compile filtered %d firewalls",fo.size());
    m_project->compile(fo);
}
void ObjectManipulator::filterFirewallsFromSelection(vector<FWObject*> &so,set<Firewall*> &fo)
{
    Firewall *fw;
    ObjectGroup *gr;
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        fw= Firewall::cast( *i );
        if (fw!=NULL)
        {
            fo.insert(fw);
            continue;
        }
        gr=ObjectGroup::cast( *i);
        if (gr!=NULL)
        {
            extractFirewallsFromGroup(gr,fo);
        }
    }

}
void ObjectManipulator::extractFirewallsFromGroup(ObjectGroup *gr,set<Firewall*> &fo)
{
   Firewall *f;
   set<FWObject*> oset;
   m_project->db()->findObjectsInGroup(gr,oset);

   set<FWObject*>::iterator i;
   for(i=oset.begin();i!=oset.end();++i)
   {
       f=Firewall::cast(*i);
       if (f!=NULL) fo.insert(f);
   }
}
void ObjectManipulator::install()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    //FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    //if (obj==NULL) return;
    //m_project->showFirewall(obj);

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    set<Firewall*> fo;
    filterFirewallsFromSelection(so,fo);



    m_project->install(fo);
}

FWObject* ObjectManipulator::duplicateObject(FWObject *targetLib,
                                             FWObject *obj,
                                             const QString &name,
                                             bool  askForAutorename)
{
    if (!isTreeReadWrite(this, targetLib)) return NULL;

    openLib(targetLib);

    FWObject *o=NULL;

    QString n;
    if (!name.isEmpty()) n=name;
    else                 n=QString::fromUtf8(obj->getName().c_str());

    o=createObject(obj->getTypeName().c_str(), n, obj);
    if (o)
    {
      openObject(o);
      if (!o->isReadOnly() && (Host::isA(o) || Firewall::isA(o) || Interface::isA(o)) )
        autorename(o,askForAutorename);
    }
    return o;
}

void ObjectManipulator::duplicateObj(QAction *action)
{
    int libid = action->data().toInt();
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    ObjectTreeView* ot=getCurrentObjectTree();
    ot->freezeSelection(true);
    FWObject *obj;
    FWObject *nobj = NULL;
    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;

        if ( m_project->isSystem(obj) || Interface::isA(obj) ) continue;

        FWObject *cl   = idxToLibs[libid];

        nobj = duplicateObject(cl,obj,"",false);

        if (nobj->getTypeName()==Firewall::TYPENAME)
        {
            m_project->addFirewallToList(nobj);
            m_project->showFirewall(nobj);
        }
    }
    editObject(nobj);
    ot->freezeSelection(false);
}

void ObjectManipulator::duplicateObjUnderSameParent()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    ObjectTreeView* ot=getCurrentObjectTree();
    ot->freezeSelection(true);
    FWObject *obj;
    FWObject *o = NULL;
    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;

        o=NULL;

        QString n=QString::fromUtf8(obj->getName().c_str());

        openObject(o=createObject(obj->getParent(),
                                  obj->getTypeName().c_str(), n, obj));

        if (Host::isA(o) || Firewall::isA(o) || Interface::isA(o))
            autorename(o,false);

        if (Firewall::isA(o))
        {
            m_project->addFirewallToList(o);
            m_project->showFirewall(o);
        }
    }
    if (o!=NULL)   editObject(o);
    ot->freezeSelection(false);
}

void ObjectManipulator::moveObject(FWObject *targetLib, FWObject *obj)
{
    FWObject *cl=getCurrentLib();
    if (cl==targetLib) return;

//    bool inDeletedObjects = (obj->getParent()->getId()==FWObjectDatabase::getDeletedObjectsId());

//    QString parentType;
//    QString parentName;
    FWObject *grp = NULL;

    if (FWObjectDatabase::isA(targetLib))        grp = targetLib;
    else
    {
        grp=m_project->getStandardSlotForObject(targetLib,
                                              obj->getTypeName().c_str());
    }

    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  grp= %p", grp);

    if (grp==NULL) grp=targetLib;

    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  grp= %s",grp->getName().c_str());

    if (!grp->isReadOnly())
    {
        obj->ref();
        obj->ref();

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  removing from the widget");

        ObjectTreeViewItem *itm = allItems[obj];
        if (itm->parent()==NULL) return;

        itm->parent()->takeChild(itm->parent()->indexOfChild(itm));

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  removing from the tree");

        obj->getParent()->remove(obj);

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  adding to the tree");

        grp->add(obj);
        obj->unref();

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  adding to the widget");

        if (allItems[grp]==NULL)
        {
/* adding to the root, there is not such tree item */
            if (Library::isA(obj))
            {
                addTreePage(obj);
                openLib(obj);
            } else
            {
/* it screwed up, just print debugging message */
                if (fwbdebug)
                    qDebug("ObjectManipulator::moveObject  no place in the tree corresponding to the object %p %s",grp,grp->getName().c_str());
            }
        } else
            allItems[grp]->addChild(itm);

        if (Firewall::cast(obj)!=NULL)
        {
            m_project->addFirewallToList(obj);
            m_project->showFirewall(obj);
        }
    }

//    if (fwbdebug)
//        qDebug("ObjectManipulator::moveObject  open lib cl %s",
//               cl->getName().c_str());
//    openLib(cl);

    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  all done");
}

/*
 *  targetLibName is the name of the target library in Unicode
 */
void ObjectManipulator::moveObject(const QString &targetLibName,
                                   FWObject *obj)
{
    list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib=*i;
        if (targetLibName==QString::fromUtf8(lib->getName().c_str()))
        {
            if (fwbdebug)
                qDebug("ObjectManipulator::moveObject  found lib %s",
                       lib->getName().c_str() );

            moveObject(lib,obj);
        }
    }
}

void ObjectManipulator::moveObj(QAction* action)
{
    int libid = action->data().toInt();

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    ObjectTreeView* ot=getCurrentObjectTree();
    ot->freezeSelection(true);
    FWObject *obj;

    FWObject *targetLib   = idxToLibs[libid];

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;

        if (fwbdebug)
        {
            qDebug("ObjectManipulator::moveObj  obj=%p  obj: %s",
                   obj, obj->getName().c_str() );
        }
        if (Library::isA(obj))
        {
/* We can only move library to the root of the tree. This case only
 * happens when user tries to undelete a library.
 */
            moveObject(m_project->db(),obj);
        } else
        {
            if (obj->isChildOf(targetLib)) continue;

            if ( m_project->isSystem(obj) ||
                 Interface::isA(obj)    ||
                 Interface::isA(obj->getParent())) continue;

            moveObject(targetLib,obj);
        }
    }
    ot->freezeSelection(false);
}

void ObjectManipulator::copyObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    FWObject *obj;
    FWObjectClipboard::obj_clipboard->clear();

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        if ( ! m_project->isSystem(obj) )
            FWObjectClipboard::obj_clipboard->add( obj,m_project );
    }
}

void ObjectManipulator::cutObj()
{
    copyObj();
    deleteObj();   // works with the list getCurrentObjectTree()->getSelectedObjects()
}

void ObjectManipulator::pasteObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    vector<string>::iterator i;
	int idx = 0;
    for (i= FWObjectClipboard::obj_clipboard->begin(); i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
		FWObject *co= FWObjectClipboard::obj_clipboard->getObjectByIdx(idx); //win->db()->findInIndex(*i);
        FWObject *nobj=pasteTo( obj , co );
        if (nobj!=NULL)
        {
            if (Firewall::isA(nobj)) m_project->addFirewallToList(nobj);
            if (Firewall::isA(obj))  m_project->showFirewall(obj);
        }
    }
}



FWObject*  ObjectManipulator::pasteTo(FWObject *target,FWObject *obj,
                                      bool openobj,bool validateOnly, bool renew_id)
{
    FWObject *ta=target;
    if (IPv4::isA(ta)) ta=ta->getParent();
    try
    {
/* clipboard holds a copy of the object */
//        if (ta->getTypeName()==obj->getTypeName()) ta=ta->getParent();

        Host      *hst  = Host::cast(ta);   // works for firewall, too
        Interface *intf = Interface::cast(ta);

        if (m_project->isSystem(ta))
        {
            if (!m_project->validateForInsertion(ta,obj))
            {
                if (validateOnly) return NULL;

                QMessageBox::warning(
                    this,"Firewall Builder",
                    QObject::tr("Impossible to insert object %1 (type %2) into %3\nbecause of incompatible type.")
                    .arg(obj->getName().c_str())
                    .arg(obj->getTypeName().c_str())
                    .arg(target->getName().c_str()),
                    "&Continue", QString::null, QString::null,
                    0, 1 );

                return obj;
            }
        }

        if ( m_project->isSystem(ta) ||
            (hst!=NULL  && hst->validateChild(obj)) ||
            (intf!=NULL && intf->validateChild(obj))
        )
        {
            if (validateOnly) return obj;

/* add a copy of the object to system group */

            FWObject *nobj=
                m_project->db()->create(obj->getTypeName());
            assert (nobj!=NULL);
            nobj->ref();
            nobj->duplicate(obj,renew_id);   //if renew_id == true creates new object ID

            makeNameUnique(ta,nobj);
            ta->add( nobj );
            insertSubtree( allItems[ta], nobj);

            if (openobj) openObject(nobj);

            return nobj;
        }

        Group *grp=Group::cast(ta);

        if (grp!=NULL && grp->validateChild(obj))
        {
            if (validateOnly) return obj;

/* check for duplicates. We just won't add an object if it is already there */
            string cp_id=obj->getId();
            list<FWObject*>::iterator j;
            for(j=grp->begin(); j!=grp->end(); ++j)
            {
                FWObject *o1=*j;
                if(cp_id==o1->getId()) return o1;

                FWReference *ref;
                if( (ref=FWReference::cast(o1))!=NULL &&
                    cp_id==ref->getPointerId()) return o1;
            }

            grp->addRef(obj);
            if (openobj) openObject(grp);
        }
    }
    catch(FWException &ex)
    {
        if (validateOnly) return NULL;

        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
    }

    if (validateOnly) return NULL;
    return obj;
}



void ObjectManipulator::lockObject()
{

    if (fwbdebug)
        qDebug("ObjectManipulator::lockObject selected %d objects ",
               getCurrentObjectTree()->getNumSelected());

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        FWObject *lib = obj->getLibrary();
        // these lbraries are locked anyway, do not let the user
        // lock objects inside because they won't be able to unlock them.
        if (lib->getId()!=STANDARD_LIB && lib->getId()!=TEMPLATE_LIB)
            obj->setReadOnly(true);
    }
    getCurrentObjectTree()->setLockFlags();
    getCurrentObjectTree()->updateTreeItems();
}

void ObjectManipulator::unlockObject()
{
    if (fwbdebug)
        qDebug("ObjectManipulator::unlockObject selected %d objects ",
               getCurrentObjectTree()->getNumSelected());

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        FWObject *lib = obj->getLibrary();
        if (lib->getId()!=STANDARD_LIB && lib->getId()!=TEMPLATE_LIB)
            obj->setReadOnly(false);
    }
    getCurrentObjectTree()->setLockFlags();
    getCurrentObjectTree()->updateTreeItems();
}

void ObjectManipulator::deleteObj()
{
    if (fwbdebug)
        qDebug("ObjectManipulator::deleteObj selected %d objects ",
               getCurrentObjectTree()->getNumSelected());

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj;
    bool emptyingTrash      = false;
    bool emptyingTrashInLib = false;

    FWObject *delObjLib = m_project->db()->findInIndex(FWObjectDatabase::getDeletedObjectsId());
    if (fwbdebug)
        qDebug("ObjectManipulator::deleteObj  delObjLib=%p",delObjLib);

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

    if (delObjLib!=NULL)
    {
        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
        {
            obj= *i;
            emptyingTrash |= obj->isChildOf(delObjLib);
        }
    }

    emptyingTrashInLib = emptyingTrash && m_project->editingLibrary();

/* Ask user iff:
 *
 * we are emptying trash while editing library file (.fwl)
 *    else
 *
 * if we are not emptying Trash (i.e. not deleting "Deleted objects" library)
 *    and
 * (we delete more than one object
 *    or
 * we delete one object and it is not a library (because in this case
 * we ask them later anyway))
 */

    QString msg;

    if (emptyingTrashInLib)
    {
        msg = tr(
    "Emptying the 'Deleted Objects' in a library file is not recommended.\n"
    "When you remove deleted objects from a library file, Firewall Builder\n"
    "loses ability to track them. If a group or a policy rule in some\n"
    "data file still uses removed object from this library, you may encounter\n"
    "unusual and unexpected behavior of the program.\n"
    "Do you want to delete selected objects anyway ?"
        );
        if (QMessageBox::warning(
                this,"Firewall Builder", msg,
                tr("&Yes"), tr("&No"), QString::null,
                0, 1 )!=0) return;
    } else
    {

        if (fwbdebug)
            qDebug("ObjectManipulator::deleteObj emptyingTrash=%d so.size=%d  so.front()->type=%s",
                   emptyingTrash,
                   so.size(),
                   so.front()->getTypeName().c_str() );


        if (!emptyingTrash && (so.size()>1 || !Library::isA(so.front())))
        {
            /*
            msg = tr(
                "When you delete an object, it is removed from the tree and\n"
                "all groups and firewall policy rules that reference it.\n"
                "Do you want to delete selected objects ?"
            );
            if (QMessageBox::warning(
                    this,"Firewall Builder", msg,
                    tr("&Yes"), tr("&No"), QString::null,
                    0, 1 )!=0) return;
                    */
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
            ConfirmDeleteObjectDialog * dlg= new ConfirmDeleteObjectDialog(this);

            dlg->load(so);

            QApplication::restoreOverrideCursor();
            if(dlg->exec()==QDialog::Rejected ) return;
        }
    }


/* need to work with a copy of the list of selected objects because
 * some of the methods we call below clear list
 * getCurrentObjectTree()->getSelectedObjects()
 */

    vector<FWObject*> so2 = so;

    if (fwbdebug)
    {
        for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
        {
            obj= *i;
            qDebug("ObjectManipulator::deleteObj will delete obj=%p ( %s %s ) ",
                   obj, obj->getTypeName().c_str(), obj->getName().c_str());
        }
    }

    try
    {
        for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
        {
            obj= *i;

//        openObject(obj,false);

            if ( ! m_project->isSystem(obj) )
            {
                if (Library::isA(obj))
                {
                    list<FWObject*> ll=m_project->db()->getByType(Library::TYPENAME);
                    if (ll.size()==1)  return;

                    if (QMessageBox::warning(
                            this,"Firewall Builder",
                            tr(
"When you delete a library, all objects that belong to it\n"
"disappear from the tree and all groups and rules that reference them.\n"
"You won't be able to reverse this operation later.\n"
"Do you still want to delete library %1?")
                            .arg(QString::fromUtf8(obj->getName().c_str())),
                            tr("&Yes"), tr("&No"), QString::null,
                            0, 1 )!=0 ) continue;
                }

                if (m_project->isEditorVisible() && m_project->getOpenedEditor()==obj) m_project->hideEditor();

                delObj(obj);
            }
        }
    }
    catch(FWException &ex)
    {
    }
}

void ObjectManipulator::delObj(FWObject *obj,bool openobj)
{
    if (obj->getId()==STANDARD_LIB || obj->getId()==DELETED_LIB) return;

    m_project->findObjectWidget->reset();
    try
    {
        if (fwbdebug)
            qDebug("ObjectManipulator::delObj  delete obj %p %s openobj=%d",
                   obj,obj->getName().c_str(),openobj);

        FWObject *parent=obj->getParent();
        FWObject *delObjLib = m_project->db()->findInIndex( DELETED_LIB );

        if (fwbdebug)
            qDebug("ObjectManipulator::delObj  deleted obj lib %p",
                   delObjLib);

        bool islib  = Library::isA(obj);
//        bool isintf = (Interface::isA(obj) && Firewall::isA(parent));
        bool isfw   = Firewall::isA(obj);
        bool isDelObj = (delObjLib!=NULL && obj->isChildOf(delObjLib));

        if (!islib && !isDelObj && obj->getId()!=TEMPLATE_LIB)
            updateLastModifiedTimestampForAllFirewalls(obj);

        if (fwbdebug)
            qDebug("ObjectManipulator::delObj  delete islib=%d isfw=%d isDelObj=%d",islib,isfw,isDelObj);

/*
 * TODO: we have to remove not only the object, but also all its child
 * objects from the database, as well as all references to them. This
 * logic should really be in FWObject::removeAllInstances(FWObject*);
 */

/* remove from our internal tables before it is removed from the
 * object tree so we could use obj->getId()
 */
        if (islib && !isDelObj)
        {
            int idx = getIdxForLib(obj);
            QTreeWidget *otv = idxToTrees[idx];
            assert(otv!=NULL);
            m_objectManipulator->widgetStack->removeWidget( otv );
            removeLib(idx);

            list<FWObject*> fl;
            findFirewalls(obj, fl);
            for (list<FWObject*>::iterator i=fl.begin(); i!=fl.end(); i++)
                m_project->deleteFirewall( *i );
        }

        if (isfw && !isDelObj) m_project->deleteFirewall(obj);


//        removeObjectFromTreeView(obj);

        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

        if (islib && obj->isReadOnly()) obj->setReadOnly(false);

        if (obj->getId()==TEMPLATE_LIB) // special case
        {
            if (fwbdebug) qDebug("ObjectManipulator::delObj:   special case: deleting template library");
            m_project->db()->removeAllInstances(obj);
        } else
        {
            if (fwbdebug) qDebug("ObjectManipulator::delObj:   recursively deleting library and all its objects");
            m_project->db()->recursivelyRemoveObjFromTree(obj, false);
            if (islib) parent=m_project->db()->getFirstByType(Library::TYPENAME);
        }

        QApplication::restoreOverrideCursor();
        if (fwbdebug) qDebug("ObjectManipulator::delObj:   done");

        removeObjectFromTreeView(obj);
        m_project->scheduleRuleSetRedraw();

        if (!isDelObj)
        {
            if (allItems[delObjLib]!=NULL)
                insertSubtree( allItems[delObjLib], obj );
        } else
            FWObjectClipboard::obj_clipboard->clear();

        if (openobj)
        {
            if (isfw && !isDelObj)
            {
                std::list<Firewall*> fwlist;
                findAllFirewalls(fwlist);
                if (fwlist.size()>0)
                {
                    FWObject *first_fw = fwlist.front();
                    if (first_fw!=NULL)
                    {
                        m_project->showFirewall( first_fw );
                        openObject( first_fw );
                    }
                }
                //QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );
            } else {
                openObject(parent);
            }
        }
    }
    catch(FWException &ex)
    {
        if (fwbdebug) qDebug("ObjectManipulator::delObj: catch:  restoreOverrideCursor");
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
        throw(ex);
    }
}

void ObjectManipulator::groupObjects()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *co = getCurrentObjectTree()->getSelectedObjects().front();

    newGroupDialog ngd( this );

    if (ngd.exec()==QDialog::Accepted)
    {
        QString objName = ngd.m_dialog->obj_name->text();
        QString libName = ngd.m_dialog->libs->currentText();

        QString type = ObjectGroup::TYPENAME;
        if (Service::cast(co)!=NULL)  type=ServiceGroup::TYPENAME;
        if (Interval::cast(co)!=NULL) type=IntervalGroup::TYPENAME;

        FWObject *newgrp=NULL;

        list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
        {
            FWObject *lib=*i;
            if (libName==QString::fromUtf8(lib->getName().c_str()))
            {
/* TODO: need to show a dialog and say that chosen library is read-only.
 * this is not critical though since newGroupDialog fills the pull-down
 * only with names of read-write libraries
 */
                if (lib->isReadOnly()) return;
                FWObject *parent = m_project->getStandardSlotForObject(lib,type);
                if (parent==NULL)
                {
                    if (fwbdebug)
                        qDebug("ObjectManipulator::groupObjects(): could not find standard slot for object of type %s in library %s",
                               type.toAscii().constData(),lib->getName().c_str());
                    return;
                }
                newgrp = createObject(parent,type,objName);

                break;
            }
        }
        if (newgrp==NULL) return;

        FWObject *obj;

        ObjectTreeView* ot=getCurrentObjectTree();
        ot->freezeSelection(true);

        vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
        {
            obj= *i;
            newgrp->addRef(obj);
        }
        ot->freezeSelection(false);

        openObject(newgrp);
        editObject(newgrp);
    }
}

void ObjectManipulator::info()
{
    if (fwbdebug) qDebug("ObjectManipulator::info()");

    if (currentObj)
    {
        m_project->info(currentObj, true); //forcing info window update
        active=true;
    }
    if (fwbdebug) qDebug("/ObjectManipulator::info()");
}


void ObjectManipulator::restoreSelection(bool same_widget)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::restoreSelection  same_widget=%d",same_widget);

    select();
    openObject( m_project->getOpenedEditor(), false);
}

void ObjectManipulator::editSelectedObject()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    editObject(obj);
}

bool ObjectManipulator::editObject(FWObject *obj)
{
    if (!m_project->isEditorVisible()) m_project->showEditor();
    return switchObjectInEditor(obj);
}

bool ObjectManipulator::switchObjectInEditor(FWObject *obj)
{
    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor");

    m_project->unselectRules();

    if (!m_project->isEditorVisible()) return false;

    if (!m_project->requestEditorOwnership(this,
                                    obj,
                                    ObjectEditor::optNone,
                                    true))
        return false;

    select();

    if (obj!=m_project->getOpenedEditor())
    {
        m_project->openEditor(obj);
        currentObj=obj;
        active=true;
        openObject(obj);  // position the tree so that obj is visible
    }
    return true;      // successfully (re)opened obj in the editor
}


void ObjectManipulator::openObject(ObjectTreeViewItem *otvi,
                                   bool register_in_history)
{
    openObject(otvi->getFWObject(),register_in_history);
}

/* This method is called from the GroupObjectDialog when user double
 * clicks on the object in a group, so first we should check if this
 * object is shown in the tree and if not, find and open it.
 */
void ObjectManipulator::openObject(FWObject *obj, bool /*register_in_history*/)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::openObject   obj=%s",
               (obj)?obj->getName().c_str():"NULL");

    if (obj==NULL) return;

    raise();
    FWObject *o=obj;
    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    ObjectTreeViewItem *otvi=allItems[o];
// this changes selection and thus calls slot slectionChanged
    showObjectInTree(otvi);

    m_objectManipulator->libs->setCurrentIndex( getIdxForLib( obj->getLibrary() ) );
    updateCreateObjectMenu( obj->getLibrary() );
}

void ObjectManipulator::selectionChanged(QTreeWidgetItem *cur)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::selectionChanged");

    QTreeWidget *qlv= getCurrentObjectTree();
    if (qlv==NULL) return;

    ObjectTreeViewItem* otvi=dynamic_cast<ObjectTreeViewItem*>(cur);

    if (otvi==NULL) return;

    FWObject *obj=otvi->getFWObject();
    if (obj==NULL) return;

    FWObject *o=obj;
//    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    if (history.empty() || otvi!=history.top().item() )
    {
        mw->m_mainWindow->backAction->setEnabled( true );
        history.push( HistoryItem(otvi,o->getId().c_str()) );
    }

    currentObj = obj;

    bool dupMenuItem=true;
    bool moveMenuItem=true;
    bool copyMenuItem=true;
    bool pasteMenuItem=true;
    bool delMenuItem=true;
    bool newMenuItem=true;
    bool inDeletedObjects = false;

    getMenuState(false,
                 dupMenuItem,moveMenuItem,copyMenuItem,pasteMenuItem,
                 delMenuItem,newMenuItem,inDeletedObjects);

    mw->m_mainWindow->editCopyAction->setEnabled(copyMenuItem);
    mw->m_mainWindow->editDeleteAction->setEnabled(delMenuItem);
    mw->m_mainWindow->editCutAction->setEnabled(copyMenuItem);
    mw->m_mainWindow->editPasteAction->setEnabled(pasteMenuItem);

    active=true;

/*
    m_project->unselectRules();
    if (m_project->validateAndSaveEditor())
    {
        //oe->selectionChanged(obj);
    }
*/
    info();
    select();
}

/*
 * I could use default value for the parameter register_in_history,
 * but that caused problems when this method was used as a slot
 */
void ObjectManipulator::openObject(QTreeWidgetItem *item)
{
    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    openObject(otvi,true);
}

void ObjectManipulator::openObject(FWObject *obj)
{
    openObject(obj,true);
}

void ObjectManipulator::showObjectInTree(ObjectTreeViewItem *otvi)
{
    if (fwbdebug) qDebug("ObjectManipulator::showObjectInTree");
    if (otvi==NULL) return;

    ObjectTreeView* otv = otvi->getTree();

    if (fwbdebug) qDebug("ObjectManipulator::showObjectInTree  current_tree_view=%p  new_otv=%p",current_tree_view,otv);

    otv->raise();
    m_objectManipulator->widgetStack->setCurrentWidget(otv);

    otvi->getTree()->clearSelection();
    otvi->getTree()->scrollToItem( otvi );
    otvi->getTree()->setCurrentItem( otvi );
    otvi->setSelected( true );
}

void ObjectManipulator::invalidateDialog()
{
    currentObj=NULL;
}

void ObjectManipulator::libChanged(int ln)
{
    QTreeWidget *lv = idxToTrees[ln];
    assert(lv!=NULL);

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(lv->currentItem());

    if (otvi == NULL)
        if (lv->invisibleRootItem()->childCount() > 0)
            otvi = dynamic_cast<ObjectTreeViewItem*>(lv->invisibleRootItem()->child(0));
        else
            assert(FALSE);

    currentObj=otvi->getFWObject();
    showObjectInTree( otvi );

    info();

    updateCreateObjectMenu( idxToLibs[ln] );
    return;
}

void ObjectManipulator::updateCreateObjectMenu(FWObject* lib)
{
    bool      f   =
        lib->getId()==STANDARD_LIB ||
        lib->getId()==TEMPLATE_LIB ||
        lib->getId()==DELETED_LIB  ||
        lib->isReadOnly();

    m_objectManipulator->newButton->setEnabled( !f );
    QAction *noa = (QAction*)(mw->findChild<QAction*>("newObjectAction"));
    noa->setEnabled( !f );
}

void ObjectManipulator::back()
{
//    if (!validateDialog()) return;

    if (!history.empty())
    {
        history.pop();

/* skip objects that have been deleted */
        while ( ! history.empty())
        {
            if (m_project->db()->findInIndex( history.top().id().toLatin1().constData() )!=NULL) break;
            history.pop();
        }

        if (history.empty())
        {
            mw->m_mainWindow->backAction->setEnabled( false );
            return;
        }

        openObject( history.top().item(), false );

        if (m_project->isEditorVisible())
        {
            ObjectTreeViewItem *otvi=history.top().item();
            switchObjectInEditor(otvi->getFWObject());
        }
    }
}

FWObject*  ObjectManipulator::getCurrentLib()
{
    return idxToLibs[ m_objectManipulator->libs->currentIndex() ];
}

ObjectTreeView* ObjectManipulator::getCurrentObjectTree()
{
    return current_tree_view;
}

void ObjectManipulator::openLib(FWObject *obj)
{
    openObject(obj->getLibrary(),false);
}

void ObjectManipulator::newObject()
{
//    QToolButton *btn = (QToolButton*)(m_project->toolBar)->child("newObjectAction_action_button");
    m_objectManipulator->newButton->showMenu();
}

FWObject* ObjectManipulator::createObject(const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom)
{
    if (!validateDialog()) return NULL;

    if (fwbdebug) qDebug("ObjectManipulator::createObject   check 1");

    FWObject *lib  = getCurrentLib();
    int       i = 0;

    if (fwbdebug)
    {
        qDebug("lib: %s %s",lib->getName().c_str(), lib->getId().c_str());
        qDebug("lib: isReadOnly=%d isLoaded=%d",
               lib->isReadOnly(), m_project->getAddOnLibs()->isLoaded( lib->getName().c_str() ) );
        qDebug("libs->count()=%d", m_objectManipulator->libs->count() );
    }

    while ( lib->getId()==STANDARD_LIB ||
            lib->getId()==TEMPLATE_LIB ||
            lib->getId()==DELETED_LIB  ||
            lib->isReadOnly() )
    {
        if (i>=m_objectManipulator->libs->count())
        {
//            if (fwbdebug)
//                qDebug("ObjectManipulator::createObject   return NULL");
//            return NULL;
            lib  = getCurrentLib();
            break;
        }

        lib= idxToLibs[i];

        if (fwbdebug)
        {
            qDebug("i=%d",i);
            qDebug("lib: %s %s",lib->getName().c_str(), lib->getId().c_str());
            qDebug("lib: isReadOnly=%d isLoaded=%d",
                   lib->isReadOnly(), m_project->getAddOnLibs()->isLoaded( lib->getName().c_str() ) );
        }
        i++;
    }

    FWObject *parent=m_project->getStandardSlotForObject(lib, objType);
    if (parent==NULL)
    {

      QMessageBox::warning(this,"Firewall Builder",
                           QObject::tr(
"Type '%1': new object can not be created because\n"
"corresponding branch is missing in the object tree.\n"
"Please repair the tree using command 'fwbedit -s -f file.fwb'.")
                           .arg(objType),
                           "&Continue", QString::null, QString::null,
                           0, 1 );


      return NULL;
    }
    return actuallyCreateObject(parent,objType,objName,copyFrom);
}

FWObject* ObjectManipulator::createObject(FWObject *parent,
                                       const QString &objType,
                                       const QString &objName,
                                       FWObject *copyFrom)
{
    if (!validateDialog()) return NULL;

    FWObject *lib  = getCurrentLib();
    int       i = 0;

    assert(parent!=NULL);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::createObject 2: parent=%s",
               parent->getName().c_str());
        qDebug("ObjectManipulator::createObject 2: objType=%s  objName=%s",
               objType.toLatin1().constData(), objName.toLatin1().constData());
    }

    while ( lib->getId()==STANDARD_LIB ||
            lib->getId()==TEMPLATE_LIB ||
            lib->getId()==DELETED_LIB  ||
            lib->isReadOnly() )
    {
        if (i >= m_objectManipulator->libs->count())
        {
            lib=getCurrentLib();
            break;
        }
        lib= idxToLibs[i];
        i++;
    }

    if (parent==NULL) parent=lib;

    return actuallyCreateObject(parent,objType,objName,copyFrom);
}


FWObject* ObjectManipulator::copyObj2Tree(const QString &objType, const QString &objName,
         libfwbuilder::FWObject *copyFrom, FWObject *parent, bool askLib)
{
    if (!validateDialog()) return NULL;


   // Firewall * nobj_ = new Firewall();
    
    // FWObject *nobj_= m_project->db()->create(copyFrom->getTypeName());
    //                nobj_->ref();
    //                nobj_->duplicate(copyFrom,false);   //if renew_id == true creates new object ID
    //                nobj_->setReadOnly(false);

    FWObject *lib  = getCurrentLib();
    if (askLib)
        lib = AskLibForCopyDialog::askLibForCopyDialog(m_project, m_project->db(), lib);
    if (!lib)
        return 0;
    if (!parent)
        parent=m_project->getStandardSlotForObject(lib, objType);
    ids.clear();
    return copyObjWithDeep(copyFrom);
/*

    FWObject *nobj = pasteTo (parent, copyFrom, true, false, false); 
    FWObject * nobj_ = nobj ; 
   // nobj_->setReadOnly(false);
   // nobj_->setRoot(copyFrom->getRoot());
    list<FWObject*> refs;
    map<const std::string, FWObject*> objByIds;
    list<FWReference*> refLinfs ;
    m_project->check4Depends(copyFrom, refs,refLinfs, lib);
    list<FWReference*>::iterator k = refLinfs.begin();
    for(list<FWObject*>::iterator i=refs.begin(); i!=refs.end(); ++i)
    {
        FWObject *o = (*i);
        FWReference * ref = (*k);
        ++k;
        if (o && Firewall::isA(o))
        {
            continue ;
        }
        if (m_project->db()->getRoot()->getById(o->getId())==NULL)
        {
        FWObject * lib2 = idxToLibs[1];
        FWObject *par = m_project->getStandardSlotForObject(lib2, o->getTypeName().c_str());
        FWObject * newobj = nobj_->getById(o->getId());
        FWObject *no  = pasteTo (par, o, true, false, false);
        //ref->setPointerId(no->getId());
        //o->setReadOnly(false);
        //o->setId(no->getId());
        //if (newobj!=NULL)
        //{
        //    newobj->setReadOnly(false);
        //    newobj->setId(no->getId());
        //}
        objByIds[no->getId()] = no;
        }
        
    }
    if (nobj && Firewall::isA(nobj))
    {
        m_project->addFirewallToList(nobj);
        m_project->showFirewall(nobj);
    }
*/
//    return nobj;
}

libfwbuilder::FWObject * ObjectManipulator::copyObjWithDeep(libfwbuilder::FWObject *copyFrom)
{
    
    if (copyFrom==NULL)
        return NULL;
    FWObject *nobj= copyFrom;//m_project->db()->create(copyFrom->getTypeName());
    if (ids.contains(nobj->getId().c_str()))
    {
        
        return nobj;
    }
    else
    {
        ids.insert(nobj->getId().c_str());
    }
    //nobj->duplicate(copyFrom,false); 
    FWReference * ref = FWReference::cast(nobj);
    if (ref!=NULL)
    {
        FWObject * obj = copyObjWithDeep(ref->getPointer());
        return ref ;
    }

    Group * group = Group::cast(nobj);
    if (group!=NULL)
    {
        for (list<FWObject*>::iterator i=nobj->begin() ; i!=nobj->end(); ++i)
        {
            copyObjWithDeep(*i);
        }
        return group;
        //return group;
    }
    Firewall * fw = Firewall::cast(nobj);
    if (fw!=NULL)
    {
        for (libfwbuilder::FWObject::iterator i=fw->begin(); i!=fw->end(); i++)
        {
            
            RuleSet *rule = RuleSet::cast(*i);
            if (rule==NULL)
                continue;
            copyObjWithDeep(rule);
            //FWOptions  *ropt = rule->getOptionsObject();
            //if (ropt)
            //    extRefs.push_back(ropt);
        }        
    }
    RuleSet * ruleset = RuleSet::cast(nobj);
    if (ruleset!=NULL)
    {
        
        for (list<FWObject*>::iterator i=ruleset->begin() ; i!=ruleset->end(); ++i)
        {
            copyObjWithDeep(*i);
        }
    }

    Rule * rule = Rule::cast(nobj);
    if (rule!=NULL)
    {
            for (int col =0; col < 5; col++)
            {
                RuleElement *re = m_project->getRE(rule, col);
                if (!re) continue;
                copyObjWithDeep(re);
            } 
            return rule;
    }
    FWObject * lib = getCurrentLib();

    if (lib->getRoot()->getById(nobj->getId(),true)==NULL)
    {
    FWObject *par = m_project->getStandardSlotForObject(lib, nobj->getTypeName().c_str());
    FWObject *no  = pasteTo (par, nobj, true, false, false);
    if (no && Firewall::isA(no))
    {
        m_project->addFirewallToList(no);
        m_project->showFirewall(no);
    }

    }


}


FWObject* ObjectManipulator::actuallyCreateObject(FWObject *parent,
                                                  const QString &objType,
                                                  const QString &objName,
                                                  FWObject *copyFrom)
{
    if (!isTreeReadWrite(this, parent)) return NULL;
    FWObject *nobj = m_project->db()->create(objType.toLatin1().constData());
    assert(nobj!=NULL);

    if (copyFrom!=NULL) nobj->duplicate(copyFrom,true);
    if (nobj->isReadOnly()) nobj->setReadOnly(false);

    nobj->setName( string(objName.toUtf8().constData()) );
    makeNameUnique(parent,nobj);

    parent->add(nobj);
    insertSubtree(allItems[parent], nobj);

    m_project->db()->setDirty(true);
    return nobj;
}

void ObjectManipulator::newLibrary()
{
    if (!validateDialog()) return;

    FWObject *nlib = m_project->createNewLibrary(m_project->db());

    addTreePage( nlib );

    openObject( nlib );
    editObject(nlib);
}

void ObjectManipulator::newFirewall()
{
    newFirewallDialog *nfd=new newFirewallDialog();
    if (m_project->isEditorVisible()) m_project->hideEditor();
    nfd->exec();
    FWObject *o = nfd->getNewFirewall();
    delete nfd;

    if (o!=NULL)
    {
        openObject(o);

        m_project->addFirewallToList(o);
        m_project->showFirewall(o);
//        updateLastModifiedTimestampForAllFirewalls(o);

        editObject(o);
    }
}

void ObjectManipulator::newHost()
{
    newHostDialog *nhd=new newHostDialog();
    if (m_project->isEditorVisible()) m_project->hideEditor();
    nhd->exec();
    FWObject *o = nhd->getNewHost();
    delete nhd;

    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newInterface()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *i=NULL;

    if (Host::isA(currentObj) || Firewall::isA(currentObj))
        i=createObject(currentObj,Interface::TYPENAME,tr("New Interface"));

    if (Interface::isA(currentObj))
        i=createObject(currentObj->getParent(),Interface::TYPENAME,tr("New Interface"));

    if (i==NULL) return;

#ifdef USE_INTERFACE_POLICY
    if (Firewall::isA(i->getParent())) i->add(new InterfacePolicy());
#endif

    openObject( i );

    if (Firewall::isA(i->getParent())) m_project->showFirewall(i->getParent());
    updateLastModifiedTimestampForAllFirewalls(i);

    editObject(i);
}

void ObjectManipulator::newNetwork()
{
    FWObject *o=createObject(Network::TYPENAME,tr("New Network"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newAddress()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *o;
/*
 * Oleg reports that his expectation was that "New Address" should
 * always create an address object even if current selected object in
 * the tree is an interface. I tend to agree with him, this was a
 * usability issue because behavior of the program was different
 * depending on which object was selected in the tree. I am changing
 * it and will make it so "New Address" will always create a new
 * Address object uner Objects/Addresses. Interface address can be
 * created using context pop-up menu.
 *                                         12/19/04 --vk
 */
    o=createObject(IPv4::TYPENAME,tr("New Address"));

#if 0
    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
	QString iname=QString("%1:%2:ip")
	    .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
	    .arg(QString::fromUtf8(currentObj->getName().c_str()));
        o=createObject(currentObj, IPv4::TYPENAME, iname);
    }
    else
    {
        o=createObject(IPv4::TYPENAME,tr("New Address"));
    }
#endif


    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newDNSName()
{
    FWObject *o;
    o=createObject(DNSName::TYPENAME,tr("New DNS Name"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newAddressTable()
{
    FWObject *o;
    o=createObject(AddressTable::TYPENAME,tr("New Address Table"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newInterfaceAddress()
{
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
	QString iname=QString("%1:%2:ip")
	    .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
	    .arg(QString::fromUtf8(currentObj->getName().c_str()));
        FWObject *o=createObject(currentObj, IPv4::TYPENAME, iname);
        if (o!=NULL)
        {
            openObject(o);
            editObject(o);
            updateLastModifiedTimestampForAllFirewalls(o);
        }
    }
}

void ObjectManipulator::newTagService()
{
    FWObject *o;
    o=createObject(TagService::TYPENAME,tr("New TagService"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}
void ObjectManipulator::newPhysicalAddress()
{
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf->getByType(physAddress::TYPENAME).empty())
        {
            QString iname=QString("%1:%2:mac")
                .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
                .arg(QString::fromUtf8(currentObj->getName().c_str()));
            FWObject *o=createObject(currentObj,physAddress::TYPENAME,iname);
            if (o!=NULL)
            {
                openObject(o);
                editObject(o);
                updateLastModifiedTimestampForAllFirewalls(o);
            }
        }
    }
}

void ObjectManipulator::newAddressRange()
{
    FWObject *o;
    o=createObject(AddressRange::TYPENAME,tr("New Address Range"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newObjectGroup()
{
    FWObject *o;
    o=createObject(ObjectGroup::TYPENAME,tr("New Object Group"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}


void ObjectManipulator::newCustom()
{
    FWObject *o;
    o=createObject(CustomService::TYPENAME,tr("New Custom Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newIP()
{
    FWObject *o;
    o=createObject(IPService::TYPENAME,tr("New IP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newICMP()
{
    FWObject *o;
    o=createObject(ICMPService::TYPENAME,tr("New ICMP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newTCP()
{
    FWObject *o;
    o=createObject(TCPService::TYPENAME,tr("New TCP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newUDP()
{
    FWObject *o;
    o=createObject(UDPService::TYPENAME,tr("New UDP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newServiceGroup()
{
    FWObject *o;
    o=createObject(ServiceGroup::TYPENAME,tr("New Service Group"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}


void ObjectManipulator::newInterval()
{
    FWObject *o;
    o=createObject(Interval::TYPENAME,tr("New Time Interval"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}



bool ObjectManipulator::validateDialog()
{
    if (currentObj==NULL) return true;
    if (!m_project->isEditorVisible()) return true;
    return m_project->validateAndSaveEditor();
}

void ObjectManipulator::select()
{
    if (fwbdebug)
        qDebug("ObjectManipulator::select()");

    if (currentObj==NULL) return;
    ObjectTreeViewItem *otvi=allItems[currentObj];
    if (otvi)
    {
        otvi->setSelected(true);
        active=true;
        otvi->treeWidget()->setFocus();
        otvi->treeWidget()->update();
    }

    m_project->updateRuleSetViewSelection();
    
    if (fwbdebug)
        qDebug("/ObjectManipulator::select()");
}

void ObjectManipulator::unselect()
{
    if (currentObj==NULL) return;

    for (int i=0; i<m_objectManipulator->libs->count(); i++)
        idxToTrees[i]->clearSelection();

    active=false;
}

bool ObjectManipulator::isSelected()
{
    return active;
}

list<Firewall *> ObjectManipulator::findFirewallsForObject(FWObject *o)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::findFirewallsForObject");

    list<Firewall *> fws;
    set<FWObject *> resset;
    QTime tt;
    tt.start();
    FWObject *f=o;
    while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
    if (f) fws.push_back(Firewall::cast(f));
    m_project->db()->findWhereUsed(o,m_project->db(),resset);

    set<FWObject *>::iterator i=resset.begin();
    for ( ;i!=resset.end();++i)
    {
        RuleElement *re=RuleElement::cast(*i);
        if (re==NULL) continue;

        Rule *r=Rule::cast(re->getParent());
        if (r && !r->isDisabled())
        {
            f=r;
            while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
            if (f && std::find(fws.begin(),fws.end(),f)==fws.end())
                fws.push_back(Firewall::cast(f));
        }
    }

    if (fwbdebug)
        qDebug(QString("Program spent %1 ms searching for firewalls.")
               .arg(tt.elapsed()).toAscii().constData());

    return fws;
}

void ObjectManipulator::updateLastModifiedTimestampForOneFirewall(FWObject *o)
{
    if (fwbdebug) qDebug("ObjectManipulator::updateLastModifiedTimestampForOneFirewall");

    if (o==NULL) return;

    Firewall *f = Firewall::cast(o);
    if (f==NULL) return;

    f->updateLastModifiedTimestamp();
    getCurrentObjectTree()->updateTreeItems ();
    info();
}


void ObjectManipulator::updateLastModifiedTimestampForAllFirewalls(FWObject *o)
{
    if (fwbdebug) qDebug("ObjectManipulator::updateLastModifiedTimestampForAllFirewalls");

    if (o==NULL) return;

    QStatusBar *sb = mw->statusBar();
    sb->showMessage( tr("Searching for firewalls affected by the change...") );

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);

    if (fwbdebug) qDebug("ObjectManipulator::updateLastModifiedTimestampForAllFirewalls:   setOverrideCursor");
    QApplication::setOverrideCursor(QCursor( Qt::WaitCursor));

    list<Firewall *> fws = findFirewallsForObject(o);
    if (fws.size())
    {
        Firewall *f;
        for (list<Firewall *>::iterator i=fws.begin();
                i!=fws.end();
                ++i)
        {
            f=*i;
            f->updateLastModifiedTimestamp();
        }

        getCurrentObjectTree()->updateTreeItems ();
        info();
    }
    if (fwbdebug) qDebug("ObjectManipulator::updateLastModifiedTimestampForAllFirewalls:   restoreOverrideCursor");
    QApplication::restoreOverrideCursor();
    sb->clearMessage();

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,100);
}

void ObjectManipulator::updateLastInstalledTimestamp(FWObject *o)
{
    if (fwbdebug) qDebug("ObjectManipulator::updateLastInstalledTimestamp");
    if (o==NULL) return;
    Firewall * f=(Firewall *)o;
    if (f!=NULL)
    {
        bool visualUpdate=f->needsInstall();
        f->updateLastInstalledTimestamp();
        if (visualUpdate)
        {
            getCurrentObjectTree()->updateTreeItems ();
        }
        info();
    }
}
void ObjectManipulator::updateLastCompiledTimestamp(FWObject *o)
{
    if (fwbdebug) qDebug("ObjectManipulator::updateLastCompiledTimestamp");

    if (o==NULL) return;
    Firewall * f=(Firewall *)o;
    if (f!=NULL)
    {
        f->updateLastCompiledTimestamp();
        info();
    }
}

void ObjectManipulator::simulateInstall()
{
    if (fwbdebug) qDebug("ObjectManipulator::simulateInstall");

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    Firewall *fw;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        fw= Firewall::cast( *i );
        if (fw!=NULL)
        {
            fw->updateLastCompiledTimestamp();
            fw->updateLastInstalledTimestamp();
        }
    }
    getCurrentObjectTree()->updateTreeItems ();

}

void ObjectManipulator::findAllFirewalls (list<Firewall *> &fws)
{
    if (fwbdebug) qDebug("ObjectManipulator::findAllFirewalls");

    std::list<FWObject*> fwlist;
    findByObjectType(m_project->db(),Firewall::TYPENAME,fwlist);
    for (list<FWObject*>::iterator m=fwlist.begin(); m!=fwlist.end(); m++)
        fws.push_back( Firewall::cast(*m) );
}

FWObject* ObjectManipulator::getSelectedObject()
{
    return currentObj;
}

void ObjectManipulator::findWhereUsedSlot()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    m_project->findWhereUsed(obj);

}

void ObjectManipulator::reopenCurrentItemParent()
{
    QTreeWidgetItem *itm = current_tree_view->currentItem();
    if (itm)
        itm = itm->parent();
    if (!itm)
        return;
    itm->parent()->setExpanded(false);
    itm->parent()->setExpanded(true);

    current_tree_view->scrollToItem(itm);
}






