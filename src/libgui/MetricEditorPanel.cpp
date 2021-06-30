/*

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/



#include "global.h"

#include "MetricEditorPanel.h"
#include "FWBSettings.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"

#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <fstream>

using namespace std;
using namespace libfwbuilder;

MetricEditorPanel::~MetricEditorPanel()
{
    delete m_widget;
}

MetricEditorPanel::MetricEditorPanel(QWidget *p) : BaseObjectDialog(p)
{
    m_widget = new Ui::MetricEditorPanel_q;
    m_widget->setupUi(this);

    //spin_box->setMinValue( minValue);
    //spin_box->setMaxValue( maxValue);
    //spin_box->setValue( value);
}

int MetricEditorPanel::value()
{
    return m_widget->spin_box->value();
}

void MetricEditorPanel::changed()
{
    emit changed_sign();
}

void MetricEditorPanel::applyChanges()
{
    rule->setMetric( value() );
}

void MetricEditorPanel::loadFWObject(libfwbuilder::FWObject *obj)
{
    RoutingRule *r=RoutingRule::cast(obj);
    if (r==nullptr) return;
    rule=r;

    FWObject *o = r;
    while (o!=nullptr && Firewall::cast(o)==nullptr) o=o->getParent();
    assert(o!=nullptr);

    m_widget->spin_box->setMinimum( 0);
    m_widget->spin_box->setMaximum( 255);
    m_widget->spin_box->setValue( r->getMetric());
}

void MetricEditorPanel::validate(bool* b )
{
    *b=true;
}

void MetricEditorPanel::closeEvent(QCloseEvent *)
{
}

