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
