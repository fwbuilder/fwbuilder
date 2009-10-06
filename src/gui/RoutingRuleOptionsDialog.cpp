/*

                 Copyright (C) 2005 Compal GmbH, Germany

  Author:  Roman Hoog Antink  <fwbuilder-routing at compal.de>

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


#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "ProjectPanel.h"

#include "RoutingRuleOptionsDialog.h"
#include "RuleSetView.h"
#include "FWWindow.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Resources.h"

#include <qstackedwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include <iostream>
#include "FWBSettings.h"

using namespace libfwbuilder;
using namespace std;

RoutingRuleOptionsDialog::~RoutingRuleOptionsDialog()
{
    delete m_dialog;
}

RoutingRuleOptionsDialog::RoutingRuleOptionsDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::RoutingRuleOptionsDialog_q;
    m_dialog->setupUi(this);
}

void RoutingRuleOptionsDialog::getHelpName(QString *str)
{
    *str = "RoutingRuleOptionsDialog";
}

void RoutingRuleOptionsDialog::loadFWObject(FWObject *o)
{
    obj=o;
//    rsv=rv;

    FWObject *p=obj;
    while ( !Firewall::cast(p) ) p=p->getParent();
    platform=p->getStr("platform").c_str();

    Rule      *rule = dynamic_cast<Rule*>(o);
    FWOptions *ropt = rule->getOptionsObject();
    // m_dialog->editorTitle->setText(QString("%1 / %2 / %3 ")
    //         .arg(QString::fromUtf8(p->getName().c_str()))
    //         .arg(rule->getTypeName().c_str())
    //         .arg(rule->getPosition()));

    int wid=0;
    if (platform=="iptables") wid=0;
    if (platform=="pix")      wid=1;
/*
    if (platform=="ipf")      wid=1;
    if (platform=="pf")       wid=2;
    if (platform=="ipfw")     wid=3;
*/

    m_dialog->wStack->setCurrentIndex( wid );
    m_dialog->wStack->widget(wid)->raise();

    data.clear();

    if (platform=="iptables")
    {
        data.registerOption( m_dialog->routing_non_critical_rule, ropt,  "no_fail" );
        //data.registerOption( ipt_stateless            , ropt,  "stateless" );
    }

    init=true;
    data.loadAll();

    //apply->setEnabled( false );
    init=false;
}

void RoutingRuleOptionsDialog::validate(bool *res)
{
    *res=true;
}



void RoutingRuleOptionsDialog::applyChanges()
{
    if (!isTreeReadWrite(this,obj)) return;

    init=true;
    data.saveAll();
    init=false;

    BaseObjectDialog::applyChanges();
}

void RoutingRuleOptionsDialog::discardChanges()
{
    loadFWObject(obj);
}

