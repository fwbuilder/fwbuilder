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


#ifndef __FWOBJECTDROPAREA_H_
#define __FWOBJECTDROPAREA_H_

#include <ui_fwobjectdroparea_q.h>

#include "fwbuilder/FWObject.h"
#include <QPaintEvent>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QPixmap>

class QWidget;
class QPixmap;
class QMenu;
class QContextMenuEvent;
class QDropEvent;
class QDragEnterEvent;

class FWObjectDropArea : public QWidget//Ui::FWObjectDropArea_q
{
    Q_OBJECT;

private:
    libfwbuilder::FWObject *object;
    QVector <QString> acceptedTypes ;
    QString helperText ;

protected:
    void paintEvent(QPaintEvent *ev);
    void contextMenuEvent (QContextMenuEvent * e);
    void dropEvent( QDropEvent *ev);
    void dragEnterEvent( QDragEnterEvent *ev);
        
public:
    Ui::FWObjectDropArea_q *m_objectDropArea;
    FWObjectDropArea(QWidget*p, const char * n = 0, Qt::WindowFlags f = 0);
    ~FWObjectDropArea(); 
    libfwbuilder::FWObject * getObject(){return object;};
    void setObject(libfwbuilder::FWObject * o){ object = o ;};
    void addAcceptedTypes (QString type){acceptedTypes.push_back(type);};
    bool isEmpty() {return object==nullptr;};
    void setHelperText(const QString &text) { helperText=text; }
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    public slots:
    void insertObject(libfwbuilder::FWObject *o);
    void deleteObject();
    void pasteObject();
    void showInTreeObject();
    void editObject();
    
signals:
    void objectDeleted();
    void objectInserted();
    
};

#endif 
