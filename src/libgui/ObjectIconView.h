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


#ifndef  __OBJECTICONVIEW_H_
#define  __OBJECTICONVIEW_H_

#include <qlistwidget.h>
#include <qdrag.h>

namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
};

class ObjectIconView : public QListWidget {

    Q_OBJECT ;
            
    libfwbuilder::FWObjectDatabase *db;
    bool startingDrag;

protected:

    virtual QDrag* dragObject();
    virtual void dragEnterEvent( QDragEnterEvent *ev);
    virtual void dropEvent(QDropEvent *ev);
    virtual void dragMoveEvent( QDragMoveEvent *ev);
    virtual void keyPressEvent( QKeyEvent* ev );
    void mousePressEvent ( QMouseEvent * event );
    void mouseMoveEvent ( QMouseEvent * event );
    
    
    bool event ( QEvent * event );
    
public:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    ObjectIconView(QWidget* parent, const char * name = 0, Qt::WindowFlags f = Qt::WindowFlags());
#else
    ObjectIconView(QWidget* parent, const char * name = 0, Qt::WindowFlags f = 0);
#endif
    
    void setDB(libfwbuilder::FWObjectDatabase *_db) { db = _db; }

 signals:

    void delObject_sign();
    void dropped(QDropEvent *ev);
};


#endif

