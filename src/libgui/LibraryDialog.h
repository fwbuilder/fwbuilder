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


#ifndef __LIBRARYDIALOG_H_
#define __LIBRARYDIALOG_H_

#include "../../config.h"
#include <ui_librarydialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"


class ProjectPanel;
class LibraryDialog : public BaseObjectDialog
{
    Q_OBJECT;

    libfwbuilder::FWObject *obj;
    bool init;
    QString  color;
    
    void fillColor();
    void changeIds(libfwbuilder::FWObject *root);
    
 public:
    Ui::LibraryDialog_q *m_dialog;
     
    LibraryDialog(QWidget *parent);
    ~LibraryDialog();
    
public slots:
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void changeColor();
    virtual void getHelpName(QString*);
    

};

#endif // LIBRARYDIALOG_H