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


#ifndef __NEWGROUPDIALOG_H_
#define __NEWGROUPDIALOG_H_

#include <ui_newgroupdialog_q.h>
#include <QDialog>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"


class newGroupDialog : public QDialog
{
    Q_OBJECT;
            
    libfwbuilder::FWObjectDatabase *db;
 
 public:
    newGroupDialog(QWidget *parent, libfwbuilder::FWObjectDatabase *_db);
    ~newGroupDialog();
    
    Ui::newGroupDialog_q *m_dialog;

public slots:
    virtual void accept();
    
};

#endif // __NEWGROUPDIALOG_H
