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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "FWBSettings.h"

#include "FWObjectPropertiesFactory.h"
#include "FWWindow.h"
#include "ObjectListView.h"
#include "ObjectListViewItem.h"
#include "FWObjectDrag.h"

#include <QHeaderView>
#include <qdrag.h>
#include <qtreewidget.h>
#include <qpixmapcache.h>
#include <qtooltip.h>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

/****************************************************************************
 *
 *    class ObjectListView
 *
 ****************************************************************************/

ObjectListView::ObjectListView(QWidget* parent, const char*,
                               Qt::WindowFlags f) :
    QTreeWidget(parent)
{
    setWindowFlags(f);
    /*setColumnWidthMode(0, QTreeWidget::Maximum);
    setColumnWidthMode(1, QTreeWidget::Maximum);
    setItemMargin( 2 );*/
    setFocusPolicy( Qt::StrongFocus  );
    setFocus();
    header()->setClickable(true);
    header()->setMovable(false);
    setSortingEnabled(true);
    sortByColumn ( 0, Qt::AscendingOrder );
    connect (header (),SIGNAL(sectionClicked (int)),this,SLOT(sectionClicked (int)));
}

bool ObjectListView::event ( QEvent * event )
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

            QTreeWidgetItem      *itm   = itemAt( QPoint(cx,cy - header()->height()) );
            if (itm==NULL) return false;
            ObjectListViewItem *oivi  = dynamic_cast<ObjectListViewItem*>(itm);
            assert(oivi!=NULL);
            obj     = oivi->getFWObject();

            if (obj==NULL) return false;

            cr = visualItemRect(itm);

            QRect global = QRect(
                viewport()->mapToGlobal(cr.topLeft()), viewport()->mapToGlobal(cr.bottomRight()));

            //finally stretch rect up to component's width and even more
            //(it fixes bug with horizontal scroll)
            global.setWidth(width() + horizontalOffset());

            QToolTip::showText(mapToGlobal( he->pos() ),
                FWObjectPropertiesFactory::getObjectPropertiesDetailed(obj,true,true),
                this, global);
        }

        return true;
    }

    return QTreeWidget::event(event);
}

QDrag* ObjectListView::dragObject()
{
    QTreeWidgetItem *ovi = currentItem();
    ObjectListViewItem *otvi=dynamic_cast<ObjectListViewItem*>(ovi);
    assert(otvi!=NULL);

    FWObject *obj = otvi->getFWObject();
    QString icn = (":/Icons/"+obj->getTypeName()+"/icon-ref").c_str();
        //Resources::global_res->getObjResourceStr(obj, "icon-ref").c_str();

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
    drag->setHotSpot( QPoint( pm.rect().width() / 2,
                             pm.rect().height() / 2 ) );

    return drag;
}

void ObjectListView::dragMoveEvent( QDragMoveEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectListView::dragMoveEvent");
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void ObjectListView::dragEnterEvent( QDragEnterEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectListView::dragEnterEvent");
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void ObjectListView::dropEvent(QDropEvent *ev)
{
    if (fwbdebug)
        qDebug("ObjectListView::dropEvent");
    emit dropped(ev);
}

void ObjectListView::keyPressEvent( QKeyEvent* ev )
{
    if (ev->key()==Qt::Key_Delete)
    {
        emit delObject_sign();
    }
    QTreeWidget::keyPressEvent(ev);
}

void ObjectListView::mousePressEvent ( QMouseEvent * event )
{
    startingDrag = true;
    QTreeWidget::mousePressEvent(event);
}

void ObjectListView::mouseMoveEvent ( QMouseEvent * event )
{
    if (startingDrag)
    {
        QDrag *dr = dragObject();
        dr->start();

        startingDrag = false;
    }
    QTreeWidget::mouseMoveEvent(event);
}

void ObjectListView::sectionClicked ( int logicalIndex ) 
{
    sortByColumn ( logicalIndex, Qt::AscendingOrder );
}
