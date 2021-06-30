/* 

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>
  
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

#ifndef __METRICEDITORPANEL_H__
#define __METRICEDITORPANEL_H__

#include "BaseObjectDialog.h"
#include <ui_metriceditorpanel_q.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Firewall.h"

class MetricEditorPanel : public BaseObjectDialog
{
    Q_OBJECT;

    libfwbuilder::RoutingRule *rule;
    Ui::MetricEditorPanel_q   *m_widget;

 public:

    MetricEditorPanel(QWidget* p);
    ~MetricEditorPanel();

    int value();

public slots:

    virtual void changed();
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void closeEvent(QCloseEvent *e);
};

#endif
