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

#ifndef RULESETMODEL_H
#define RULESETMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QVariant>
#include <QStringList>
#include <QHash>
#include "RuleNode.h"
#include "ColDesc.h"

namespace libfwbuilder
{
    class Firewall;
    class RuleSet;
    class RuleElement;
    class Rule;
    class FWObject;
}

class RuleSetModel;

////////////////////////////////////////////////////////////////////////
// RuleSetModelIterator
////////////////////////////////////////////////////////////////////////

class RuleSetModelIterator
{
    friend class RuleSetModel;
public:
    RuleSetModelIterator();


    bool isValid();

    bool hasNext();
    bool hasPrev();

    RuleSetModelIterator& operator= (const RuleSetModelIterator&);
    RuleSetModelIterator& operator++ ();
    RuleSetModelIterator& operator-- ();

    bool operator== ( RuleSetModelIterator& );
    bool operator!= ( RuleSetModelIterator& );

    QModelIndex index();

private:
    QAbstractItemModel *model;
    int row;
    QModelIndex parent;
};

////////////////////////////////////////////////////////////////////////
// ActionDesc
////////////////////////////////////////////////////////////////////////

class ActionDesc
{
    public:
    QString name;
    QString displayName;
    QString tooltip;
    QString argument;
};

Q_DECLARE_METATYPE(ActionDesc)

////////////////////////////////////////////////////////////////////////
// RuleSetModel
////////////////////////////////////////////////////////////////////////

class RuleSetModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    QList<ColDesc> header;

    RuleSetModel(libfwbuilder::RuleSet* ruleset, QObject *parent = 0);
    ~RuleSetModel() {delete root;}
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex index(int row, int column, QString groupName) const;
    QModelIndex index(libfwbuilder::Rule *rule, int col=0) const;
    QModelIndex index(libfwbuilder::Rule *rule, libfwbuilder::RuleElement *re) const;
    QModelIndex index(QString groupName) const;
    QModelIndex indexForPosition(int position) const;    
    QModelIndex parent(const QModelIndex &child) const;

    bool isEmpty();
    bool isGroup(const QModelIndex &index) const;

    libfwbuilder::RuleSet* getRuleSet() {return ruleset;}
    libfwbuilder::Firewall* getFirewall() const;

    libfwbuilder::Rule* insertNewRule();
    libfwbuilder::Rule* insertNewRule(QModelIndex &index, bool isAfter = false);

    libfwbuilder::Rule* insertRule(libfwbuilder::Rule *rule, QModelIndex &index, bool isAfter = false);
    void insertRule(libfwbuilder::Rule *rule);

    virtual void initRule(libfwbuilder::Rule *new_rule, libfwbuilder::Rule *old_rule = nullptr) = 0;

    void removeRow(int row,const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

    void renameGroup(QModelIndex group, const QString &newName);
    void removeFromGroup(QModelIndex group, int first, int last);
    QModelIndex createNewGroup(QString groupName, int first, int last);
    QString addToGroupAbove(int first, int last);
    QString addToGroupBelow(int first, int last);

    void moveRuleUp(const QModelIndex &group, int first, int last);
    void moveRuleDown(const QModelIndex &group, int first, int last);

    void changeRuleColor(const QList<QModelIndex> &indexes, const QString &c);
    void changeGroupColor(const QModelIndex index, const QString &c);

    bool isIndexRule(const QModelIndex index);
    RuleNode *nodeFromIndex(const QModelIndex &index) const;

    int getRulePosition(QModelIndex index);
    libfwbuilder::Rule * findRuleForPosition(int position) const;
    libfwbuilder::Rule * getRule(QModelIndex index) const;

    void setEnabled(const QModelIndex &index, bool flag);

    virtual bool checkRuleType(libfwbuilder::Rule *rule) = 0;
    void deleteObject(QModelIndex &index, libfwbuilder::FWObject* obj);
    bool insertObject(QModelIndex &index, libfwbuilder::FWObject *obj);

    int columnByType(ColDesc::ColumnType type);
    void rowChanged(const QModelIndex &index);
    void groupChanged(const QModelIndex &index);

    void getGroups(QList<QModelIndex> &list);

    RuleSetModelIterator begin();
    RuleSetModelIterator end();

    void resetAllSizes();
    QString findUniqueNameForGroup(const QString &groupName);
    void restoreRules(QList<libfwbuilder::Rule*> rules, bool topLevel = true);
    void restoreRule(libfwbuilder::Rule* rule);
    void objectChanged(libfwbuilder::FWObject* object);

protected:
    libfwbuilder::RuleElement *getRuleElementByRole(libfwbuilder::Rule* r, std::string roleName) const;

    void insertRuleToModel(libfwbuilder::Rule *rule, QModelIndex &index, bool isAfter = false);
    int columnForRuleElementType(QString) const;

    QString getPositionAsString(RuleNode *node) const;
    ActionDesc getRuleActionDesc(libfwbuilder::Rule* r) const;

    void copyRuleWithoutId(libfwbuilder::Rule* fromRule, libfwbuilder::Rule* toRule);

private:
    libfwbuilder::RuleSet *ruleset;
    RuleNode *root;
//    QHash<int,libfwbuilder::Rule*> rulesByPosition;

    void initModel();

    QVariant getDecoration(const QModelIndex &index) const;
    QVariant getDataForDisplayRole(const QModelIndex &index) const;
    QVariant getGroupDataForDisplayRole(const QModelIndex &index, RuleNode* node) const;

    virtual QVariant getRuleDataForDisplayRole(const QModelIndex &index, RuleNode* node) const = 0;
    QVariant getColumnDesc(const QModelIndex &index) const;


    void moveToGroup(RuleNode *targetGroup, int first, int last, bool append=true);

    void removeToList(QList<RuleNode*> &list, const QModelIndex &group, int first, int last);
    void insertFromList(const QList<RuleNode*> &list, const QModelIndex &parent, int position);

    QModelIndexList findObject (libfwbuilder::FWObject* object);

};

////////////////////////////////////////////////////////////////////////
// PolicyModel
////////////////////////////////////////////////////////////////////////

class PolicyModel : public RuleSetModel
{
public:
    PolicyModel(libfwbuilder::RuleSet* ruleset, QObject *parent = 0) : RuleSetModel(ruleset, parent) {configure();}

    void initRule(libfwbuilder::Rule *new_rule, libfwbuilder::Rule *old_rule = nullptr);
    bool checkRuleType(libfwbuilder::Rule *rule);

private:

    bool supports_time;
    bool supports_logging;
    bool supports_rule_options;

    QVariant getRuleDataForDisplayRole(const QModelIndex &index, RuleNode* node) const;
    QString getRuleDirection(libfwbuilder::Rule* r) const;
    QStringList getRuleOptions(libfwbuilder::Rule* r) const;
    void configure();
};

////////////////////////////////////////////////////////////////////////
// NatModel
////////////////////////////////////////////////////////////////////////

class NatModel : public RuleSetModel
{
public:
    NatModel(libfwbuilder::RuleSet* ruleset, QObject *parent = 0) : RuleSetModel(ruleset, parent) {configure();}

    void initRule(libfwbuilder::Rule *new_rule, libfwbuilder::Rule *old_rule = nullptr);
    bool checkRuleType(libfwbuilder::Rule *rule);

private:
    bool supports_actions;
    bool supports_inbound_interface;
    bool supports_outbound_interface;
    
    QVariant getRuleDataForDisplayRole(const QModelIndex &index, RuleNode* node) const;
    QStringList getRuleOptions(libfwbuilder::Rule* r) const;
    void configure();
};

////////////////////////////////////////////////////////////////////////
// RoutingModel
////////////////////////////////////////////////////////////////////////

class RoutingModel : public RuleSetModel
{
public:
    RoutingModel(libfwbuilder::RuleSet* ruleset, QObject *parent = 0) : RuleSetModel(ruleset, parent) {configure();}

    void initRule(libfwbuilder::Rule *new_rule, libfwbuilder::Rule *old_rule = nullptr);
    bool checkRuleType(libfwbuilder::Rule *rule);

private:
    bool supports_routing_itf;
    bool supports_metric;

    QVariant getRuleDataForDisplayRole(const QModelIndex &index, RuleNode* node) const;
    QStringList getRuleOptions(libfwbuilder::Rule* r) const;
    void configure();
};

#endif // RULESETMODEL_H
