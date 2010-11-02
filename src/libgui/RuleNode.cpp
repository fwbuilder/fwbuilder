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

#include "RuleNode.h"

RuleNode::RuleNode(Type type, const QString &str )
{
    this->type = type;
    this->name = str;
    parent = 0;

}

RuleNode::~RuleNode()
{
    qDeleteAll(children);
}

void RuleNode::add(RuleNode * node)
{
    children << node;
    node->parent = this;
}

void RuleNode::prepend(RuleNode * node)
{
    children.prepend(node);
    node->parent = this;
}

void RuleNode::insert(int pos, RuleNode* node)
{
    children.insert(pos, node);
    node->parent = this;
}

bool RuleNode::isInGroup()
{
    if (parent == 0) return false;
    return !parent->isRoot();
}

bool RuleNode::isRoot()
{
    return type == RuleNode::Root;
}

QString RuleNode::groupName()
{
    if (isInGroup())
        return parent->name;
    return QString();
}

QString RuleNode::nameOfSuccessorGroup()
{
    if (parent == 0 || isInGroup()) return QString();
    int idx = parent->children.indexOf(this);
    if (idx < (parent->children.size() - 1) && parent->children[idx + 1]->type == Group)
    {
        return parent->children[idx + 1]->name;
    }
    return QString();
}

QString RuleNode::nameOfPredecessorGroup()
{
    if (parent == 0 || isInGroup()) return QString();
    int idx = parent->children.indexOf(this);
    if (idx > 0 && parent->children[idx - 1]->type == Group)
    {
        return parent->children[idx - 1]->name;
    }
    return QString();
}

bool RuleNode::isOutermost()
{
    if (type != RuleNode::Rule) return false;
    return this == this->parent->children.first() || this == this->parent->children.last();
}

bool RuleNode::operator==(const RuleNode &rn) const
{
    if (rn.type != this->type) return false;
    if (this->type == Group)
    {
        return this->name == rn.name;
    }
    else
    {
        return this->rule == rn.rule;
    }
}

void RuleNode::resetSizes()
{
    for(int i=0;i<MAX_COLUMNS;i++)
    {
        sizes[i]=QSize();
    }
}

void RuleNode::resetAllSizes()
{
    resetSizes();
    foreach(RuleNode* node, children)
    {
        node->resetAllSizes();
    }
}
