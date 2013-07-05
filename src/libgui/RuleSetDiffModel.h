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

#ifndef RULESETDIFFMODEL_H
#define RULESETDIFFMODEL_H

#include <QSortFilterProxyModel>
#include "ProjectPanel.h"
#include "RuleSetModel.h"

class RuleSetDiffModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RuleSetDiffModel(QObject *parent, RuleSetModel *model, ProjectPanel *project);
    void displayUnmodifiedRules(bool val);
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
private:
    ProjectPanel* m_project;
    bool m_displayUnmodifiedRules;
signals:
    
public slots:
    
};

#endif // RULESETDIFFMODEL_H
