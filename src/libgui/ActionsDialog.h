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


#ifndef __ACTIONSDIALOG_H_
#define __ACTIONSDIALOG_H_

#include <ui_actionsdialog_q.h>
#include "BaseObjectDialog.h"
#include "DialogData.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"

namespace libfwbuilder
{
    class Rule;
    class Firewall;
}

class QLineEdit;
class QComboBox;
class FWObjectDropArea;


class ActionsDialog : public BaseObjectDialog
{
    Q_OBJECT;

private:
    libfwbuilder::Firewall *firewall; 
    libfwbuilder::Rule *rule; 
    std::string editor;
    std::string platform;
    QLineEdit *branchNameInput;
    
    DialogData data;
    FWObjectDropArea * BranchChainArea ;
    FWObjectDropArea * BranchAnchorArea;
    FWObjectDropArea * TagIntArea ;
    FWObjectDropArea * TagStrArea ;

 public:
    Ui::ActionsDialog_q *m_dialog;
     
    ActionsDialog(QWidget *parent);
    ~ActionsDialog();


public slots:
    virtual void applyChanges();
    virtual void tagvalueChanged(int);
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    void setRule(libfwbuilder::Rule*);
};

#endif 
