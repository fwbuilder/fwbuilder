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

#include "RuleSetDiffModel.h"
#include "RuleNode.h"
#include "fwbuilder/Rule.h"

using namespace libfwbuilder;

RuleSetDiffModel::RuleSetDiffModel(QObject *parent, RuleSetModel *model, ProjectPanel *project) :
    QSortFilterProxyModel(parent), m_project(project), m_displayUnmodifiedRules(false)
{
    setSourceModel(model);
}

void RuleSetDiffModel::displayUnmodifiedRules(bool val)
{
    beginResetModel();
    m_displayUnmodifiedRules = val;
    endResetModel();
}

bool RuleSetDiffModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_displayUnmodifiedRules)
        return true;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (index.isValid()) {

        RuleNode *node = static_cast<RuleNode *>(index.internalPointer());
        if (!node) return false;

        if (RuleNode::Rule != node->type) return true;

        if (m_project->getDiffType(node->rule->getId()) != DiffType::None)
            return true;
    }
    return false;
}
