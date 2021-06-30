/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com

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


#ifndef __RULESETDIALOG_H_
#define __RULESETDIALOG_H_

#include <ui_rulesetdialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWOptions.h"

class ProjectPanel;

class RuleSetDialog : public BaseObjectDialog
{
    Q_OBJECT;

    std::string platform;
    libfwbuilder::FWOptions *fwopt;
    Ui::RuleSetDialog_q *m_dialog;

public:
    RuleSetDialog(QWidget *parent);
    ~RuleSetDialog();
    
public slots:
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
};

#endif // RULESETDIALOG_H
