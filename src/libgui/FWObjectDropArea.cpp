/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "events.h"

#include "FWObjectDropArea.h"
#include "FWObjectDrag.h"
#include "FWObjectClipboard.h"
#include "FWWindow.h"
#include "ProjectPanel.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleSet.h"

#include <qpainter.h>
#include <qrect.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qrect.h>
#include <qmenu.h>
//Added by qt3to4:
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QCoreApplication>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

FWObjectDropArea::~FWObjectDropArea()
{
    delete m_objectDropArea;
}

FWObjectDropArea::FWObjectDropArea(QWidget*p, const char * n, Qt::WindowFlags f):
    QWidget(p)
{
    setObjectName( QString(n) );
    setWindowFlags( f );
    m_objectDropArea = new Ui::FWObjectDropArea_q;
    m_objectDropArea->setupUi(this);
    object=nullptr;
    helperText = tr("Drop object here.");

}

void FWObjectDropArea::paintEvent(QPaintEvent *)
{
    int w=width();
    int h=height();

    QPainter p(this);

    QPixmap bufferpixmap;
    bufferpixmap = QPixmap( w , h );
    bufferpixmap.fill( Qt::white );
    QPainter tp( &bufferpixmap );

    tp.setBrush(Qt::black);
    tp.drawLine(0,0,w-1,0);
    tp.drawLine(w-1,0,w-1,h-1);
    tp.drawLine(w-1,h-1,0,h-1);
    tp.drawLine(0,h-1,0,0);
    tp.fillRect(1, 1, w-2, h-2, Qt::white);

    if (object!=nullptr)
    {

        QPixmap pm;
        QString icn_file = (":/Icons/"+object->getTypeName()+"/icon").c_str();

        if ( ! QPixmapCache::find( icn_file, pm) )
        {
            pm.load( icn_file );
            QPixmapCache::insert( icn_file, pm);
        }

        tp.drawPixmap( ((w-pm.width())/2), (h/2)-(2+pm.height()) , pm );

        QString t=QString::fromUtf8(object->getName().c_str());

        int t_x=2;
        int t_y=2+h/2;
        int t_w=w-4;
        int t_h=h/2-4;

        tp.drawText( t_x, t_y , t_w, t_h ,
                     Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap, t );
    }
    else
    {
        QString t = helperText ; 

        int t_x = 2;
        int t_y = 2;
        int t_w = w-4;
        int t_h = h-4;

        tp.drawText( t_x, t_y , t_w, t_h ,
                     Qt::AlignHCenter|Qt::AlignVCenter|Qt::TextWordWrap, t );


    }
    tp.end();
    p.drawPixmap( 0, 0, bufferpixmap );

}
void FWObjectDropArea::insertObject(libfwbuilder::FWObject *o)
{
    if (object!=o)
    {
        object=o;
        update();
        emit objectInserted();
    }
}

void FWObjectDropArea::deleteObject()
{
    object=nullptr;
    update();
    emit objectDeleted();
}

void FWObjectDropArea::contextMenuEvent (QContextMenuEvent * e)
{
    QMenu *popup;

    popup=new QMenu(this);

    QAction *sitAct = popup->addAction( tr("Show in the tree") ,    this , SLOT( showInTreeObject( )) );
    QAction *editAct = popup->addAction( tr("Edit") ,    this , SLOT( editObject( )) );
    popup->addSeparator();

    QAction *psAct = popup->addAction( tr("Paste") ,    this , SLOT( pasteObject( )) );
    popup->addSeparator();
    QAction *dlAct = popup->addAction( tr("Delete") ,   this , SLOT( deleteObject( )) );

    sitAct->setEnabled(object!=nullptr);
    editAct->setEnabled(object!=nullptr);
    dlAct->setEnabled(object!=nullptr);
    psAct->setEnabled(FWObjectClipboard::obj_clipboard->size()>0);

    popup->exec(e->globalPos ());
    delete popup;
}

void FWObjectDropArea::dropEvent( QDropEvent *ev)
{
    if (fwbdebug)
    {
        qDebug("FWObjectDropArea::dropEvent  drop event mode=%d", ev->dropAction());
        qDebug("                        src widget = %p", ev->source());
    }

    list<FWObject*> dragol;
    if (FWObjectDrag::decode(ev, dragol))
    {
        if (dragol.size()>0)
        {
            insertObject(dragol.front());
        }
    }
}

void FWObjectDropArea::dragEnterEvent( QDragEnterEvent *ev)
{
    list<FWObject*> dragol;
    if (FWObjectDrag::decode(ev, dragol))
    {
        if (dragol.size()>0)
        {
            FWObject * o = dragol.front();
            bool ok = false ;
            if (acceptedTypes.size()==0)
                ok = true ;
            for (int p = 0 ; p < acceptedTypes.size(); p++)
            {
                QString type =o->getTypeName().c_str(); 
                if (type==acceptedTypes[p])
                {
                    ok = true ;
                    break ;
                }
            }
            if (!ok)
            {
                ev->setAccepted(false);
                return ;
            }
        }
    }

    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void FWObjectDropArea::pasteObject()
{
    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for( i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        FWObject *co= i->second->db()->findInIndex(i->first);
        insertObject(co);
    }

}

void FWObjectDropArea::showInTreeObject()
{
    ProjectPanel * pp = mw->activeProject();
    if (pp!=nullptr)
    {
        QCoreApplication::postEvent(
            pp, new showObjectInTreeEvent(pp->getFileName(), object->getId()));
    }
}

void FWObjectDropArea::editObject()
{
    ProjectPanel * pp = mw->activeProject();
    if (pp!=nullptr)
    {
        if (RuleSet::cast(object)!=nullptr)
            QCoreApplication::postEvent(
                pp, new openRulesetEvent(pp->getFileName(), object->getId()));

        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(pp->getFileName(), object->getId()));
        QCoreApplication::postEvent(
            pp, new showObjectInTreeEvent(pp->getFileName(), object->getId()));
    }
}

void FWObjectDropArea::mouseDoubleClickEvent(QMouseEvent *)
{
    if (object!=nullptr) editObject();
}
