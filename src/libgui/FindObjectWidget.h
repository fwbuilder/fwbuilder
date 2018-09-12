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


#ifndef __FINDOBJECTWIDGET_H_
#define __FINDOBJECTWIDGET_H_

#include <ui_findobjectwidget_q.h>
#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"

class QRegExp;
class ObjectDescriptor;
class QWidget;

class FindObjectWidget : public QWidget
{
    Q_OBJECT;

private:

    QString lastAttrSearch;
    libfwbuilder::FWObject *lastFound;
    libfwbuilder::FWObject::tree_iterator  treeSeeker;
    std::list<libfwbuilder::FWObject*> found_objects;
    std::list<libfwbuilder::FWObject*>::iterator found_objects_iter;
    libfwbuilder::Firewall* selectedFirewall;
    ProjectPanel *project_panel;

    void _findAll();
    void _replaceCurrent();
    
    bool matchName(const QString &name);
    bool matchID(int id);
    bool matchAttr(libfwbuilder::FWObject* obj);
    bool validateReplaceObject();
    bool inSelectedFirewall( libfwbuilder::RuleElement* r);

    virtual void keyPressEvent( QKeyEvent* ev );
    
 public:
    Ui::findObjectWidget_q *m_widget;
    FindObjectWidget(QWidget*p, ProjectPanel *pp, const char * n = 0, Qt::WindowFlags f = 0);
    ~FindObjectWidget() { delete m_widget; };
    void findObject (libfwbuilder::FWObject *o);
    void attachToProjectWindow(ProjectPanel *pp) { project_panel = pp; }

public slots:
    virtual void enableAll();
    virtual void disableAll();
    virtual void objectInserted();
    
    virtual void find();
    virtual void findNext();
    virtual void reset();
    virtual void findAttrChanged(const QString&);
    virtual void findPrev();
    virtual void replaceNext();
    
    virtual void replace();
    virtual void replaceAll();
    virtual void replaceEnable();
    virtual void replaceDisable();
    void showObject(libfwbuilder::FWObject* o);
    void init();
    void clear();
    void firewallOpened(libfwbuilder::Firewall *f);
    void scopeChanged();
    void objectDeleted();

 signals:
    void close();
};

#endif 
