/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

  $Id: ActionsDialog.h,v 1.8 2006/09/13 15:46:46 vkurland Exp $

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

#include "config.h"
#include <ui_actionsdialog_q.h>
#include "DialogData.h"
#include <QWidget>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"

namespace libfwbuilder
{
    class PolicyRule;
    class Firewall;
}

class QLineEdit;
class QComboBox;

class ActionsDialog : public QWidget
{
    Q_OBJECT
 private:
    libfwbuilder::Firewall *firewall; 
    libfwbuilder::PolicyRule *rule; 
    std::string               editor;
    std::string               platform;
    QLineEdit                *branchNameInput;
    
    DialogData                data;
 public:
    Ui::ActionsDialog_q *m_dialog;
     
    ActionsDialog(QWidget *parent) : QWidget(parent)
    {
        m_dialog = new Ui::ActionsDialog_q;
        m_dialog->setupUi(this);
    };
    
    ~ActionsDialog() { delete m_dialog; };
    virtual void closeEvent(QCloseEvent *e);
    void fillInterfaces(QComboBox *);


public slots:
    virtual void changed();
    virtual void applyChanges();
    virtual void discardChanges();
    virtual void tagvalueChanged(int);
    virtual void iptRouteContinueToggled();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void isChanged(bool*);
    void setRule(libfwbuilder::PolicyRule*);
    
 signals:
/**
 * This signal is emitted from closeEvent, ObjectEditor connects
 * to this signal to make checks before the object editor can be closed
 * and to store its position on the screen
 */
    void close_sign(QCloseEvent *e);
    void changed_sign();
    
};

#endif 
