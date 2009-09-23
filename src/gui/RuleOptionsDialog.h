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

#include "../../config.h"
#include <ui_ruleoptionsdialog_q.h>
#include <QWidget>

#include "DialogData.h"

#include "fwbuilder/FWObject.h"

class RuleSetView;
class ProjectPanel;

class RuleOptionsDialog : public QWidget
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;
    DialogData              data;
    QString                 platform;
    RuleSetView            *rsv;
    Ui::RuleOptionsDialog_q*m_dialog;
    
    bool init;
    QString help_name;
    ProjectPanel *m_project;

public:
    RuleOptionsDialog(ProjectPanel *project, QWidget *parent);
    ~RuleOptionsDialog();
    
public slots:
    virtual void changed();
    virtual void libChanged();
    virtual void applyChanges();
    virtual void cancelChanges();
    virtual void discardChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void isChanged(bool*);
    virtual void closeEvent(QCloseEvent *e);
    virtual void getHelpName(QString*);

 signals:
/**
 * This signal is emitted from closeEvent, ObjectEditor connects
 * to this signal to make checks before the object editor can be closed
 * and to store its position on the screen
 */
    void close_sign(QCloseEvent *e);
    void changed_sign();
    void notify_changes_applied_sign();

};

#endif // __RULEOPTIONSDIALOG_H
