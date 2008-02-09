/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectTreeView.h,v 1.22 2007/01/08 03:37:53 vkurland Exp $

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


#ifndef  __OBJECTTREEVIEW_H_
#define  __OBJECTTREEVIEW_H_

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMimeData>
#include <qtimer.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QDragLeaveEvent>

#include <vector>

namespace libfwbuilder {
    class FWObject;
};

class ObjectTreeView : public QTreeWidget {

     Q_OBJECT

    QTreeWidgetItem   *item_before_drag_started;
    QTreeWidgetItem   *lastSelected;
    QItemSelection     lastSelection;
    QTimer           singleClickTimer;
    bool             second_click;

    bool             selectionFrozen;
    bool             expandOrCollapse;
    bool             Lockable;
    bool             Unlockable;
    bool             startingDrag;

    bool             visible;

    bool             process_mouse_release_event;

    std::vector<libfwbuilder::FWObject*>          selectedObjects;
        
 protected:

    bool event( QEvent *event );

    virtual void dragEnterEvent( QDragEnterEvent *ev);
    virtual void dragMoveEvent( QDragMoveEvent *ev);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragLeaveEvent( QDragLeaveEvent *ev);

    virtual void keyPressEvent( QKeyEvent* ev );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseDoubleClickEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );

    virtual void keyReleaseEvent( QKeyEvent* ev );

    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);

    void startDrag(Qt::DropActions supportedActions);
    
 public:

    ObjectTreeView(QWidget* parent = 0, const char * name = 0, Qt::WFlags f = 0);

    void freezeSelection(bool f) { selectionFrozen=f; }

    std::vector<libfwbuilder::FWObject*>& getSelectedObjects()
        { return selectedObjects; }

    bool isSelected(libfwbuilder::FWObject* obj);
    int  getNumSelected();
    bool isLockable();
    bool isUnlockable();
    void setLockFlags();

    libfwbuilder::FWObject* getCurrentObject();

    void editCurrentObject();

    void clearLastSelected();

    void becomingVisible() { visible=true; }
    void becomingHidden()  { visible=false; }
    void updateAfterPrefEdit();
/* Under some circumstances, user may select several host or fw
 * objects so that their children objects are selected as well
 * (e.g. when shift-click is used). "Delete objects" or "group
 * objects" operations will work on all children objects, which leads
 * to unexpected results since it is not obvious to the user that
 * children objects were selected (since they are invisible). We need
 * to remove them from the list before we delete or perform other
 * actions.
 */
    std::vector<libfwbuilder::FWObject*> getSimplifiedSelection();

    void ignoreNextMouseReleaseEvent() { process_mouse_release_event = false; }
    
 public slots:

     void itemSelectionChanged();
     void resetSelection();
     void currentItemChanged(QTreeWidgetItem *cur);
     void itemCollapsed(QTreeWidgetItem *itm);
     void itemExpanded(QTreeWidgetItem *itm);
     void itemOpened ();
     virtual void updateTreeItems();

 signals:

//     void showObjectInfo_sign(libfwbuilder::FWObject *);
     void editCurrentObject_sign();
     void switchObjectInEditor_sign(libfwbuilder::FWObject*);
     void objectDropped_sign(libfwbuilder::FWObject *);
     void deleteObject_sign(libfwbuilder::FWObject *);
     void contextMenuRequested_sign(const QPoint&);
};


#endif

