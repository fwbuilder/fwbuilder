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

#include "FWBTree.h"
#include "ProjectPanel.h"
#include "ObjectIconView.h"
#include "ObjectIconViewItem.h"
#include "FWObjectDrag.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Resources.h"

#include <qdrag.h>
#include <qlistwidget.h>
#include <qpixmapcache.h>
#include <qtooltip.h>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

/****************************************************************************
 *
 *    class ObjectIconView
 *
 ****************************************************************************/

ObjectIconView::ObjectIconView(QWidget* parent, const char*, Qt::WindowFlags) :
    QListWidget(parent)
{
    db = nullptr;

    //setWindowFlags(f);

    setDragEnabled(true);
    setViewMode(QListView::IconMode);
    setSpacing(10);
    setAcceptDrops(true);

    //startingDrag = false;
}

bool ObjectIconView::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *he = (QHelpEvent*) event;
        QPoint pos = he->pos();

        if (st->getObjTooltips())
        {
            int cx = pos.x(), cy = pos.y();

            //viewportToContents(pos.x(),pos.y(),cx,cy);

            FWObject *obj = nullptr;
            QRect cr;

            QListWidgetItem *itm = itemAt( QPoint(cx,cy) );
            QModelIndex ind = indexAt( QPoint(cx,cy) );
            if (itm==nullptr) return false;

            int obj_id = itm->data(Qt::UserRole).toInt();
            obj = db->findInIndex(obj_id);
            if (obj==nullptr) return false;

            cr = rectForIndex(ind);
            cr = QRect(
                cr.left() - horizontalOffset(),
                cr.top() - verticalOffset(),
                cr.width(),
                cr.height());

            QRect global = QRect(
                viewport()->mapToGlobal(cr.topLeft()),
                viewport()->mapToGlobal(cr.bottomRight()));

            QToolTip::showText(mapToGlobal( he->pos() ),
                FWObjectPropertiesFactory::getObjectPropertiesDetailed(
                    obj, true, true), this, global);
        }

        return true;
    }

    return QListWidget::event(event);
}

QDrag* ObjectIconView::dragObject()
{
    QListWidgetItem *ivi = currentItem();
    // currentItem returns nullptr if the list is empty
    if (ivi==nullptr) return nullptr;
    int obj_id = ivi->data(Qt::UserRole).toInt();
    FWObject *obj = db->findInIndex(obj_id);
    QString icn =
        Resources::global_res->getObjResourceStr(obj, "icon-ref").c_str();
    list<FWObject*> dragobj;
    dragobj.push_back(obj);

    FWObjectDrag    *drag = new FWObjectDrag(dragobj, this);
    //QPixmap          pm   = QPixmap::fromMimeSource( icn_filename );

    QPixmap pm;
    if ( ! QPixmapCache::find( icn, pm) )
    {
        pm.load( icn );
        QPixmapCache::insert( icn, pm);
    }

    drag->setPixmap( pm );
    drag->setHotSpot(QPoint( pm.rect().width() / 2,
                             pm.rect().height() / 2 ));
    return drag;
}

void ObjectIconView::dragEnterEvent( QDragEnterEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectIconView::dragEnterEvent");
//    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );

    QWidget *fromWidget = qobject_cast<QWidget*>(ev->source());

    // The source of DnD object must be the same instance of fwbuilder
    if (!fromWidget)
    {
        ev->setAccepted(false);
        return;
    }
   
    if (!ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE))
    {
        ev->setAccepted(false);
        return;
    }

    list<FWObject*> dragol;
    if (!FWObjectDrag::decode(ev, dragol))
        ev->setAccepted(false);
    for (list<FWObject*>::iterator i=dragol.begin();i!=dragol.end(); ++i)
    {
        FWObject *dragobj = *i;
        assert(dragobj!=nullptr);

        if (FWBTree().isSystem(dragobj))
        {
// can not drop system folder anywhere 
            ev->setAccepted(false);
            return;
        }

        // see #1976 do not allow pasting object that has been deleted
        if (dragobj->getLibrary()->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
        {
            ev->setAccepted(false);
            return;
        }
    }

    ev->setAccepted(true);
}

void ObjectIconView::dragMoveEvent( QDragMoveEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectIconView::dragMoveEvent");
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void ObjectIconView::dropEvent(QDropEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectIconView::dropEvent");
//    QListWidget::dropEvent(ev);
    emit dropped(ev);
}

void ObjectIconView::keyPressEvent( QKeyEvent* ev )
{
    if (ev->key()==Qt::Key_Delete)
    {
        emit delObject_sign();
    }
    QListWidget::keyPressEvent(ev);
}

void ObjectIconView::mousePressEvent ( QMouseEvent * event )
{
    if (fwbdebug)
        qDebug("ObjectIconView::mousePressEvent");

    startingDrag = true;
    QListWidget::mousePressEvent(event);
}

void ObjectIconView::mouseMoveEvent ( QMouseEvent * event )
{
    if (startingDrag)
    {
        startingDrag = false;
        QDrag *dr = dragObject();
        if (dr) dr->start();
    }
    QListWidget::mouseMoveEvent(event);
}


