/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#ifndef  __OBJECTEDITORDOCKWIDGET_H_
#define  __OBJECTEDITORDOCKWIDGET_H_


#include <QDockWidget>
#include <QCloseEvent>

class ObjectEditor;

class ObjectEditorDockWidget : public QDockWidget {

    Q_OBJECT ;

    ObjectEditor *editor;

public:

    ObjectEditorDockWidget(const QString &title,
                           QWidget *parent = 0,
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
                           Qt::WindowFlags flags = Qt::WindowFlags());
#else
                           Qt::WindowFlags flags = 0);
#endif

    ObjectEditorDockWidget(QWidget *parent = 0,
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
                          Qt::WindowFlags flags = Qt::WindowFlags());
#else
                          Qt::WindowFlags flags = 0);
#endif

    void setupEditor(ObjectEditor *ed);
    
    virtual void closeEvent(QCloseEvent *event);

public slots:

    void topLevelChanged(bool topLevel);
    
};

#endif
