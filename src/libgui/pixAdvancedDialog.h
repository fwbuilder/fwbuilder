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



#ifndef __PIXADVANCEDDIALOG_H_
#define __PIXADVANCEDDIALOG_H_

#include <ui_pixadvanceddialog_q.h>
#include "DialogData.h"

#include <string>

class QWidget;
class QSpinBox;
class QComboBox;
class QCheckBox;

namespace libfwbuilder {
    class FWObject;
    class FWOptions;
};

struct fixupControl {
    class QComboBox *switch_widget;
    class QSpinBox *arg1;
    class QSpinBox *arg2;
    class QCheckBox *arg3;
    QString fwoption;
    QString fixup_cmd;
    int page;     // number of the notebook page in fixup_notebook widget
    bool active;  // if false, then this fixup is not supported on the given
                  // version of PIX OS

    fixupControl(QComboBox *s,
                 QSpinBox *w1,
                 QSpinBox *w2,
                 QCheckBox *w3,
                 const QString &o,
                 const QString &f,
                 int p)
    {
        switch_widget=s;
        arg1=w1; arg2=w2; arg3=w3;
        fwoption=o; fixup_cmd=f; page=p;
        active=true;
    }
};




class pixAdvancedDialog : public QDialog
{
    Q_OBJECT

    libfwbuilder::FWObject          *obj;
    DialogData                       data;
    std::list<struct fixupControl>   allFixups;
    bool                             syslogDeviceIdSupported;
    
    Ui::pixAdvancedDialog_q         *m_dialog;

 public:
    pixAdvancedDialog(QWidget *parent, libfwbuilder::FWObject *o);
    ~pixAdvancedDialog();

    void setDefaultTimeoutValue(const QString &option);
    void updateFixupCommandsDisplay();
    void loadFixups();
    void saveFixups(libfwbuilder::FWOptions *options);
    
    int translateFixupSwitchFromOptionToWidget(int o);
    int translateFixupSwitchFromWidgetToOption(int o);
    void changeAllFixups(int state);
    
protected slots:

    virtual void accept();
    virtual void reject();

    virtual void editProlog();
    virtual void editEpilog();
    virtual void defaultTimeouts();
    virtual void regenerateFixups();

    virtual void fixupCmdChanged();
    virtual void enableAllFixups();
    virtual void disableAllFixups();
    virtual void skipAllFixups();
    virtual void scriptACLModeChanged();
    virtual void displayCommands();
    
};

#endif // __PIXADVANCEDDIALOG_H

