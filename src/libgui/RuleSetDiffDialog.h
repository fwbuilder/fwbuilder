/*

                          Firewall Builder

                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#ifndef RULESETDIFFDIALOG_H
#define RULESETDIFFDIALOG_H

#include <ui_rulesetdiffdialog_q.h>

#include <QDialog>
#include <QHash>

namespace libfwbuilder {
class FWObject;
class FWObjectDatabase;
class Library;
}

class ProjectPanel;
class RuleSetModel;

namespace Ui {
class RuleSetDiffDialog_q;
}

class RuleSetDiffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RuleSetDiffDialog(ProjectPanel *project, QWidget *parent = 0);
    ~RuleSetDiffDialog();

public slots:
    void updateFirewallList();
    void updateRuleSetList();
    void updateRuleSetView();
    void horizontalScrollBarValueChanged(int val);

    void itemClicked(const QModelIndex &index);
    void itemDoubleClicked(const QModelIndex &index);
    void selectItem(const QModelIndex &index, bool sourceIsCurrentRuleSetView, bool doubleClick = false);

    void itemExpanded(const QModelIndex &index);
    void itemCollapsed(const QModelIndex &index);
    void expandCollapseItem(const QModelIndex &index, bool expand = false);

    void displayUnmodifiedRules();

private:
    Ui::RuleSetDiffDialog_q *ui;
    ProjectPanel *m_project;
    libfwbuilder::FWObjectDatabase *m_currentObjdb;
    libfwbuilder::FWObjectDatabase *m_originalObjdb;
    libfwbuilder::Library *m_currentUserLibrary;
    libfwbuilder::Library *m_originalUserLibrary;
    libfwbuilder::Library* findUserLibrary(libfwbuilder::FWObjectDatabase *db);
    void setLabelColor(QLabel *label, const QString& color);
};

#endif // RULESETDIFFDIALOG_H
