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

#ifndef RULESETDIFFDELEGATE_H
#define RULESETDIFFDELEGATE_H

#include "RuleSetViewDelegate.h"
#include "ProjectPanel.h"
#include <QAbstractProxyModel>

class RuleSetDiffDelegate : public RuleSetViewDelegate
{
    Q_OBJECT
public:
    RuleSetDiffDelegate(QObject *parent, FWObjectSelectionModel *selectionModel, QAbstractProxyModel *model, ProjectPanel *project);
    void paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint (const QStyleOptionViewItem &, const QModelIndex &) const;
private:
    QAbstractProxyModel *m_model;
    ProjectPanel *m_project;
    virtual QString getRuleColor(RuleNode *node) const;
};

#endif // RULESETDIFFDELEGATE_H
