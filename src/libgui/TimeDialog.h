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


#ifndef __TIMEDIALOG_H_
#define __TIMEDIALOG_H_

#include <ui_timedialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"

class ProjectPanel;

class TimeDialog : public BaseObjectDialog
{
    Q_OBJECT;

    Ui::TimeDialog_q *m_dialog;

    void enableAllWidgets();
    
 public:
    TimeDialog(QWidget *parent);
    ~TimeDialog();
    
public slots:
    virtual void applyChanges();
    virtual void useStartOrEndDate();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
};

#endif // __TIMEDIALOG_H
