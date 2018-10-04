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

#include "FWBSettings.h"
#include "FWBTree.h"
#include "FWObjectDrag.h"
#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "IconSetter.h"
#include "ObjectManipulator.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Network.h"

#include <QAbstractItemView>
#include <QBitmap>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>
#include <QtDebug>
#include <qpainter.h>
#include <qpixmapcache.h>

#include <iostream>
#include <algorithm>

using namespace std;
using namespace libfwbuilder;

/****************************************************************************
 *
 *    class ObjectTreeView
 *
 ****************************************************************************/

ObjectTreeView::ObjectTreeView(ProjectPanel* project,
                               QWidget* parent,
                               const char * name,
                               Qt::WindowFlags f) :
    QTreeWidget(parent), 
    m_project(project)
{
    setObjectName(name);
    this->setParent(parent, f);
    setFont(st->getTreeFont());

    QPalette updated_palette = palette();
    updated_palette.setColor(
        QPalette::Inactive,
        QPalette::Highlight,
        QColor("silver"));

//        palette().color(QPalette::Highlight).lighter(300));

    setPalette(updated_palette);

    setExpandsOnDoubleClick(false);

    setDragEnabled(true);
    item_before_drag_started=nullptr;
    lastSelected = nullptr;
    second_click = false;
    selectionFrozen = false;
    expandOrCollapse = false;
    Lockable = false;
    Unlockable = false;
    visible = false;
    /*
     * note about process_mouse_release_event
     *
     * we use mouseReleaseEvent event to switch object opened in the
     * editor panel (i.e. we open new object when mouse button is
     * released rather than when it is pressed). This allows us to
     * start drag without switching object in the editor. The problem
     * is that mouseReleaseEvent is received in this widget after the
     * d&d ends with a drop somewhere else, which triggers call to
     * mouseReleaseEvent which switches object in the
     * editor. This is undesired when the editor shows a group and we
     * try to drag and drop an object into that group. Flag
     * process_mouse_release_event is used to suppress object
     * switching when mouseReleaseEvent is called after
     * successfull drop.
     */
    process_mouse_release_event = true;

    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(currentItemChanged(QTreeWidgetItem*)));

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));

    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(itemCollapsed(QTreeWidgetItem*)));

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(itemExpanded(QTreeWidgetItem*)));

    QStringList qsl;
    qsl.push_back(tr("Object"));
    qsl.push_back(tr("Attributes"));
    setHeaderLabels(qsl);

    //header()->hide();

    header()->setDefaultAlignment(Qt::AlignLeft);
    header()->setSectionResizeMode(QHeaderView::Interactive);

    showOrHideAttributesColumn();

    setMinimumSize( QSize( 100, 0 ) );

    setAutoScroll(true);
    setAutoScrollMargin(50);
    setAllColumnsShowFocus( true );
    setSelectionMode( ExtendedSelection );
    setAcceptDrops( true );
    setDragDropMode( QAbstractItemView::DragDrop );
    setRootIsDecorated( true );

    setFocusPolicy(Qt::StrongFocus);

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this,
            SLOT(updateFilter()));

// disable sorting, otherwise gui crashes when built with
// QT 4.3.4 (discovered on Ubuntu Hardy). Crash happened when
// second object was added to any branch of the tree.
//
// This causes crash with Qt 4.6 as well
//
//    setSortingEnabled(true);

}

void ObjectTreeView::paintEvent(QPaintEvent *ev)
{
    QTreeWidget::paintEvent(ev);
}

void ObjectTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index ) const
{
    // qDebug() << "ObjectTreeView::drawRow"
    //          << "QStyleOptionViewItem.state=" << int(option.state)
    //          << "hasFocus()=" << hasFocus()
    //          << "isActiveWindow()=" << isActiveWindow();

    // QWidget *fw = QApplication::focusWidget();
    // qDebug() << "Currently has focus:" << fw;

    // this is a patch for #2475
    // Looks like the state remains State_Active even when the tree view widget
    // loses focus (as long as parent window is active).
    QStyleOptionViewItem new_opt = option;
    if ( ! hasFocus()) new_opt.state &= ~QStyle::State_Active;
    QTreeWidget::drawRow(painter, new_opt, index);
}

bool ObjectTreeView::event( QEvent *event )
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *he = (QHelpEvent*) event;
        QPoint pos = he->pos();

        if (st->getObjTooltips())
        {
            int cx = pos.x(), cy = pos.y();

            FWObject  *obj=nullptr;
            QRect      cr;

            QTreeWidgetItem *itm = itemAt(QPoint(cx, cy - header()->height()));
            if (itm==nullptr) return false;
            ObjectTreeViewItem *oivi  = dynamic_cast<ObjectTreeViewItem*>(itm);
            assert(oivi!=nullptr);
            obj = oivi->getFWObject();

            if (obj==nullptr) return false;

            if (obj->getId() == FWObjectDatabase::ANY_ADDRESS_ID  ||
                obj->getId() == FWObjectDatabase::ANY_SERVICE_ID  ||
                obj->getId() == FWObjectDatabase::ANY_INTERVAL_ID ||
                obj->getId() == FWObjectDatabase::DUMMY_ADDRESS_ID ||
                obj->getId() == FWObjectDatabase::DUMMY_SERVICE_ID ||
                obj->getId() == FWObjectDatabase::DUMMY_INTERFACE_ID)
                return false;

            cr = visualItemRect(itm);

            QRect global = QRect(
                viewport()->mapToGlobal(cr.topLeft()),
                viewport()->mapToGlobal(cr.bottomRight()));

            //finally stretch rect up to component's width and even more
            //(it fixes bug with horizontal scroll)
            global.setWidth(width() + horizontalOffset());

            QToolTip::showText(mapToGlobal( he->pos() ),
                FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj,
                                                                       true,
                                                                       true),
                this, global);
        }

        return true;
    }

    return QTreeWidget::event(event);
}

void ObjectTreeView::currentItemChanged(QTreeWidgetItem*)
{
    expandOrCollapse = false;
}

void ObjectTreeView::itemCollapsed(QTreeWidgetItem* itm)
{
    expandOrCollapse = true;

    ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(itm);
    assert(otvi!=nullptr);
    FWObject *o = otvi->getFWObject();
    if (o)
    {
        int id = o->getId();
        expanded_objects.erase(id);
    }
}

void ObjectTreeView::itemExpanded(QTreeWidgetItem* itm)
{
    expandOrCollapse = true;

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(itm);
    assert(otvi!=nullptr);
    FWObject *o = otvi->getFWObject();
    if (o)
    {
        int id = o->getId();
        expanded_objects.insert(id);
    }
}

/*
 * This method makes list selectedObjects flat. If user selects
 * several objects in the tree, and some of them have children, QT
 * puts all the children in the selected objects list even if
 * corresponding subtrees are collapsed. This method eliminates these
 * selected children objects.
 *
 */
std::vector<libfwbuilder::FWObject*> ObjectTreeView::getSimplifiedSelection()
{
    vector<FWObject*> so  = selectedObjects;
    vector<FWObject*> so2 = selectedObjects;
    for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
    {
        for (vector<FWObject*>::iterator j=i;  j!=so2.end(); ++j)
        {
            vector<FWObject*>::iterator k=std::find(so.begin(),so.end(),*j);
            if ( (*j)->isChildOf( *i ) && k!=so.end())
                so.erase( k );
        }
    }
    return so;
}

FWObject* ObjectTreeView::getCurrentObject()
{
    QTreeWidgetItem *ovi = currentItem();
    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(ovi);
    if (otvi==nullptr) return nullptr;
    return otvi->getFWObject();
}

void ObjectTreeView::focusInEvent(QFocusEvent* ev)
{
    QTreeWidget::focusInEvent(ev);
    QTreeWidgetItem *ci = currentItem();
    if (ci) repaint();
}

void ObjectTreeView::focusOutEvent(QFocusEvent* ev)
{
    QTreeWidget::focusOutEvent(ev);
    QTreeWidgetItem *ci = currentItem();
    if (ci) repaint();
}

void ObjectTreeView::updateTreeIcons()
{
    QTreeWidgetItemIterator it(this);
    for ( ; *it; ++it)
    {
        QTreeWidgetItem *itm = *it;
        ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(itm);
        FWObject *obj = otvi->getFWObject();

        /* We can have obj==0 if it's a user-create subfolder */
        if (obj == nullptr) continue;

        QPixmap pm_obj;
        IconSetter::setObjectIcon(obj, &pm_obj, 0);
        itm->setIcon(0, pm_obj );
    }
    update();
}

void ObjectTreeView::startDrag(Qt::DropActions supportedActions)
{
    QTreeWidgetItem *ovi = currentItem();
    if (ovi==nullptr) return;

    FWObject *current_obj = getCurrentObject();

    /* User-defined folders can't be dragged */
    if (current_obj == nullptr) return;

    if (fwbdebug) qDebug("ObjectTreeView::startDrag: this: %p current_obj: %s",
                         this, current_obj->getName().c_str());

/* can't drag system folders

    in fact, I have to allow to drag system folders because otherwise
    QListView triggers highlighting of objects in the tree when user
    drags mouse cursor across them. This is weird behavior and there
    does not seem to be any way to turn it off. It happens close to
    the end of void QListView::mouseMoveEvent( QMouseEvent * e)
    (See code after they decided that they do not need to call startDrag())

    if (FWBTree().isSystem(obj)) return nullptr;
*/
    if ((current_obj->getId() == FWObjectDatabase::DUMMY_ADDRESS_ID)   ||
        (current_obj->getId() == FWObjectDatabase::DUMMY_INTERFACE_ID) ||
        (current_obj->getId() == FWObjectDatabase::DUMMY_SERVICE_ID))
        return;

    QString icn = (":/Icons/"+current_obj->getTypeName()+"/icon-ref").c_str();

    vector<FWObject*> so = getSimplifiedSelection();

    list<FWObject*> dragobj;
    for (vector<FWObject*>::iterator v=so.begin(); v!=so.end(); v++)
    {   
        //m_project->check4Depends(*v, dragobj);

        if (fwbdebug)
            qDebug("ObjectTreeView::startDrag: adding object to drag list: %s",
                   (*v)->getName().c_str());

        // while obj is still part of the tree, do some clean up
        // to avoid problems in the future.  Create
        // InterfaceOptions objects for interfaces because we'll
        // need them for various validations during paste/drop
        // operation.
        Interface *intf = Interface::cast(*v);
        if (intf) intf->getOptionsObject();

        dragobj.push_back( *v );
    }
    FWObjectDrag *drag = new FWObjectDrag(dragobj, this);

    QPixmap pm;
    if ( ! QPixmapCache::find( icn, pm) )
    {
        pm.load( icn );
        QPixmapCache::insert( icn, pm);
    }

    if (dragobj.size()>1)
    {
        QPixmap npm(32,32);
        QPainter p( &npm );
        p.fillRect( 0,0,32,32, QBrush( QColor("white"),Qt::SolidPattern ) );
        p.setBackgroundMode( Qt::TransparentMode );
        p.drawPixmap( 0, 32-pm.rect().height(), pm);
        p.setPen( QColor("red") );
        p.setBrush( QBrush( QColor("red"),Qt::SolidPattern ) );
        p.drawPie( 16, 0, 16,16, 0, 5760 );
        QString txt;
        txt.setNum(dragobj.size());
        QRect br=p.boundingRect(0, 0, 1000, 1000,
                                Qt::AlignLeft|Qt::AlignVCenter,
                                txt );
        p.setPen( QColor("white") );
        p.drawText( 24-br.width()/2 , 4+br.height()/2, txt );
        p.end();
        npm.setMask( npm.createHeuristicMask() );
        drag->setPixmap( npm );
    } else
        drag->setPixmap( pm );

/*
 * This fragment returns selection in the tree back to the object that
 * was selected before drag operation has started. This help in the
 * following case:
 *
 *  - open a group for editing (group is selected in the tree)
 *  - left-click on another object in the tree, start dragging it
 *
 * at this point selection in the tree returns to the group, so when
 * user finishes d&d operation, the selection in the tree is consisten
 * with object currently opened in the editor panel.
 *
 * There is a problem with this however. If user wants to put an
 * object from a different library into the group, they have to switch
 * to that library before doing d&d. When they switch, ObjectTree
 * shown in the left panel becomes different from the tree in which
 * the group is located. When d&d finishes, the ObjectTree object
 * receives mouseReleaseEvent event. Since it is not the right
 * tree object, it can not properly restore selection and choses an
 * object that was previously opened in that tree, which in turn
 * changes the object opened in the editor panel. To make things
 * worse, this event is only delivered to the tree object on Mac OS X.
 *
 *
 */
    if (fwbdebug) qDebug("ObjectTreeView::dragObject()  this=%p visible=%d",
                         this,visible);

    drag->start(supportedActions);
}

void ObjectTreeView::dragEnterEvent( QDragEnterEvent *ev)
{
    ev->setAccepted(ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
    ev->setDropAction(Qt::MoveAction);
}


static bool isValidDropTarget(QTreeWidgetItem *item, list<FWObject *> &objs)
{
    ObjectTreeViewItem *dest = dynamic_cast<ObjectTreeViewItem *>(item);
    if (dest == nullptr) return false;

    bool dragIsNoop = true;
    list<FWObject *>::const_iterator iter;
    for (iter = objs.begin(); iter != objs.end(); ++iter) {
        FWObject *dragobj = *iter;
        assert(dragobj != nullptr);

        if (Interface::cast(dragobj) != nullptr ||
            Interface::cast(dragobj->getParent()) != nullptr ||
            Policy::cast(dragobj) != nullptr ||
            NAT::cast(dragobj) != nullptr ||
            Routing::cast(dragobj) != nullptr) return false;

        /* See if destination is a user folder */
        if (dest->getUserFolderParent() != nullptr) {
            /* Dragged object has to match parent of user folder */
            if (dest->getUserFolderParent() != dragobj->getParent()) {
                return false;
            }

            /* Are we dragging within the same user folder? */
            if (dest->getUserFolderName() !=
                QString::fromUtf8(dragobj->getStr("folder").c_str())) {
                dragIsNoop = false;
            }
        } else {
            /* OK to drag onto parent itself, or object that shares parent */
            if (dragobj->getParent() != dest->getFWObject() &&
                dragobj->getParent() != dest->getFWObject()->getParent()) {
                return false;
            }

            /* Are we dragging to a new place? */
            if ((FWBTree().isSystem(dest->getFWObject()) &&
                 dragobj->getStr("folder") != "") ||
                (dest->getFWObject()->getStr("folder") !=
                 dragobj->getStr("folder"))) {
                dragIsNoop = false;
            }
        }
    }

    return !dragIsNoop;
}


void ObjectTreeView::dragMoveEvent(QDragMoveEvent *ev)
{
    /* Call the parent so that auto-scrolling works properly */
    QTreeWidget::dragMoveEvent(ev);

    list<FWObject*> objs;
    if (ev->source() != this || !FWObjectDrag::decode(ev, objs) ||
        !isValidDropTarget(itemAt(ev->pos()), objs)) {
        ev->setAccepted(false);
        return;
    }

    ev->setDropAction(Qt::MoveAction);
    ev->setAccepted(true);
}


void ObjectTreeView::dropEvent(QDropEvent *ev)
{
    // only accept drops from the same instance of fwbuilder
    if (ev->source() == nullptr) return;

    ObjectTreeViewItem *dest =
        dynamic_cast<ObjectTreeViewItem *>(itemAt(ev->pos()));
    if (dest == nullptr) {
    notWanted:
        ev->setAccepted(false);
        return;
    }

    list<FWObject*> objs;
    if (!FWObjectDrag::decode(ev, objs)) goto notWanted;

    /* Make sure the drop event is on an object that can handle it */
    if (ev->source() != this || !isValidDropTarget(dest, objs)) goto notWanted;

    emit moveItems_sign(dest, objs);
    ev->setAccepted(true);
}

void ObjectTreeView::dragLeaveEvent( QDragLeaveEvent *ev)
{
    QTreeWidget::dragLeaveEvent(ev);

    clearSelection();
}

void ObjectTreeView::mouseMoveEvent( QMouseEvent * e )
{
    /* This stops highlighting of stuff in the tree when the user
       clicks and tries to drag something non-draggable. */
    if (state() == DragSelectingState) return;
    QTreeWidget::mouseMoveEvent(e);
    if (e==nullptr)  return;
}

void ObjectTreeView::mousePressEvent( QMouseEvent *e )
{
    if (fwbdebug) qDebug("ObjectTreeView::mousePressEvent");

    second_click = false;
    process_mouse_release_event = true;

    if (fwbdebug)
    {
        qDebug() << "ObjectTreeView::mousePressEvent :: currentItem="
                 << ((currentItem())?currentItem()->text(0):"nil");
        qDebug() << "ObjectTreeView::mousePressEvent :: lastSelected="
                 << ((lastSelected)?lastSelected->text(0):"nil");
    }

    lastSelected = currentItem();

    QTreeWidget::mousePressEvent(e);

    if (e->button() == Qt::LeftButton)
    {
        startingDrag = true;
    }

    if (e->button() == Qt::RightButton)
        emit contextMenuRequested_sign(e->pos());
}

/*
 * Two modes of operation of this widget:
 *
 * 1.  this widget can intercept single mouse click and return
 * selection back to the object that was current before it. If user
 * double ckicks mouse button, then this reset is not done and new
 * object is selected. This is done using timer.
 *
 * 2. this widget can act as usual QListView does, that is, select an object
 * on a single click.
 *
 * uncomment the line that starts timer for mode #1.
 *
 *
 * we use mouseReleaseEvent event to switch object opened in the
 * editor panel (i.e. we open new object when mouse button is released
 * rather than when it is pressed). This allows us to start drag
 * without switching object in the editor. The problem is that
 * mouseReleaseEvent is received in this widget after the d&d ends
 * with a drop somewhere else, which triggers call to
 * mouseReleaseEvent which switches object in the editor. This
 * is undesired when the editor shows a group and we try to drag and
 * drop an object into that group. Flag process_mouse_release_event is
 * used to suppress object switching when mouseReleaseEvent is
 * called after successfull drop.
 *
 */
void ObjectTreeView::mouseReleaseEvent( QMouseEvent *e )
{
    if (fwbdebug)
        qDebug("ObjectTreeView::mouseReleaseEvent 1 this=%p  process_mouse_release_event=%d",
               this,process_mouse_release_event);

    QTreeWidget::mouseReleaseEvent(e);

    if (!process_mouse_release_event)
    {
        // just do not switch object in the editor, otherwise
        // process this event as usual
        process_mouse_release_event = true;
        return;
    }

    if (fwbdebug)
        qDebug("ObjectTreeView::mouseReleaseEvent 2 selectedObjects.size()=%d getCurrentObject()=%p current object %s",
               int(selectedObjects.size()),
               getCurrentObject(),
               (getCurrentObject()!=nullptr)?getCurrentObject()->getName().c_str():"nil");

    if (expandOrCollapse) return;  // user expanded or collapsed subtree,
                                   // no need to change object in the editor

// Experiment: single click on the object in the tree should not open
// it in the editor
#if 0
    if (selectedObjects.size()==1)
        emit switchObjectInEditor_sign( getCurrentObject() );
    else
    {
        // user selected multiple objects
        // do not let them if editor has unsaved changes
        //
        if (mw->isEditorVisible() && mw->isEditorModified())
            emit switchObjectInEditor_sign( getCurrentObject() );
        else
            mw->blankEditor();
    }
#endif
}

/*
 * normally QAbstractItemView::edit starts in-place editing. We use
 * double click to open object in a separate editor panel 
 */
bool ObjectTreeView::edit(const QModelIndex &index,
                          EditTrigger trigger, QEvent *event)
{
    if (fwbdebug) qDebug("ObjectTreeView::edit");
    if (trigger==QAbstractItemView::DoubleClicked) editCurrentObject();
    return QTreeWidget::edit(index, trigger, event);
}

/*
 * sends signal that should be connected to a slot in
 * ObjectManipulator which opens editor panel if it is closed and then
 * opens current object in it
 */
void ObjectTreeView::editCurrentObject()
{
    if (fwbdebug) qDebug("ObjectTreeView::editCurrentObject");
    emit editCurrentObject_sign();
    if (fwbdebug) qDebug("ObjectTreeView::editCurrentObject done");
}

void ObjectTreeView::keyPressEvent( QKeyEvent* ev )
{
    FWObject *obj = getCurrentObject();
    if (obj)
    {
        if (ev->key()==Qt::Key_Enter || ev->key()==Qt::Key_Return)
        {
            if (fwbdebug)
                qDebug() << "ObjectTreeView::keyPressEvent Qt::Key_Enter";
            editCurrentObject();
            ev->accept();
            return;
        }
        if (ev->key()==Qt::Key_Delete)
        {
            emit deleteObject_sign(obj);
            ev->accept();
            return;
        }
    }
    QTreeWidget::keyPressEvent(ev);
}

void ObjectTreeView::keyReleaseEvent( QKeyEvent* ev )
{
    QTreeWidget::keyReleaseEvent(ev);
}

void ObjectTreeView::itemOpened ()
{
    if (fwbdebug) qDebug("ObjectTreeView::itemOpened");
    editCurrentObject();
}

void ObjectTreeView::clearLastSelected()
{
    lastSelected = nullptr;
}


void ObjectTreeView::resetSelection()
{
    if (lastSelected)
    {
        if (fwbdebug)
            qDebug() << "ObjectTreeView::resetSelection :: lastSelected="
                     << lastSelected->text(0);

        setCurrentItem(lastSelected);
        lastSelected->setSelected(true);
    }
}

void ObjectTreeView::itemSelectionChanged()
{
    if (fwbdebug)
        qDebug("ObjectTreeView::itemSelectionChanged selectionFrozen=%d",
               selectionFrozen);

    if (selectionFrozen) return;

/* in extended selection mode there may be several selected items */

    selectedObjects.clear();

    QList<QTreeWidgetItem*> selected = selectedItems();
    QList<QTreeWidgetItem*>::Iterator it;
    for (it=selected.begin(); it!=selected.end(); it++)
    {
        QTreeWidgetItem *itm = (*it);
        ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(itm);

        FWObject *obj = otvi->getFWObject();
        if (obj == nullptr) continue;

        selectedObjects.push_back(otvi->getFWObject());

        if (fwbdebug) qDebug(
            "ObjectTreeView::selectionChanged: selected otvi=%p object %s",
            otvi, otvi->getFWObject()->getName().c_str());
    }

    if (fwbdebug)
        qDebug("ObjectTreeView::itemSelectionChanged completed");
/* now list  selectedObjects   holds all selected items */
}

bool ObjectTreeView::isSelected(FWObject* obj)
{
    for (vector<FWObject*>::iterator i=selectedObjects.begin();
         i!=selectedObjects.end(); ++i)
    {
        if ( (*i)==obj)  return true;
    }
    return false;
}

int  ObjectTreeView::getNumSelected()
{
    return selectedObjects.size();
}

void ObjectTreeView::updateAfterPrefEdit()
{
     setFont(st->getTreeFont());
}

void ObjectTreeView::ExpandTreeItems(const set<int> &ids)
{
    if (fwbdebug)
        qDebug() << "ObjectTreeView::ExpandTreeItems()";

    QTreeWidgetItemIterator it(this);
    for ( ; *it; ++it)
    {
        QTreeWidgetItem *itm = *it;
        ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(itm);
        FWObject *obj = otvi->getFWObject();
        if (obj == nullptr) continue;
        if (ids.count(obj->getId()))
            itm->setExpanded(true);
    }
}

void ObjectTreeView::showOrHideAttributesColumn()
{
    if (st->getBool("UI/ShowObjectsAttributesInTree"))
        setColumnCount(2);
    else
        setColumnCount(1);
}

QSet<QTreeWidgetItem*> ObjectTreeView::resolveChildren(QTreeWidgetItem *parent)
{
    QSet<QTreeWidgetItem*> children;
    children.insert(parent);
    if (parent->childCount() == 0) return children;
    for (int i=0; i<parent->childCount(); i++)
        children.unite(resolveChildren(parent->child(i)));
    return children;
}

QSet<QTreeWidgetItem*> ObjectTreeView::resolveParents(QTreeWidgetItem *child)
{
    QSet<QTreeWidgetItem*> parents;
    parents.insert(child);
    if (child->parent() == nullptr) return parents;
    parents.unite(resolveParents(child->parent()));
    return parents;
}

void ObjectTreeView::updateFilter()
{
    if (filter.isEmpty()) return;
    setFilter(filter);
}

static bool filterMatchesPortRange(const QStringList &args,
                                   FWObject *obj)
{
    if (!obj) return false;

    // We traverse the service group. If the children are references
    // they may be pointing to ports
    if (obj->getTypeName() == ServiceGroup::TYPENAME) {
        for (list<FWObject*>::const_iterator it=obj->begin(); it!=obj->end(); ++it) {
            FWServiceReference *ref = FWServiceReference::cast(*it);
            if (ref && filterMatchesPortRange(args, ref->getPointer()))
                return true;
        }
    }

    TCPUDPService *service = dynamic_cast<TCPUDPService*>(obj);
    if (!service) return false;

    QRegExp rx("\\s*([><]?)\\s*(\\d*)(?:-(\\d*))?");

    foreach (const QString &arg, args) {

        if (!rx.exactMatch(arg)) continue;

        int lowerBound = rx.cap(2).toInt(), upperBound = lowerBound;

        if (rx.pos(3) != -1) {
            upperBound = rx.cap(3).toInt();
        }

        if (rx.pos(1) != -1) {
            if (rx.pos(3) != -1) // [><] cannot be combined with range
                continue;

            if (rx.cap(1) == ">") {
                upperBound = 65535;
                ++lowerBound; // Adjust for using >= below
            } else {// "<"
                lowerBound = 1;
                --upperBound; // Adjust for using <= below
            }
        }

        if (lowerBound > upperBound) continue;

        int ds = service->getDstRangeStart(), de = service->getDstRangeEnd(),
                ss = service->getSrcRangeStart(), se = service->getSrcRangeEnd();

        if (ds && de && (lowerBound <= ds) && (de <= upperBound)) return true;
        if (ss && se && (lowerBound <= ss) && (se <= upperBound)) return true;
    } // End foreach

    return false;
}

static bool filterMatchesIpAddress(const QStringList &args,
                                   FWObject *obj)
{
    if (!obj) return false;

    // We traverse the object group. If the children are references
    // they may be pointing to adresses
    if (obj->getTypeName() == ObjectGroup::TYPENAME) {
        for (list<FWObject*>::const_iterator it=obj->begin(); it!=obj->end(); ++it) {
            FWObjectReference *ref = FWObjectReference::cast(*it);
            if (ref && filterMatchesIpAddress(args, ref->getPointer()))
                return true;
        }
    }

    Address *addr = dynamic_cast<Address*>(obj);
    if (!addr) return false;

    QRegExp rx("\\s*([.:0-9a-fA-F]+)(?:/([.:0-9a-fA-F]+))?");

    InetAddrMask searchAddrAndMask;
    foreach (const QString &arg, args) {

        if (!rx.exactMatch(arg)) continue;

        try {
            std::string netmask = rx.cap(2).isEmpty() ? "32" : rx.cap(2).toStdString();
            InetAddr ipv4addr(rx.cap(1).toStdString());
            InetAddr ipv4mask(netmask);
            searchAddrAndMask = InetAddrMask(ipv4addr, ipv4mask);
        } catch (const FWException &) { // Could not create IPv4 object. Trying IPv6.
            try {
                int netmask = rx.cap(2).isEmpty() ? 128 : rx.cap(2).toInt();
                InetAddr ipv6addr(AF_INET6, rx.cap(1).toStdString());
                InetAddr ipv6mask(AF_INET6, netmask);
                searchAddrAndMask = InetAddrMask(ipv6addr, ipv6mask);
            } catch (const FWException &) { // Could not create IPv6 object.
                // User did not submit a valid IP address
                return false;
            }
        }

        const InetAddr *searchAddr = searchAddrAndMask.getAddressPtr();

        if (addr->getTypeName() == AddressRange::TYPENAME) {
            AddressRange *addrRange = dynamic_cast<AddressRange*>(obj);
            if (addrRange
                && (searchAddr->addressFamily() == addrRange->getRangeStart().addressFamily()) ) {

                if ( !(searchAddr->opLT(addrRange->getRangeStart()))
                     && !(searchAddr->opGT(addrRange->getRangeEnd())) )
                    return true;
            }
            continue; // Next argument
        }

        const InetAddr *inetAddr = addr->getAddressPtr();


        if ( inetAddr && (inetAddr->addressFamily() == searchAddr->addressFamily()) ) {
            if (addr->getTypeName() == Network::TYPENAME) {
                if (addr->belongs(*searchAddr))
                    return true;
            }
            if (searchAddrAndMask.belongs(*inetAddr))
                return true;
        }
    } // End foreach

    return false;
}

static bool filterMatchesCommand(const QString &text,
                                 ObjectTreeViewItem *item)
{
    QRegExp rx("(?:(port)|(ip)):(.*)", Qt::CaseInsensitive);
    if (!rx.exactMatch(text)) return false;

    QStringList args = rx.cap(3).split(",", QString::SkipEmptyParts);

    if (rx.pos(1) != -1)
        return (filterMatchesPortRange(args, item->getFWObject()));
    else
        return (filterMatchesIpAddress(args, item->getFWObject()));
}

static bool filterMatches(const QString &text,
                          ObjectTreeViewItem *item)
{
    if (text.isEmpty()) return true;
    if (item->text(0).contains(text, Qt::CaseInsensitive)) return true;

    // Support for port and ip search
    if (filterMatchesCommand(text, item)) return true;

    if (item->getUserFolderParent() != nullptr) return false;
    FWObject *obj = item->getFWObject();

    QByteArray utf8 = text.toUtf8();
    set<string> keys = obj->getKeywords();
    set<string>::const_iterator iter;
    for (iter = keys.begin(); iter != keys.end(); ++iter) {
        QString keyword = QString::fromUtf8((*iter).c_str());
        if (keyword.contains(text, Qt::CaseInsensitive)) return true;
    }

    return false;
}

static uint qHash(const QStringList &list)
{
    uint ret = 0;
    for (int ii = 0; ii < list.size(); ii++) {
        ret += qHash(list.at(ii));
    }
    return ret;
}

void ObjectTreeView::doExpandedState(bool save, QStringList &list,
                                     QTreeWidgetItem *item)
{
    list.append(item->text(0));
    if (save) {
        if (item->isExpanded()) expandedState.insert(list);
    } else {
        if (expandedState.contains(list)) item->setExpanded(true);
    }

    for (int ii = 0; ii < item->childCount(); ii++) {
        doExpandedState(save, list, item->child(ii));
    }
    list.removeLast();
}


void ObjectTreeView::setFilter(QString text)
{
    if (filter.isEmpty() && !text.isEmpty()) {
        QStringList list;
        for (int ii = 0; ii < topLevelItemCount(); ii++) {
            doExpandedState(true, list, topLevelItem(ii));
        }
    } else if (text.isEmpty() && !filter.isEmpty()) {
        QStringList list;
        for (int ii = 0; ii < topLevelItemCount(); ii++) {
            doExpandedState(false, list, topLevelItem(ii));
        }
        expandedState.clear();
    }
    filter = text;

    if (fwbdebug)
        qDebug() << "ObjectTreeView::setFilter " << text;

    list<QTreeWidgetItem *> expand;
    for (QTreeWidgetItemIterator wit(this); *wit; ++wit) {
        ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem *>(*wit);

        if (filterMatches(text, otvi)) {
            (*wit)->setHidden(false);

            if (Firewall::cast(otvi->getFWObject()) != nullptr) {
                expand.push_back(otvi);
            }

            QTreeWidgetItem *parent = (*wit)->parent();
            while (parent != nullptr) {
                parent->setHidden(false);
                parent = parent->parent();
            }
        } else {
            (*wit)->setHidden(true);
        }
    }

    list<QTreeWidgetItem *>::const_iterator iter;
    for (iter = expand.begin(); iter != expand.end(); ++iter) {
        QTreeWidgetItem *item = *iter;
        item->setHidden(false);
        for (int ii = 0; ii < item->childCount(); ii++) {
            expand.push_back(item->child(ii));
        }
    }

    if (!text.isEmpty()) this->expandAll();
}
