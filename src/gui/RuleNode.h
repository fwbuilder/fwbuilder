#ifndef RULENODE_H
#define RULENODE_H

#include <QObject>
#include <QSize>

#define MAX_COLUMNS 10

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
