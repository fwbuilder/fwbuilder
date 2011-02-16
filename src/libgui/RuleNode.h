/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#ifndef RULENODE_H
#define RULENODE_H

#include <QObject>
#include <QSize>

#define MAX_COLUMNS 20

namespace libfwbuilder {
    class Rule;
}

class RuleNode : public QObject
{
public:
    enum Type { Root, Group, Rule };

    RuleNode(Type type, const QString &str = "");
    ~RuleNode();

    Type type;
    QString name; //TODO This field is unused in real application
    RuleNode *parent;
    QList<RuleNode *> children;
    libfwbuilder::Rule* rule;
    QSize sizes[MAX_COLUMNS];

    void prepend(RuleNode * node);
    void add(RuleNode* );
    void insert(int pos, RuleNode* node);

    bool isInGroup();
    bool isRoot();
    bool isOutermost();
    QString groupName();
    QString nameOfSuccessorGroup();
    QString nameOfPredecessorGroup();
    void resetSizes();
    void resetAllSizes();

    bool operator==(const RuleNode &rn) const;
};

#endif // RULENODE_H
