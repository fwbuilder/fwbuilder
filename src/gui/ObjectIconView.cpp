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

#include "fwbuilder/FWObjectDatabase.h"
#include "FWWindow.h"
#include "ObjectIconView.h"
#include "ObjectIconViewItem.h"
#include "FWObjectDrag.h"
#include "FWBSettings.h"

#include "FWObjectPropertiesFactory.h"
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

            FWObject  *obj=NULL;
            QRect      cr;

            QListWidgetItem *itm = itemAt( QPoint(cx,cy) );
            QModelIndex ind = indexAt( QPoint(cx,cy) );
            if (itm==NULL) return false;

            int obj_id = itm->data(Qt::UserRole).toInt();
            obj = mw->db()->findInIndex(obj_id);
            if (obj==NULL) return false;

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
                FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj,
                                                                       true,
                                                                       true),
                this, global);
        }

        return true;
    }

    return QListWidget::event(event);
}

QDrag* ObjectIconView::dragObject()
{
    QListWidgetItem *ivi = currentItem();
    int obj_id = ivi->data(Qt::UserRole).toInt();
    FWObject *obj = mw->db()->findInIndex(obj_id);
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
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
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

        if (dr)
            dr->start();
    }
    QListWidget::mouseMoveEvent(event);
}


