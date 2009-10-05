/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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


#ifndef __FINDWHEREUSEDWIDGET_H_
#define __FINDWHEREUSEDWIDGET_H_

#include "../../config.h"
#include <ui_findwhereusedwidget_q.h>
#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"

#include <qvector.h>
#include <set>

class QRegExp;
class ObjectDescriptor;
class QWidget;

namespace libfwbuilder {
    class FWReference;
}

class FindWhereUsedWidget : public QWidget
{
    Q_OBJECT

private:
    ProjectPanel *project_panel;
    bool flShowObject;
    libfwbuilder::FWObject* object;
    std::set<libfwbuilder::FWObject *> resset;
    Ui::findWhereUsedWidget_q *m_widget;
    
    void showObject(libfwbuilder::FWObject*);
    void _find(libfwbuilder::FWObject *obj);

public:
    FindWhereUsedWidget(QWidget*p, ProjectPanel* pp, const char * n = 0,
                        Qt::WindowFlags f = 0, bool f_mini=false);
    ~FindWhereUsedWidget();

    void setShowObject(bool fl);
    void attachToProjectWindow(ProjectPanel *pp) { project_panel = pp; }

    /**
     * Post-process set of FWObject* returned by
     * FWObjectDatabase::findWhereObjectIsUsed to make it more
     * suitable for the user. Since findWhereObjectIsUsed returns
     * actual reference objects that point at the object we search
     * for, humanizeSearchResults replaces them with appropriate
     * parent objects. These can be either groups that hold
     * references, or rules, which are two levels up.
     */
    static void humanizeSearchResults(std::set<libfwbuilder::FWObject *>&);
    static QTreeWidgetItem* createQTWidgetItem(libfwbuilder::FWObject* obj,
                                               libfwbuilder::FWObject* container);
    
public slots:
    virtual void find();
    virtual void find(libfwbuilder::FWObject *obj);
    void init();
    void itemActivated(QTreeWidgetItem*);
    void findFromDrop();
    
 signals:
    void close();
};

#endif 
