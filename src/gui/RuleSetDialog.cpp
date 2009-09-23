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

#include "../../config.h"
#include "global.h"
#include "utils.h"

#include "FWBTree.h"
#include "RuleSetDialog.h"
#include "ProjectPanel.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Policy.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include "FWBSettings.h"

#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

RuleSetDialog::RuleSetDialog(ProjectPanel *project,
                             QWidget *parent) : QWidget(parent),
                                                m_project(project)
{
    m_dialog = new Ui::RuleSetDialog_q;
    m_dialog->setupUi(this);
    obj = NULL;
    platform = "";
}

RuleSetDialog::~RuleSetDialog()
{
    delete m_dialog;
}

void RuleSetDialog::getHelpName(QString *str)
{
    *str = QString(obj->getTypeName().c_str());
}

void RuleSetDialog::loadFWObject(FWObject *o)
{
    obj = o;
    RuleSet *s = dynamic_cast<RuleSet*>(obj);
    assert(s!=NULL);

    init = true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );
    // ipv4_6_rule_set QComboBox:
    // 0 - ipv4
    // 1 - ipv6
    // 2 - dual
    int idx = 0;
    if (s->isV4()) idx = 0;
    if (s->isV6()) idx = 1;
    if (s->isDual()) idx = 2;
    m_dialog->ipv4_6_rule_set->setCurrentIndex(idx);

    m_dialog->top_rule_set->setChecked(s->isTop());

    FWObject *fw = o;
    while (fw && (!Firewall::isA(fw) && !Cluster::isA(fw))) fw = fw->getParent();
    assert(fw!=NULL);
    platform = fw->getStr("platform");
    fwopt = Firewall::cast(fw)->getOptionsObject();

    if (platform == "iptables")
    {
        m_dialog->top_rule_set->setToolTip(
            QApplication::translate("RuleSetDialog_q",
                                    "On iptables \"top\" rule set goes into \n"
                                    "the built-in chains INPUT, OUTPUT,\n"
                                    "FORWARD; if this flag is unchecked,\n"
                                    "rules go into user-defined chain \n"
                                    "with the name the same as the name of \n"
                                    "the rule set.",
                                    0, QApplication::UnicodeUTF8));

        if (Policy::isA(obj))
        {
            // if this attribute is absent, we consider it False, so for
            // backwards compatibility the rule set is considered
            // filter+mangle rather than mangle only.
            m_dialog->iptables_only->show();
            QStringList mangle_rulesets = 
                QString(fwopt->getStr("ipt_mangle_only_rulesets").c_str()).
                split(" ");
            bool f = (mangle_rulesets.indexOf(s->getName().c_str()) >= 0);
            m_dialog->ipt_filter_table->setChecked(!f);
            m_dialog->ipt_mangle_table->setChecked(f);
        } else
            m_dialog->iptables_only->hide();

    } else
    {
        m_dialog->iptables_only->hide();
    }


    if (platform == "pf")
        m_dialog->top_rule_set->setToolTip(
            QApplication::translate("RuleSetDialog_q",
                                    "If this flag is unchecked, rules go \n"
                                    "into anchor with the name the same as\n"
                                    "the name of the rule set.",
                                    0, QApplication::UnicodeUTF8));

    if (platform == "iosacl" || platform == "pix" || platform=="fwsm")
        m_dialog->top_rule_set->setToolTip(
            QApplication::translate("RuleSetDialog_q",
                                    "If this flag is unchecked, generated\n"
                                    "access list will not be assigned to\n"
                                    "interfaces with \"ip access-group\"\n"
                                    "command. The name of the rule set will\n"
                                    "be used as a prefix for names of\n"
                                    "access access lists generated for it.",
                                    0, QApplication::UnicodeUTF8));

    if (platform == "ipf" || platform == "ipfw")
        m_dialog->top_rule_set->hide();
    else
        m_dialog->top_rule_set->show();

    init=false;
}

void RuleSetDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void RuleSetDialog::validate(bool *res)
{
    *res = true;
    if (!isTreeReadWrite(this, obj)) { *res = false; return; }
    if (!validateName(this, obj, m_dialog->obj_name->text())) { *res = false; return; }

    // Do not allow ':' in the rule set names because this character is
    // used as a separator in error and warning messages 
    QString pattern("([a-zA-Z0-9_-+=@%^]+)");

    // branch (anchor) names for PF may end with "/*"
    if (platform == "pf")
        pattern = "([a-zA-Z0-9_-+=@%^]+)(/\\*)?";

    QRegExp rx(pattern);

    if (!rx.exactMatch(m_dialog->obj_name->text()))
    {
        *res = false ;
        QMessageBox::critical(
            this,
            "Firewall Builder",
            tr("Rule set name '%1' is invalid. Only '[a-z][A-Z][0-9]_-+=@%^' characters are allowed.").arg( m_dialog->obj_name->text() ),
            tr("&Continue"), 0, 0,
            0 );

        return ;
    }
}

void RuleSetDialog::isChanged(bool *)
{
}

void RuleSetDialog::libChanged()
{
    changed();
}

void RuleSetDialog::applyChanges()
{
    RuleSet *s = dynamic_cast<RuleSet*>(obj);
    assert(s!=NULL);

    string oldname = obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment(
        string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    switch (m_dialog->ipv4_6_rule_set->currentIndex())
    {
    case 1: s->setV6(); break;
    case 2: s->setDual(); break;
    default: s->setV4(); break;
    }

    s->setTop(m_dialog->top_rule_set->isChecked());

    QStringList mangle_rulesets = 
        QString(fwopt->getStr("ipt_mangle_only_rulesets").c_str()).
        split(" ");

    if (platform == "iptables")
    {
        if (m_dialog->ipt_mangle_table->isChecked() &&
            mangle_rulesets.indexOf(s->getName().c_str()) < 0)
            mangle_rulesets.push_back(s->getName().c_str());

        if (!m_dialog->ipt_mangle_table->isChecked() &&
            mangle_rulesets.indexOf(s->getName().c_str()) >= 0)
            mangle_rulesets.removeAll(s->getName().c_str());
    }

    fwopt->setStr("ipt_mangle_only_rulesets",
                  mangle_rulesets.join(" ").toAscii().constData());

    m_project->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    init=false;

    emit notify_changes_applied_sign();

}

void RuleSetDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void RuleSetDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

