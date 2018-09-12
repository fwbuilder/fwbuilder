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

#include "RuleSetDiffDelegate.h"

#include <string>

#include "global.h"
#include "utils.h"
#include "FWBSettings.h"

#include "fwbuilder/Rule.h"
#include "RuleSetDiffDialog.h"


using namespace libfwbuilder;
using namespace std;

RuleSetDiffDelegate::RuleSetDiffDelegate(QObject *parent,
                                                     FWObjectSelectionModel *selectionModel,
                                                     QAbstractProxyModel *model,
                                                     ProjectPanel *project)
    : RuleSetViewDelegate(parent, selectionModel), m_model(model), m_project(project)
{}

void RuleSetDiffDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    RuleSetViewDelegate::paint(painter, option, m_model->mapToSource(index));
}

QSize RuleSetDiffDelegate::sizeHint(const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    return RuleSetViewDelegate::sizeHint(option, m_model->mapToSource(index));
}

QString RuleSetDiffDelegate::getRuleColor(RuleNode *node) const
{
    int changeType = m_project->getDiffType(node->rule->getId());

    switch (changeType) {
    case DiffType::Add:
        return QString(st->getDiffColor(FWBSettings::ADD_COLOR));
    case DiffType::Edit:
        return QString(st->getDiffColor(FWBSettings::EDIT_COLOR));
    case DiffType::Move:
        return QString(st->getDiffColor(FWBSettings::MOVE_COLOR));
    case DiffType::Remove:
        return QString(st->getDiffColor(FWBSettings::REMOVE_COLOR));
    default:
        break;
    }
    return QString();
}
