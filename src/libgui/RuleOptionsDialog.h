/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __RULEOPTIONSDIALOG_H_
#define __RULEOPTIONSDIALOG_H_

#include <ui_ruleoptionsdialog_q.h>
#include "BaseObjectDialog.h"
#include <QWidget>

#include "DialogData.h"

#include "fwbuilder/FWObject.h"

class RuleSetView;
class ProjectPanel;

class RuleOptionsDialog : public BaseObjectDialog
{
    Q_OBJECT;

    DialogData              data;
    QString                 platform;
   
    RuleSetView            *rsv;
    Ui::RuleOptionsDialog_q*m_dialog;
    libfwbuilder::FWObject *firewall;
    
public:
    RuleOptionsDialog(QWidget *parent);
    ~RuleOptionsDialog();

    void fillInterfaces(QComboBox *); // #2367

public slots:
    virtual void changed();
    virtual void applyChanges();
    virtual void cancelChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void connlimitAboveLabelChange();
    virtual void limitLabelChange();
    virtual void iptRouteContinueToggled();  // #2367
};

#endif // __RULEOPTIONSDIALOG_H
