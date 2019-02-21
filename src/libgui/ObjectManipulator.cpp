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
#include "utils_no_qt.h"
#include "platforms.h"
#include "events.h"

#include "listOfLibrariesModel.h"
#include "ProjectPanel.h"
#include "ObjectManipulator.h"
#include "ObjectEditor.h"
#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"
#include "FWObjectClipboard.h"
#include "FWObjectPropertiesFactory.h"
#include "FWBSettings.h"
#include "findDialog.h"
#include "newGroupDialog.h"
#include "FindObjectWidget.h"
#include "AskLibForCopyDialog.h"
#include "FindWhereUsedWidget.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "FWBTree.h"
#include "FWWindow.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/TagService.h"

#include <QMessageBox>
#include <QTime>
#include <QtDebug>
#include <QHeaderView>
#include <QUndoStack>
#include <QMenu>
#include <QAction>

#include <memory>
#include <algorithm>


using namespace std;
using namespace libfwbuilder;




HistoryItem::~HistoryItem() {}

ObjectManipulator::~ObjectManipulator()
{
    delete m_objectManipulator;
    delete libs_model;
}

ObjectManipulator::ObjectManipulator(QWidget *parent):
    QWidget(parent), current_tree_view(nullptr)
{
    m_objectManipulator = new Ui::ObjectManipulator_q;
    m_objectManipulator->setupUi(this);
    setObjectName(tr("Object Manipulator"));

    libs_model = new ListOfLibrariesModel();
    m_objectManipulator->libs->setModel(libs_model);

    m_project = nullptr;

    treeWidth    = -1;
    treeHeight   = -1;
    active       = false;
    current_tree_view=nullptr;
    previous_lib_index = -1;

    // used in duplicateWithDependencies()
    dedup_marker_global_counter = time(nullptr);

    popup_menu = nullptr;

//    buildNewObjectMenu();
    lastClickedItem = nullptr;

}

void ObjectManipulator::setupProject(ProjectPanel *project)
{
    m_project = project;
}

vector<QTreeWidget*> ObjectManipulator::getTreeWidgets()
{
    vector<QTreeWidget*> res;
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QTreeWidget *objTreeView = libs_model->getTreeWidget(i);
        if (objTreeView == nullptr) continue;
        res.push_back(objTreeView);
    }
    return res;
}

void ObjectManipulator::showDeletedObjects(bool f)
{
    try
    {
        FWObject *dobj = m_project->db()->findInIndex(
            FWObjectDatabase::DELETED_OBJECTS_ID);

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects f=%d  dobj=%p",
                   f, dobj);

        if (dobj==nullptr)
        {
            dobj = m_project->db()->create(Library::TYPENAME);
            dobj->setId(FWObjectDatabase::DELETED_OBJECTS_ID);
            dobj->setName("Deleted Objects");
            dobj->setReadOnly(false);
            m_project->db()->add(dobj);
        }

        QModelIndex idx = libs_model->getIdxForLib(dobj);

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects idx.row()=%d", idx.row());

        if (f)
        {
            if (idx.isValid()) return;
            addLib( dobj );
            openLib( dobj );
        } else
        {
            if (!idx.isValid()) return;

            QTreeWidget *otv = libs_model->getTreeWidget(idx);

            if (fwbdebug)
                qDebug("ObjectManipulator::showDeletedObjects otv=%p", otv);

            assert(otv!=nullptr);
            m_objectManipulator->widgetStack->removeWidget( otv );
            removeLib(idx.row());
        }
    }
    catch(FWException &ex)
    {
/* we get exception if file is opened read-only and there is no "deleted
 * objects" library yet
 */
    }
}

QString ObjectManipulator::getStandardName(FWObject *parent,
                                           const string&,
                                           const string &namesuffix)
{
    QStringList names;
    FWObject *po = parent;
    while (po!=nullptr)
    {
        names.push_front(QString::fromUtf8(po->getName().c_str()));
        if (Host::cast(po)) break;
        po = po->getParent();
    }
//    names.push_back(QString::fromUtf8(parent->getName().c_str()));
    names.push_back(namesuffix.c_str());
    return names.join(":");
}

void ObjectManipulator::currentTreePageChanged(int i)
{
    QWidget *w = m_objectManipulator->widgetStack->widget(i);
    switchingTrees(w);
}

void ObjectManipulator::switchingTrees(QWidget* w)
{
    ObjectTreeView *new_otv = dynamic_cast<ObjectTreeView*>(w);

    if (!new_otv)
        return;//assert(new_otv)

    if (current_tree_view!=nullptr) current_tree_view->becomingHidden();
    new_otv->becomingVisible();
    current_tree_view = new_otv;
}

/*
 * Make the name of the object @obj unique across all children of the
 * given @target object. If this object is an interface, use pattern
 * <ifname><ifnumber> and increment the number until the name becomes
 * unique. For all other types use pattern <basename>-<number>
 *
 * This method has ugly side-effect: if @obj is an Interface, this
 * method needs to check its type. To do that, it calls
 * Interface::getOptionsObject() which creates options object if it
 * does not exits. To do initial options configuration, it needs
 * access to the parent. We call Interface::getOptionsObject() in copt
 * and startDrag methods to make sure interfaces have options objects
 * before copy or drag operation starts to avoid this problem here.
 *
 * In case of copy/paste or d&d of an interface, the naming
 * conventions are dictated by the platform of the new parent firewall
 * rather than the old one, which in this case is either <target> or
 * its parent. So we'll use @target to get proper interfaceProperties
 * object which will do checks for us.
 */
void ObjectManipulator::makeNameUnique(FWObject *target, FWObject *obj)
{
    Interface *intf = Interface::cast(obj);
    if (intf)
    {
        // check if this is vlan subinterface. We should not change
        // names of those
        if (intf->getOptionsObject()->getStr("type") == "8021q")
            return;
        // one of the typical usage patterns is to create vlan
        // interface "eth0.101" and then immediately try to copy/paste
        // it to under br0 to make it bridge port. In this case
        // interface eth0.101 won't have type "8021q" just yet because
        // the user did not open interface "advanced" settings dialog
        // to set its type and VLAN ID. Users assume that if its name
        // is "eth0.101", then it must be vlan interface. We should
        // follow this assumption too. Also, check for names "vlanNNN"
        // as well.
        //
        QString obj_name = QString::fromUtf8(obj->getName().c_str());
        FWObject *fw = target;
        while (fw && !Firewall::cast(fw)) fw = fw->getParent();

        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(fw));

        if (int_prop->looksLikeVlanInterface(obj_name)) return;
    }
    QString newname = makeNameUnique(target,
                                     QString::fromUtf8(obj->getName().c_str()),
                                     obj->getTypeName().c_str());
    obj->setName(string(newname.toUtf8()));
}

QString ObjectManipulator::makeNameUnique(FWObject* parent,
                                          const QString &obj_name,
                                          const QString &obj_type)
{
    int suffix = 1;
    QString basename = obj_name;
    QString newname = basename;

    if (fwbdebug)
        qDebug("ObjectManipulator::makeNameUnique parent=%s obj_name=%s",
               parent->getName().c_str(),
               obj_name.toStdString().c_str());

    if (obj_type == Interface::TYPENAME)
    {
        QRegExp rx("([a-zA-Z-]+)(\\d{1,})");
        if (rx.indexIn(obj_name) != -1)
        {
            basename = rx.cap(1);
            suffix = rx.cap(2).toInt();
        }
    }

/*
 * Check if there is another object with the same name. Note that
 * FWObject::findObjectByName() searches in depth, but we only need to
 * scan child objects of the first level.
 */
    while (true)
    {
        if (fwbdebug)
            qDebug("ObjectManipulator::makeNameUnique newname=%s basename=%s suffix=%d",
                   newname.toStdString().c_str(),
                   basename.toStdString().c_str(),
                   suffix);

        FWObject::const_iterator i = find_if(
            parent->begin(), parent->end(),
            FWObjectNameEQPredicate(newname.toStdString()));
        if (i==parent->end()) break;
        if (obj_type == Interface::TYPENAME)
            newname = QString("%1%2").arg(basename).arg(suffix);
        else
            newname = QString("%1-%2").arg(basename).arg(suffix);
        suffix++;
    }
    return newname;
}

static void addKeywordsMenu(ObjectManipulator *om, QMenu *menu)
{
    QMenu *keywordsMenu = menu->addMenu(om->tr("Keywords"));
    QMenu *addKeywords = keywordsMenu->addMenu(om->tr("Add"));

    addKeywords->addAction(om->tr("New Keyword..."),
                           om, SLOT(addNewKeywordSlot()));
    addKeywords->addSeparator();

    QStringList addList;
    const set<string> &allKeywords = om->getSelectedObject()->getAllKeywords();
    set<string>::const_iterator iterz;
    for (iterz = allKeywords.begin(); iterz != allKeywords.end(); ++iterz) {
        addList.append(QString::fromUtf8((*iterz).c_str()));
    }
    addList = sortStrings(addList);

    QStringList data;
    data << "add" << "";
    foreach (QString add, addList) {
        QAction *act =
            addKeywords->addAction(add, om, SLOT(processKeywordSlot()));
        data[1] = add;
        act->setData(data);
    }

    bool allLocked = true;
    QMenu *removeKeywords = keywordsMenu->addMenu(om->tr("Remove"));
    QSet<QString> toRemove;
    foreach (FWObject *obj, om->getCurrentObjectTree()->getSelectedObjects()) {
        if (obj->isReadOnly()) continue;
        allLocked = false;

        const set<string> &keywords = obj->getKeywords();
        set<string>::const_iterator iter;
        for (iter = keywords.begin(); iter != keywords.end(); ++iter) {
            toRemove.insert(QString::fromUtf8((*iter).c_str()));
        }
    }

    if (toRemove.isEmpty()) {
        removeKeywords->setDisabled(true);
    } else {
        data[0] = "remove";
        foreach (QString str, sortStrings(toRemove.toList())) {
            QAction *act =
                removeKeywords->addAction(str, om, SLOT(processKeywordSlot()));
            data[1] = str;
            act->setData(data);
        }
    }

    if (allLocked) {
        keywordsMenu->setDisabled(true);
    }
}


void ObjectManipulator::addSubfolderActions(QList<QAction*> &AddObjectActions, FWObject *currentObj, ObjectTreeViewItem *item, bool &addSubfolder)
{
    addSubfolder = item != nullptr;
    string path;
    if (currentObj == nullptr) {
        path = item->getUserFolderParent()->getPath(true);
    }
    else {
        path = currentObj->getPath(true);
    }

    //Do not allow to create subfolders on real objects
    if(item==nullptr && (currentObj!=nullptr
                      &&!Firewall::isA(currentObj)
                      &&!Cluster::isA(currentObj)
                      &&!IPv4::isA(currentObj)
                      &&!IPv6::isA(currentObj)
                      &&!DNSName::isA(currentObj)
                      &&!AddressTable::isA(currentObj)
                      &&!AddressRange::isA(currentObj)
                      &&!Host::isA(currentObj)
                      &&!Network::isA(currentObj)
                      &&!NetworkIPv6::isA(currentObj)
                      &&!DynamicGroup::isA(currentObj)
                      &&!CustomService::isA(currentObj)
                      &&!IPService::isA(currentObj)
                      &&!ICMPService::isA(currentObj)
                      &&!ICMP6Service::isA(currentObj)
                      &&!TCPService::isA(currentObj)
                      &&!UDPService::isA(currentObj)
                      &&!TagService::isA(currentObj)
                      &&!ServiceGroup::isA(currentObj)
                      &&!UserService::isA(currentObj)
                      &&!Interval::isA(currentObj)
                      )) {
        addSubfolder = true;
    }

    if (path.find("Firewalls") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, Firewall::TYPENAME));
    }

    if (path.find("Clusters") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, Cluster::TYPENAME));
    }

    if (path.find("Objects/Addresses") == 0)
    {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, IPv4::TYPENAME));
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, IPv6::TYPENAME));
    }

    if (path.find("Objects/DNS Names") == 0)
    {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, DNSName::TYPENAME));
    }

    if (path.find("Objects/Address Tables") == 0)
    {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, AddressTable::TYPENAME));
    }

    if (path.find("Objects/Address Ranges") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, AddressRange::TYPENAME));
    }

    if (path.find("Objects/Hosts") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, Host::TYPENAME));
    }

    if (path.find("Objects/Networks") == 0)
    {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, Network::TYPENAME));
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, NetworkIPv6::TYPENAME));
    }

    if (path.find("Objects/Groups") == 0) {
        //We don't want to add subfolders to groups of objects.
        //Unfortunately the main folders are objectgroups themselves.
        //This is a temporary workaround
        if(path!="Objects/Groups") {
            addSubfolder = false;
        }
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, ObjectGroup::TYPENAME));
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, DynamicGroup::TYPENAME));
    }

    if (path.find("Services/Custom") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, CustomService::TYPENAME));
    }

    if (path.find("Services/IP") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, IPService::TYPENAME));
    }

    if (path.find("Services/ICMP") == 0)
    {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, ICMPService::TYPENAME));
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, ICMP6Service::TYPENAME));
    }

    if (path.find("Services/TCP") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, TCPService::TYPENAME));
    }

    if (path.find("Services/UDP") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, UDPService::TYPENAME));
    }

    if (path.find("Services/TagServices") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, TagService::TYPENAME));
    }

    if (path.find("Services/Groups") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, ServiceGroup::TYPENAME));
    }

    if (path.find("Services/Users") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, UserService::TYPENAME));
    }

    if (path.find("Time") == 0) {
        AddObjectActions.append(
            addNewObjectMenuItem(popup_menu, Interval::TYPENAME));
    }
}

void ObjectManipulator::contextMenuRequested(const QPoint &pos)
{
    QList<QAction*>::iterator iter;
    QList<QAction*> AddObjectActions;
    bool addSubfolder = false;
    if (popup_menu == nullptr)
    {
        popup_menu = new QMenu(this);
        popup_menu->setObjectName("objectTreeContextMenu");
    } else
        popup_menu->clear();

/* in extended selection mode there may be several selected items */

    ObjectTreeView *objTreeView = getCurrentObjectTree();
    if (objTreeView == nullptr) return;

    QTreeWidgetItem *item = objTreeView->itemAt(pos);//clicked item

    if (fwbdebug)
        qDebug("ObjectManipulator::contextMenu  selectedObjects.size=%d",
               getCurrentObjectTree()->getNumSelected());

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    if (otvi==nullptr)  return;  // happens when user clicks outside an item

    lastClickedItem = otvi;

    FWObject *obj = otvi->getFWObject();
    if (obj == nullptr) {
        assert(otvi->getUserFolderParent() != nullptr);
        QAction *action =
            popup_menu->addAction(tr("Delete"), this, SLOT(removeUserFolder()));
        /* The user-defined folder doesn't get counted as a selected obj */
        if (objTreeView->getNumSelected() > 0) {
            action->setEnabled(false);
        }

        addSubfolderActions(AddObjectActions, nullptr, otvi, addSubfolder);

        for (iter=AddObjectActions.begin(); iter!=AddObjectActions.end(); iter++)
            (*iter)->setEnabled(true);

        popup_menu->exec(QCursor::pos());
        return;
    }

    if (!getCurrentObjectTree()->isSelected(otvi->getFWObject()))
        openObjectInTree( otvi, true );

    //if (currentObj==nullptr)  currentObj=otvi->getFWObject();
    FWObject *currentObj = getSelectedObject();

    if (item->childCount() > 0)
    {
        if (item->isExpanded())
            popup_menu->addAction(tr("Collapse"), this,
                                  SLOT(collapseCurrentTreeNode()));
        else
            popup_menu->addAction(tr("Expand"), this,
                                  SLOT(expandCurrentTreeNode()));
        popup_menu->addSeparator();
    }

    QAction *edtID;

    if (currentObj->isReadOnly())
        edtID = popup_menu->addAction(tr("Inspect"),
                                      this, SLOT( editSelectedObject()));
    else
        edtID = popup_menu->addAction(tr("Edit"),
                                      this, SLOT( editSelectedObject()));

    if (RuleSet::cast(currentObj))
        popup_menu->addAction(tr("Open"), this, SLOT( openSelectedRuleSet()));

    QMenu *duptargets  = nullptr;
    QAction *dupID = nullptr;
    QMenu *movetargets = nullptr;
    int moveTargetsCounter = 0;

    if (!Interface::isA(currentObj) &&
        !physAddress::isA(currentObj) && 
        RuleSet::cast(currentObj)==nullptr &&
        !Library::isA(currentObj) &&
        !FWBTree().isStandardFolder(currentObj))
    {
        duptargets = popup_menu->addMenu( tr("Duplicate ...") );
        movetargets = popup_menu->addMenu( tr("Move ...") );

        connect ( duptargets, SIGNAL ( triggered(QAction*) ),
                  this, SLOT( duplicateObj(QAction*) ) );
        connect ( movetargets, SIGNAL ( triggered(QAction*) ),
                  this, SLOT( moveObj(QAction*) ) );

/* we add " ... to library ..." submenu to the "Move " menu item only
 * if user did not select a library, or if they selected several
 * objects. Method moveObj knows that library should not be moved
 * into another library.
 */
        bool libSelected =
            (getCurrentObjectTree()->getNumSelected()==1 &&
             Library::isA(getCurrentObjectTree()->getSelectedObjects().front()));

        FWObject *cl = getCurrentLib();
        vector<FWObject*>::iterator i;

        int N = libs_model->rowCount();
        for (int row=0 ; row<N; ++row)
        {
            QModelIndex idx = libs_model->index(row, 0);
            FWObject *lib = libs_model->getLibrary(idx);
            if (lib == nullptr) continue;
            if ( lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
                 lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
                 lib->isReadOnly())
                continue;

            dupID = duptargets->addAction(
                tr("place in library %1").arg(
                    QString::fromUtf8(lib->getName().c_str())));
            dupID->setData(row);

            // can't move to the same library
            if (lib == cl) continue; // skip current library

            if (!libSelected)
            {
                moveTargetsCounter++;
                QAction* mact = movetargets->addAction(
                    tr("to library %1").arg(
                        QString::fromUtf8(lib->getName().c_str())));
                mact->setData(row);
            }
        }
    }
    
    if (Library::isA(currentObj) &&
        currentObj->getParent()->getId()==FWObjectDatabase::DELETED_OBJECTS_ID)
    {
        popup_menu->addAction( tr("Undelete"), this, SLOT(undeleteLibrary()));
    }


    popup_menu->addSeparator();

    QAction *copyID = popup_menu->addAction(tr("Copy"), this, SLOT(copyObj()));
    QAction *cutID = popup_menu->addAction(tr("Cut"), this, SLOT(cutObj()));
    QAction *pasteID = popup_menu->addAction(tr("Paste"), this, SLOT(pasteObj()));

    popup_menu->addSeparator();

    QAction * delID = popup_menu->addAction( tr("Delete"), this,
                                        SLOT( delObj() ) );

    popup_menu->addSeparator();


    
    if (getCurrentObjectTree()->getNumSelected()==1)
    {

        if ( (Firewall::isA(currentObj) || Host::isA(currentObj)) &&
             ! currentObj->isReadOnly() )
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Interface::TYPENAME));
        }

        if ((Firewall::isA(currentObj) || Cluster::isA(currentObj)) &&
             ! currentObj->isReadOnly())
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Policy::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, NAT::TYPENAME));
        }

        if (Interface::isA(currentObj) && ! currentObj->isReadOnly())
        {
            Interface *iface = Interface::cast(currentObj);
            FWObject *h = Host::getParentHost(iface);
            //FWObject *h = iface->getParentHost();

            bool supports_advanced_ifaces = false;
            try {
                /*
                 * ignore raised exception; this just means that the host_OS
                 * option is undefined for this target (e.g. for a host).
                 */
                supports_advanced_ifaces = Resources::getTargetCapabilityBool
                        (h->getStr("host_OS"), "supports_subinterfaces");
            } catch (FWException &ex) { }

            /*
             * check if this interface can have subinterfaces. Show "Add Interface"
             * menu item only if host_os has attribute "supports_subinterfaces"
             * and if parent interface (currentObj) has the type that can have
             * subinterfaces. Also, cluster interfaces can't have subinterfaces
             * and only one level of subinterfaces is allowed.
             */
            if (supports_advanced_ifaces && Firewall::isA(currentObj->getParent()))
            {
                list<QStringPair> subint_types;
                getSubInterfaceTypes(iface, subint_types);
                if (subint_types.size())
                    addNewObjectMenuItem(popup_menu, Interface::TYPENAME);
                    // popup_menu->addAction(
                    //     tr("Add Interface"), this, SLOT( newInterface() ) );
            }

            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, IPv4::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, IPv6::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, physAddress::TYPENAME));

            /*
             * Add menu item to let user add AttachedNetworks object
             * to an interface, but only if this object does not exist yet.
             *
             * Actions added to AddObjectActions are
             * enabled and disabled all together based on the decision
             * made in getMenuState() (argument newMenuItem). But we
             * should always allow the user to add AttachedNetworks
             * object to an interface.
             */
            FWObject *att = currentObj->getFirstByType(AttachedNetworks::TYPENAME);
            if (att == nullptr)
                addNewObjectMenuItem(popup_menu, AttachedNetworks::TYPENAME);


            // Check if we should add menu item that creates failover
            // group. if parent is a cluster, allow one vrrp type
            // FailoverClusterGroup per Interface only
            FWObject *parent = nullptr;
            parent = currentObj->getParent();
            if (parent != nullptr && Cluster::isA(parent))
            {
                QAction *failover_menu_id = addNewObjectMenuItem(
                    popup_menu, FailoverClusterGroup::TYPENAME);
                // QAction *failover_menu_id = popup_menu->addAction(
                //     tr("Add Failover Group"), this,
                //     SLOT( newFailoverClusterGroup() ) );
                failover_menu_id->setEnabled(
                    currentObj->getFirstByType(
                        FailoverClusterGroup::TYPENAME) == nullptr);
            }
        }

        if (Cluster::isA(currentObj) && ! currentObj->isReadOnly())
        {
            AddObjectActions.append(
                addNewObjectMenuItem(
                    popup_menu, Interface::TYPENAME, "Add cluster interface"));

            // allow multiple state syncing groups per cluster
            // Rationale: these groups may represent different state syncing
            // protocols that can synchronize different things.
            AddObjectActions.append(
                addNewObjectMenuItem(
                    popup_menu, StateSyncClusterGroup::TYPENAME));
        }


        addSubfolderActions(AddObjectActions, currentObj, nullptr, addSubfolder);

        if (addSubfolder) {
            QAction *action =
                popup_menu->addAction(QIcon(":/Icons/SystemGroup/icon-tree"),
                                      tr("New Subfolder"), this,
                                      SLOT(addSubfolderSlot()));
            action->setData(currentObj->getId());
            AddObjectActions.append(action);
        }

        popup_menu->addSeparator();

        QAction *findID =
            popup_menu->addAction( tr("Find"), this, SLOT( findObject()));

        QAction *whereUsedID =
            popup_menu->addAction( tr("Where used"),
                                   this, SLOT( findWhereUsedSlot()));

        findID->setEnabled( !FWBTree().isStandardFolder(currentObj));
        whereUsedID->setEnabled( !FWBTree().isStandardFolder(currentObj));
    }

    popup_menu->addSeparator();
    popup_menu->addAction( tr("Group"), this, SLOT( groupObjects() ) )
            ->setDisabled(getCurrentObjectTree()->getNumSelected()==1);

    addKeywordsMenu(this, popup_menu);

    if (Firewall::cast(currentObj)!=nullptr ||
        (ObjectGroup::cast(currentObj)!=nullptr &&
         currentObj->getName()=="Firewalls"))
    {
        bool canCreateCluster = true;
        if (getCurrentObjectTree()->getNumSelected() > 1)
        {
            foreach( FWObject *obj, getCurrentObjectTree()->getSelectedObjects())
            {
                if (!Firewall::isA(obj))
                {
                    canCreateCluster = false;
                    break;
                }
            }
        } else canCreateCluster = false;
        popup_menu->addAction( tr("New cluster from selected firewalls"),
                               this, SLOT( newClusterFromSelected() ) )->setEnabled(canCreateCluster);
        popup_menu->addSeparator();
        popup_menu->addAction( tr("Compile"), this, SLOT( compile()));
        popup_menu->addAction( tr("Install"), this, SLOT( install()));
        popup_menu->addAction( tr("Inspect"), this, SLOT( inspect()));
    }

    if (Interface::cast(currentObj)!=nullptr)
    {
        popup_menu->addSeparator();
        FWObject *h = Host::getParentHost(currentObj);
        if (h != nullptr) {
            list<FWObject*> top_level_interfaces = h->getByType(Interface::TYPENAME);
            top_level_interfaces.sort(FWObjectNameCmpPredicate());
            addSubinterfaceSubmenu(popup_menu, top_level_interfaces);
        }
    }

    popup_menu->addSeparator();
    QAction* lckID = popup_menu->addAction(tr("Lock"), this, SLOT(lockObject()));
    QAction* unlckID = popup_menu->addAction(tr("Unlock"), this, SLOT(unlockObject()));

    lckID->setEnabled(isCurrentObjectLockable());
    unlckID->setEnabled(isCurrentObjectUnlockable());

    if (fwbdebug)
    {
/* keep this for debugging  */
        popup_menu->addSeparator();
        popup_menu->addAction( tr("dump"), this, SLOT( dumpObj()));
    }

    if (getCurrentObjectTree()->getNumSelected()==1)
    {
        edtID->setEnabled( !FWBTree().isStandardFolder(currentObj));
    } else
        edtID->setEnabled(false);

    bool dupMenuItem = true;
    bool moveMenuItem = true;
    bool copyMenuItem = true;
    bool pasteMenuItem = true;
    bool delMenuItem = true;
    bool newMenuItem = true;
    bool inDeletedObjects = false;

    getMenuState( (moveTargetsCounter>0),
                  dupMenuItem, moveMenuItem, copyMenuItem, pasteMenuItem,
                  delMenuItem, newMenuItem, inDeletedObjects);

    if (dupID) dupID->setEnabled(dupMenuItem);

    copyID->setEnabled(copyMenuItem);
    pasteID->setEnabled(pasteMenuItem);
    
    cutID->setEnabled(delMenuItem);
    delID->setEnabled(delMenuItem);

    // can not move object if can not delete it
    if (movetargets)
        movetargets->setEnabled(delMenuItem);



//    if (inDeletedObjects) movID->setText( tr("Undelete...") );
    for (iter=AddObjectActions.begin(); iter!=AddObjectActions.end(); iter++)
        (*iter)->setEnabled(newMenuItem);
    popup_menu->exec(QCursor::pos());
}

/*
 * Add menu item "Make subinterface of ..." and submenu with list of
 * top level interfaces.
 */
void ObjectManipulator::addSubinterfaceSubmenu(
    QMenu *menu,
    const list<FWObject*> &top_level_interfaces)
{
    QMenu *submenu = menu->addMenu( tr("Make subinterface of..."));
    
    int submenu_items_counter = 0;
    list<FWObject*>::const_iterator it;
    for (it=top_level_interfaces.begin(); it!=top_level_interfaces.end(); ++it)
    {
        Interface *intf = Interface::cast(*it);

        bool skip_interface = false;
        foreach(FWObject *obj, getCurrentObjectTree()->getSelectedObjects())
        {
            if (obj == intf)
            {
                skip_interface = true;
                break;
            }

            if (!intf->validateChild(obj))
            {
                skip_interface = true;
                break;
            }
        }
        if (skip_interface) continue;
        if (intf->isLoopback()) continue;

        // can not add interfaces to a read-only parent interface
        if (intf->isReadOnly()) continue; 

        QString itf_name = QString::fromUtf8(intf->getName().c_str());
        FWObject *parent_fw = Host::getParentHost(intf);

        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                parent_fw));
        if (int_prop->looksLikeVlanInterface(itf_name)) continue;

        QAction *a = submenu->addAction(
            QIcon(":/Icons/Interface/icon-tree"), itf_name);

        a->setData(intf->getId());

        connect( submenu, SIGNAL(triggered(QAction*)),
                 this, SLOT(makeSubinterface(QAction*)));

        submenu_items_counter++;
    }

    submenu->setEnabled(submenu_items_counter != 0);
}

bool ObjectManipulator::getDeleteMenuState(FWObject *obj)
{
    if (obj->isReadOnly()) return false;

    QString objPath = obj->getPath(true).c_str();
    bool del_menu_item_state = FWBTree().getDeleteMenuState(objPath);

    // can't delete last policy, nat and routing child objects
    // also can't delete "top" policy ruleset
    if (del_menu_item_state && RuleSet::cast(obj))
    {
        Firewall *fw = Firewall::cast(obj->getParent());
        // fw can be nullptr if this ruleset is in the Deleted objects
        // library
        if (fw==nullptr) return del_menu_item_state;
        list<FWObject*> child_objects = fw->getByType(obj->getTypeName());
        if (child_objects.size()==1) del_menu_item_state = false;
    }
    return del_menu_item_state;
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
    inDeletedObjects = false;

    if (m_project->db() == nullptr)
    {
        dupMenuItem = false;
        moveMenuItem = false;
        copyMenuItem = false;
        pasteMenuItem = false;
        delMenuItem = false;
        newMenuItem = false;
        return;
    }

    dupMenuItem = true;
    moveMenuItem = true;
    copyMenuItem = true;
    pasteMenuItem = true;
    delMenuItem = true;
    newMenuItem = true;

    FWObject *del_obj_library =
        m_project->db()->findInIndex( FWObjectDatabase::DELETED_OBJECTS_ID);

    FWObject *current_library = getCurrentLib();

    if (getCurrentObjectTree()==nullptr) return;

    // delete, cut and copy menu items will be enabled only if all
    // selected objects have the same parent (so user can not select
    // an interface and one but not all of its addresses for deletion,
    // see #1676)
    FWObject *parent = nullptr;
    vector<FWObject*> so = getCurrentObjectTree()->getSelectedObjects();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        FWObject *obj= *i;

        QString object_path = obj->getPath(true).c_str();

        if (parent == nullptr) parent = obj->getParent();
        else
        {
            if (parent != obj->getParent())
            {
                delMenuItem = false;
                copyMenuItem = false;
            }
        }

        if (AttachedNetworks::isA(obj))
        {
            dupMenuItem = false;
            moveMenuItem = false;
            copyMenuItem = false;
            pasteMenuItem = false;
            newMenuItem = false;
            continue;
        }

        copyMenuItem = copyMenuItem && FWBTree().getCopyMenuState(object_path);
        pasteMenuItem = pasteMenuItem &&
            FWBTree().getPasteMenuState(object_path) &&
            FWObjectClipboard::obj_clipboard &&
            (FWObjectClipboard::obj_clipboard->size()!=0);
        delMenuItem = delMenuItem && getDeleteMenuState(obj);
        delMenuItem = delMenuItem && current_library != nullptr &&
            current_library->getId() != FWObjectDatabase::STANDARD_LIB_ID;

#if DISABLE_PASTE_MENU_ITEM_IF_PASTE_IS_ILLEGAL
        if (pasteMenuItem)
        {
            /*
             * We used to enable Paste menu item only if object can be
             * pasted. The problem with this is that there was no
             * indication why Paste operation was not allowed. Since
             * we call validateForPaste during actual Paste operation
             * anyway, it is more user friendly to let them try and
             * actually see the error if it fails.
             */
            vector<std::pair<int,ProjectPanel*> >::iterator i;
            for (i= FWObjectClipboard::obj_clipboard->begin();
                 i!=FWObjectClipboard::obj_clipboard->end(); ++i)
            {
                FWObject *co= m_project->db()->findInIndex(i->first);
                if (co==nullptr)
                {
                    continue ;
                    //QString s2 = obj->getTypeName().c_str();
                }
                QString s3 = obj->getTypeName().c_str();
                QString err;
                bool validated = FWBTree().validateForInsertion(obj, co, err);
                pasteMenuItem = pasteMenuItem && validated;
            }
        }
#endif

        dupMenuItem=
            (dupMenuItem && ! FWBTree().isStandardFolder(obj) && ! Library::isA(obj) );

        inDeletedObjects = (del_obj_library!=nullptr && obj->isChildOf(del_obj_library));
        dupMenuItem = dupMenuItem && !inDeletedObjects;

// can't move system objects or libraries
        moveMenuItem = moveMenuItem && ! FWBTree().isStandardFolder(obj) && ! Library::isA(obj);

// can't move interfaces unless parent host object is also selected
        if ( Interface::isA(obj) &&
             std::find(so.begin(),so.end(),obj->getParent())==so.end())
            moveMenuItem = false;

// can't move ip addresses if parent is interface
        if (IPv4::isA(obj) && IPv6::isA(obj) && Interface::isA(obj->getParent()))
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

bool ObjectManipulator::isCurrentObjectLockable()
{
    FWObject *currentObj = getSelectedObject();
    return (currentObj && !currentObj->getParent()->isReadOnly() && !currentObj->getRO());
}

bool ObjectManipulator::isCurrentObjectUnlockable()
{
    FWObject *currentObj = getSelectedObject();
    return (currentObj && !currentObj->getParent()->isReadOnly() && currentObj->getRO());
}

void ObjectManipulator::filterFirewallsFromSelection(vector<FWObject*> &so,
                                                     set<Firewall*> &fo)
{
    Firewall *fw;
    ObjectGroup *gr;
    Cluster *cl;
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        cl = Cluster::cast(*i);
        if (cl != nullptr)
        {
            list<Firewall*> members;
            cl->getMembersList(members);
            // display warning if no firewalls could be extracted for a cluster
            if (members.size() == 0)
            {
                QMessageBox::warning(this, "Firewall Builder",
                        QObject::tr("No firewalls assigned to cluster '%1'").
                                     arg(cl->getName().c_str()),
                        "&Continue", QString::null, QString::null, 0, 1 );
                continue;
            }
            fo.insert(cl);
            continue;
        }
        fw = Firewall::cast(*i);
        if (fw!=nullptr)
        {
            fo.insert(fw);
            continue;
        }
        gr = ObjectGroup::cast(*i);
        if (gr!=nullptr)
        {
            extractFirewallsFromGroup(gr,fo);
        }
    }
}

void ObjectManipulator::extractFirewallsFromGroup(ObjectGroup *gr,
                                                  set<Firewall*> &fo)
{
   set<FWObject*> oset;
   m_project->db()->findObjectsInGroup(gr, oset);

   set<FWObject*>::iterator i;
   for(i=oset.begin();i!=oset.end();++i)
       if (Firewall::cast(*i)) fo.insert(Firewall::cast(*i));
}

FWObject* ObjectManipulator::prepareForInsertion(FWObject *target, FWObject *obj)
{
    if (fwbdebug)
        qDebug("prepareForInsertion   %s --> %s", obj->getName().c_str(),
               target->getName().c_str());

    FWObject *ta = target;
    if (IPv4::isA(ta) || IPv6::isA(ta)) ta = ta->getParent();

    if (Library::isA(target))
        ta = FWBTree().getStandardSlotForObject(target,
                                                 obj->getTypeName().c_str());

    QString err;
    if (! FWBTree().validateForInsertion(ta, obj, err))
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            err,
            "&Continue", QString::null, QString::null,
            0, 1 );

        return nullptr;
    }
    return ta;
}

void ObjectManipulator::editSelectedObject()
{
    if (fwbdebug) qDebug("ObjectManipulator::editSelectedObject");

    ObjectTreeView *objTreeView = getCurrentObjectTree();
    if (objTreeView == nullptr) return;
    if (objTreeView->getNumSelected()==0) return;

    FWObject *obj = getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==nullptr) return;

    // do not edit system folders (#1729)
    if (FWBTree().isSystem(obj)) return;

    if (RuleSet::cast(obj)!=nullptr)
    {
        // Open rule set object in the editor if it is already opened
        // in RuleSetView. If we just opened it in RuleSetView, check
        // if the editor is visible and if yes, open the object in the
        // editor right away. See #1331

        if (m_project->getCurrentRuleSet() == obj)
        {
            QCoreApplication::postEvent(
                mw, new openObjectInEditorEvent(
                    m_project->getFileName(), obj->getId()));
        } else
        {
            QCoreApplication::postEvent(
                m_project, new openRulesetEvent(
                    m_project->getFileName(), obj->getId()));
            if (mw->isEditorVisible())
                QCoreApplication::postEvent(
                    mw, new openObjectInEditorEvent(
                        m_project->getFileName(), obj->getId()));
        }

    } else
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(
                m_project->getFileName(), obj->getId()));
}

void ObjectManipulator::openSelectedRuleSet()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj = getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==nullptr) return;

    if (RuleSet::cast(obj)!=nullptr && m_project->getCurrentRuleSet()!=obj)
        QCoreApplication::postEvent(
            m_project, new openRulesetEvent(m_project->getFileName(), obj->getId()));
}

bool ObjectManipulator::editObject(FWObject *obj)
{
    if (fwbdebug) qDebug("ObjectManipulator::editObject");
    if (!mw->isEditorVisible()) mw->showEditor();
    return switchObjectInEditor(obj);
}

bool ObjectManipulator::switchObjectInEditor(FWObject *obj)
{
    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor");

    if (obj && fwbdebug)
    {
        qDebug("obj: %s", obj->getName().c_str());
        FWObject *edt_obj = mw->getOpenedEditor();
        if (edt_obj)
            qDebug("in editor: %s", edt_obj->getName().c_str());
    }

    if (RuleSet::cast(obj)!=nullptr)
    {
        if (obj!=m_project->getCurrentRuleSet())
        {
            m_project->openRuleSet(obj);
        }

    }
    if (!mw->isEditorVisible()) return false;

    if (!mw->requestEditorOwnership(this, obj, ObjectEditor::optNone, true))
    {
        if (fwbdebug) qDebug("Can not get editor panel ownership");
        return false;
    }

    if (fwbdebug) qDebug("Open object in editor");
    mw->openEditor(obj); // opens object in the editor
    //currentObj = obj;
    active = true;

    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor done");

    return true;      // successfully (re)opened obj in the editor
}

void ObjectManipulator::closeObject()
{
    //currentObj = nullptr;
    if (mw->isEditorVisible()) mw->hideEditor();
}

void ObjectManipulator::selectionChanged(QTreeWidgetItem *cur)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::selectionChanged");

    QTreeWidget *qlv = getCurrentObjectTree();
    if (qlv==nullptr) return;

    ObjectTreeViewItem* otvi = dynamic_cast<ObjectTreeViewItem*>(cur);

    if (otvi==nullptr) return;

    FWObject *obj = otvi->getFWObject();
    if (obj==nullptr) return;

    if (fwbdebug) qDebug("ObjectManipulator::selectionChanged obj=%s",
                         obj->getName().c_str());

    FWObject *o = obj;
    //if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    if (history.empty() || otvi != getCurrentHistoryItem() )
    {
        mw->enableBackAction();
        addObjectToHistory(otvi, o);
    }

    //currentObj = obj;

    active = true;

    update();

    if (fwbdebug) qDebug("ObjectManipulator::selectionChanged done");
}

void ObjectManipulator::openObjectInTree(ObjectTreeViewItem *otvi,
                                   bool register_in_history)
{
    openObjectInTree(otvi->getFWObject(), register_in_history);
}

/*
 * I could use default value for the parameter register_in_history,
 * but that caused problems when this method was used as a slot
 */
void ObjectManipulator::openObjectInTree(QTreeWidgetItem *item)
{
    ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(item);
    openObjectInTree(otvi, true);
}

void ObjectManipulator::openObjectInTree(FWObject *obj)
{
    openObjectInTree(obj,true);
}

/* This method is called from the GroupObjectDialog when user double
 * clicks on the object in a group, so first we should check if this
 * object is shown in the tree and if not, find and open it.
 */
void ObjectManipulator::openObjectInTree(FWObject *obj, bool /*register_in_history*/)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::openObjectInTree"
                 << "obj:" << ((obj)?obj->getName().c_str():"NULL")
                 << "id:" << ((obj)?obj->getId():0);

    if (obj==nullptr) return;

    openLibForObject(obj);

    //raise();
    FWObject *o=obj;
    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    ObjectTreeViewItem *otvi = allItems[o];
    // this changes selection and thus calls slot slectionChanged
    showObjectInTree(otvi);

    if (fwbdebug)
        qDebug() << "ObjectManipulator::openObjectInTree"
                 << "libs->currentIndex="
                 << m_objectManipulator->libs->currentIndex();

    updateCreateObjectMenu(obj->getLibrary());

    if (fwbdebug) qDebug() << "ObjectManipulator::openObjectInTree: done";
}

void ObjectManipulator::openLibForObject(FWObject *obj)
{
    // if obj is Library, its getLibrary() method returns itself. If
    // this library has been deleted and is now in the Deleted Objects
    // library, getIdxForLib() is not going to find it.
    if (FWObjectDatabase::isA(obj->getParent()))
        m_objectManipulator->libs->setCurrentIndex(
            libs_model->getIdxForLib(obj->getLibrary()).row());
    else
    {
        FWObject *parent = obj->getParent();
        FWObject *lib = parent->getLibrary();
        // see #2648 if a library was deleted with all of its
        // contents, then it is possible that variable lib may point
        // to such deleted library, which is located inside of the
        // "Deleted Objects" library.
        if (lib->getParent()->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
            lib = m_project->db()->findInIndex(
                FWObjectDatabase::DELETED_OBJECTS_ID);

        m_objectManipulator->libs->setCurrentIndex(
            libs_model->getIdxForLib(lib).row());
    }
}

void ObjectManipulator::showObjectInTree(ObjectTreeViewItem *otvi)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::showObjectInTree");
    if (otvi==nullptr) return;

    ObjectTreeView* otv = otvi->getTree();

    if (fwbdebug)
        qDebug() << "ObjectManipulator::showObjectInTree"
                 << "current_tree_view=" << current_tree_view
                 << "new_otv=" << otv;

//    otv->raise();
    m_objectManipulator->widgetStack->setCurrentWidget(otv);

    otvi->getTree()->clearSelection();
    otvi->getTree()->scrollToItem(otvi);
    otvi->getTree()->setCurrentItem(otvi);
    otvi->setSelected(true);
//    otvi->getTree()->setFocus(Qt::OtherFocusReason);
}

void ObjectManipulator::libChangedById(int obj_id)
{
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QModelIndex idx = libs_model->index(i, 0);
        FWObject *l = libs_model->getLibrary(idx);
        if (l == nullptr) continue;
        if (l->getId() == obj_id)
        {
            libChanged(i);
            m_objectManipulator->libs->setCurrentIndex(i);
            return;
        }
    }
}

FWObject* ObjectManipulator::getNextUserLib(FWObject *after_this)
{
    QString sid2 = "syslib000";
    QString sid3 = "syslib001";

    FWObject *lib = nullptr;
    if (after_this != nullptr) lib = after_this->getLibrary();

    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QModelIndex idx = libs_model->index(i, 0);
        FWObject *l = libs_model->getLibrary(idx);
        if (l == nullptr) continue;
        if (l == lib) continue;
        QString sid1 = FWObjectDatabase::getStringId(l->getId()).c_str();
        if ( sid1 == sid2 || sid1 == sid3) continue;

        return l;
    }
    return nullptr;
}

void ObjectManipulator::libChanged(int list_row)
{
    if (fwbdebug) qDebug() << "ObjectManipulator::libChanged list_row=" << list_row;

    previous_lib_index = list_row;

    QTreeWidget *objTreeView = libs_model->getTreeWidget(list_row);
    if (objTreeView == nullptr)
    {
        if (fwbdebug)
        {
            qDebug() << "There is no object tree widget associated with this row";
            qDebug() << "Scanning all rows:";

            for (int i=0; i<libs_model->rowCount(); ++i)
            {
                qDebug() << "Row" << i;

                QTreeWidget *objTreeView = libs_model->getTreeWidget(i);
                FWObject *lib = libs_model->getLibrary(i);

                qDebug() << "lib=" << lib << "objTreeView=" << objTreeView;
            }
        }
        return;
    }

    ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(objTreeView->currentItem());
    if (otvi == nullptr)
    {
        if (objTreeView->invisibleRootItem()->childCount() > 0)
            otvi = dynamic_cast<ObjectTreeViewItem*>(
                objTreeView->invisibleRootItem()->child(0));
        else
            assert(false);
    }

    showObjectInTree( otvi );
    QCoreApplication::postEvent(mw, new updateGUIStateEvent());
    return;
}

void ObjectManipulator::updateCreateObjectMenu(FWObject* lib)
{
    bool f = (
        lib == nullptr ||
        lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
        lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
        lib->isReadOnly()
    );
    bool new_object_op_possible = !f;
    emit libraryAccessChanged(new_object_op_possible);
//    m_objectManipulator->newButton->setEnabled(new_object_op_possible);
    QAction *noa = (QAction*)(mw->findChild<QAction*>("newObjectAction"));
    noa->setEnabled(new_object_op_possible);
}

FWObject* ObjectManipulator::getCurrentLib()
{
    int idx = m_objectManipulator->libs->currentIndex();
    if (idx == -1 ) return nullptr;

    FWObject *lib = libs_model->getLibrary(idx);

    return lib;
}

ObjectTreeView* ObjectManipulator::getCurrentObjectTree()
{
    return libs_model->getTreeWidget(m_objectManipulator->libs->currentIndex());

//    return current_tree_view;
}

void ObjectManipulator::openLib(FWObject *obj)
{
    openObjectInTree(obj->getLibrary(), false);
}

void ObjectManipulator::newObject()
{
//     m_objectManipulator->newButton->showMenu();
    buildNewObjectMenu();
}

void ObjectManipulator::select()
{
    FWObject *currentObj = getSelectedObject();

    if (fwbdebug)
        qDebug() << "ObjectManipulator::select()"
                 << "currentObj=" << currentObj
                 << ((currentObj)?currentObj->getName().c_str():" [unknown] ");

    if (currentObj==nullptr) return;

    m_objectManipulator->libs->setCurrentIndex(
        libs_model->getIdxForLib(currentObj->getLibrary()).row());

    // TODO: I forget why do we need flag "active", check this.
    ObjectTreeViewItem *otvi = allItems[currentObj];
    if (otvi)
    {
        active = true;
    }

    if (fwbdebug) qDebug("ObjectManipulator::select() done");
}

void ObjectManipulator::unselect()
{
    FWObject *currentObj = getSelectedObject();
    if (currentObj==nullptr) return;

    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QTreeWidget *otv = libs_model->getTreeWidget(i);
        if (otv == nullptr) continue;

        otv->clearSelection();
    }
    active=false;
}

list<Cluster*> ObjectManipulator::findClustersUsingFirewall(FWObject *fw)
{
    list<Cluster*> res;
    list<Cluster*> all_clusters;
    findAllClusters(all_clusters);
    list<Cluster*>::iterator it;
    for (it=all_clusters.begin(); it!=all_clusters.end(); ++it)
    {
        Cluster *cl = *it;
        list<Firewall*> members;
        cl->getMembersList(members);
        if (std::find(members.begin(), members.end(), Firewall::cast(fw)) != members.end())
            res.push_back(cl);
    }
    return res;
}

void ObjectManipulator::findAllFirewalls(list<Firewall*> &fws)
{
    if (fwbdebug) qDebug("ObjectManipulator::findAllFirewalls");

    list<FWObject*> fwlist;
    findByObjectType(m_project->db(), Firewall::TYPENAME, fwlist);
    for (list<FWObject*>::iterator m=fwlist.begin(); m!=fwlist.end(); m++)
        fws.push_back(Firewall::cast(*m));
}

void ObjectManipulator::findAllClusters(list<Cluster*> &clusters)
{
    list<FWObject*> cllist;
    findByObjectType(m_project->db(), Cluster::TYPENAME, cllist);
    for (list<FWObject*>::iterator m=cllist.begin(); m!=cllist.end(); m++)
        clusters.push_back(Cluster::cast(*m));
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
        if (fw!=nullptr)
        {
            fw->updateLastCompiledTimestamp();
            fw->updateLastInstalledTimestamp();
        }
    }
}

FWObject* ObjectManipulator::getSelectedObject()
{
    QTreeWidgetItem *cur = getCurrentObjectTree()->currentItem();
    if (cur)
    {
        ObjectTreeViewItem* otvi = dynamic_cast<ObjectTreeViewItem*>(cur);
        return otvi->getFWObject();
    }
    return nullptr;
}

void ObjectManipulator::reopenCurrentItemParent()
{
    QTreeWidgetItem *itm = getCurrentObjectTree()->currentItem();
    if (itm)  itm = itm->parent();
    if (!itm) return;
    itm->parent()->setExpanded(false);
    itm->parent()->setExpanded(true);
    getCurrentObjectTree()->scrollToItem(itm, QAbstractItemView::EnsureVisible);
    getCurrentObjectTree()->update();
}

void ObjectManipulator::loadSectionSizes()
{
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QTreeWidget *objTreeView = libs_model->getTreeWidget(i);
        FWObject *lib = libs_model->getLibrary(i);
        if (lib == nullptr || objTreeView == nullptr) continue;

        objTreeView->header()->resizeSection(
            0,
            st->getTreeSectionSize(
                m_project->getFileName(), lib->getName().c_str(), 0));
        objTreeView->header()->resizeSection(
            1,
            st->getTreeSectionSize(
                m_project->getFileName(), lib->getName().c_str(), 1));
    }
}

void ObjectManipulator::saveSectionSizes()
{
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        QTreeWidget *objTreeView = libs_model->getTreeWidget(i);
        FWObject *lib = libs_model->getLibrary(i);
        if (lib == nullptr || objTreeView == nullptr) continue;

        st->setTreeSectionSize(
            m_project->getFileName(), lib->getName().c_str(), 0,
            objTreeView->header()->sectionSize(0));
        st->setTreeSectionSize(
            m_project->getFileName(), lib->getName().c_str(), 1,
            objTreeView->header()->sectionSize(1));
    }
}

void ObjectManipulator::loadExpandedTreeItems()
{
    if (fwbdebug) qDebug() << "ObjectManipulator::loadExpandedTreeItems()";

    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        if (fwbdebug) qDebug() << "i=" << i;

        ObjectTreeView *objTreeView = libs_model->getTreeWidget(i);
        FWObject *lib = libs_model->getLibrary(i);
        if (lib == nullptr || objTreeView == nullptr) continue;

        set<int> expanded_objects;
        st->getExpandedObjectIds(m_project->getFileName(),
                                 lib->getName().c_str(),
                                 expanded_objects);
        objTreeView->ExpandTreeItems(expanded_objects);
        // there is no need to resize columns because call to
        //loadExpandedTreeItems is usually followed by the call to loadSectionSizes
        //objTreeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void ObjectManipulator::saveExpandedTreeItems()
{
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        ObjectTreeView *objTreeView = libs_model->getTreeWidget(i);
        FWObject *lib = libs_model->getLibrary(i);
        if (lib == nullptr || objTreeView == nullptr) continue;

        st->setExpandedObjectIds(m_project->getFileName(),
                                 lib->getName().c_str(),
                                 objTreeView->getListOfExpandedObjectIds());
    }
}

void ObjectManipulator::setAttributesColumnEnabled(bool)
{
    for (int i=0; i<libs_model->rowCount(); ++i)
    {
        ObjectTreeView *objTreeView = libs_model->getTreeWidget(i);
        if (objTreeView == nullptr) continue;
        objTreeView->showOrHideAttributesColumn();
    }
}

void ObjectManipulator::findWhereUsedRecursively(FWObject *obj,
                                                 FWObject *top,
                                                 set<FWObject*> &resset)
{
    UsageResolver().findWhereUsedRecursively(obj, top, resset, this->m_project->db());
}

list<Firewall*> ObjectManipulator::findFirewallsForObject(FWObject *o)
{
    return UsageResolver().findFirewallsForObject(o, this->m_project->db());
}

