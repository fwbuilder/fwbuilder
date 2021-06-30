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


#ifndef __FINDDIALOG_H_
#define __FINDDIALOG_H_

#include <ui_finddialog_q.h>

#include "fwbuilder/FWObject.h"
class ProjectPanel;

class findDialog : public QDialog
{
    Q_OBJECT;

    QString                   lastTextSearch;
    QString                   lastAttrSearch;
    libfwbuilder::FWObject   *lastFound;
    libfwbuilder::FWObject::tree_iterator   treeSeeker;

    bool matchName(const QString &name);
    bool matchAttr(libfwbuilder::FWObject* obj);
    ProjectPanel *m_project;

public:
    Ui::findDialog_q *m_dialog;
    findDialog(QWidget *p, ProjectPanel *project);

    void setObject(libfwbuilder::FWObject *o); 
    ~findDialog() { delete m_dialog; };

public slots:
    virtual void find();
    virtual void findNext();
    virtual void reset();
    virtual void findTextChanged(const QString&);
    virtual void findAttrChanged(const QString&);

    void makeActive();
    
 protected:

    virtual void showEvent( QShowEvent *ev);
    virtual void hideEvent( QHideEvent *ev);
    
};

#endif // __FINDDIALOG_H
