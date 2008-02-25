/* 

                 Copyright (C) 2005 Compal GmbH, Germany

  Author:  Roman Hoog Antink  <fwbuilder-routing at compal.de>
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions: 

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software. 

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 

*/


#ifndef __ROUTINGRULEOPTIONSDIALOG_H_
#define __ROUTINGRULEOPTIONSDIALOG_H_

#include "../../config.h"
#include <ui_routingruleoptionsdialog_q.h>
#include <QWidget>

#include "DialogData.h"

#include "fwbuilder/FWObject.h"

class RuleSetView;
class ProjectPanel;

class RoutingRuleOptionsDialog : public QWidget
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;
    DialogData              data;
    QString                 platform;
    RuleSetView            *rsv;
    Ui::RoutingRuleOptionsDialog_q *m_dialog;
    
    bool init;
    ProjectPanel *m_project;

 public:
    RoutingRuleOptionsDialog(ProjectPanel *project, QWidget *parent);
    ~RoutingRuleOptionsDialog();
    
public slots:
    virtual void changed();
    virtual void libChanged();
    virtual void applyChanges();
    virtual void discardChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void isChanged(bool*);
    virtual void closeEvent(QCloseEvent *e);

 signals:
/**
 * This signal is emitted from closeEvent, ObjectEditor connects
 * to this signal to make checks before the object editor can be closed
 * and to store its position on the screen
 */
    void close_sign(QCloseEvent *e);
    void changed_sign();
    
};

#endif // __ROUTINGRULEOPTIONSDIALOG_H
