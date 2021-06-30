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

#include "RuleSetDiffDialog.h"
#include "RuleSetDiffModel.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QSortFilterProxyModel>


#include "RuleSetView.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Library.h"
#include "RuleSetDiffDelegate.h"
#include "FWObjectSelectionModel.h"
#include "FWCmdRule.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleElement.h"
#include "RuleSetModel.h"
#include "FWBSettings.h"
#include "global.h"

using namespace libfwbuilder;
using namespace std;

static bool sortByName(const FWObject *a, const FWObject *b) { return a->getName() < b->getName(); }

RuleSetDiffDialog::RuleSetDiffDialog(ProjectPanel *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RuleSetDiffDialog_q),
    m_project(project)
{
    ui->setupUi(this);

    connect(ui->firewall, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRuleSetList()));
    connect(ui->ruleset, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRuleSetView()));
    connect(ui->originalRuleSetView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollBarValueChanged(int)));
    connect(ui->currentRuleSetView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollBarValueChanged(int)));

    connect(ui->originalRuleSetView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    connect(ui->currentRuleSetView, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    connect(ui->originalRuleSetView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
    connect(ui->currentRuleSetView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(itemDoubleClicked(QModelIndex)));
    connect(ui->originalRuleSetView, SIGNAL(expanded(QModelIndex)), this, SLOT(itemExpanded(QModelIndex)));
    connect(ui->currentRuleSetView, SIGNAL(expanded(QModelIndex)), this, SLOT(itemExpanded(QModelIndex)));
    connect(ui->originalRuleSetView, SIGNAL(collapsed(QModelIndex)), this, SLOT(itemCollapsed(QModelIndex)));
    connect(ui->currentRuleSetView, SIGNAL(collapsed(QModelIndex)), this, SLOT(itemCollapsed(QModelIndex)));

    ui->displayUnmodifiedRulesBtn->setChecked(st->getDisplayUnmodifiedRules());
    connect(ui->displayUnmodifiedRulesBtn, SIGNAL(clicked()), this, SLOT(displayUnmodifiedRules()));

    m_currentObjdb = m_project->db();
    if (m_currentObjdb)
        m_currentUserLibrary = findUserLibrary(m_currentObjdb);

    m_originalObjdb = m_project->origDb();
    if (m_originalObjdb)
        m_originalUserLibrary = findUserLibrary(m_originalObjdb);

    updateFirewallList();

    showMaximized();
}

Library* RuleSetDiffDialog::findUserLibrary(FWObjectDatabase *db)
{
    Library *lib = nullptr;
    foreach (FWObject *obj, db->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }
    return lib;
}

RuleSetDiffDialog::~RuleSetDiffDialog()
{
    delete ui;
}

void RuleSetDiffDialog::updateFirewallList()
{
    if (!m_currentUserLibrary) return;

    list<FWObject *> clusters = m_currentUserLibrary->getByTypeDeep(Cluster::TYPENAME);
    clusters.sort(sortByName);

    foreach (FWObject *cluster, clusters)
        if (m_originalUserLibrary->getById(cluster->getId(), true))
            ui->firewall->addItem(QString::fromStdString((cluster->getName())), QVariant(cluster->getId()));

    list<FWObject *> firewalls = m_currentUserLibrary->getByTypeDeep(Firewall::TYPENAME);
    firewalls.sort(sortByName);

    foreach(FWObject *firewall, firewalls)
        if (m_originalUserLibrary->getById(firewall->getId(), true))
            ui->firewall->addItem(QString::fromStdString(firewall->getName()), QVariant(firewall->getId()));
}

void RuleSetDiffDialog::updateRuleSetList() {
    ui->ruleset->clear();

    if (!m_currentUserLibrary) return;

    FWObject *currentFirewall = m_currentUserLibrary->getById(
                ui->firewall->itemData(ui->firewall->currentIndex()).toInt(), true);

    FWObject *originalFirewall = m_originalUserLibrary->getById(
                ui->firewall->itemData(ui->firewall->currentIndex()).toInt(), true);

    if (!currentFirewall || !originalFirewall) return;

    foreach (FWObject *ruleset, static_cast<list<FWObject *> >(*currentFirewall)) {
        if ((ruleset->getTypeName() == Policy::TYPENAME) ||
                (ruleset->getTypeName() == NAT::TYPENAME) ||
                (ruleset->getTypeName() == Routing::TYPENAME))
            if (originalFirewall->getById(ruleset->getId()))
                ui->ruleset->addItem(QString::fromStdString(ruleset->getName()), QVariant(ruleset->getId()));
    }
}

void RuleSetDiffDialog::updateRuleSetView() {
    if (!m_currentUserLibrary || !m_originalUserLibrary || !ui->ruleset->count()) return;

    FWObject *currentRuleSet = m_currentUserLibrary->getById(
                ui->ruleset->itemData(ui->ruleset->currentIndex()).toInt(), true);

    FWObject *originalRuleSet = m_originalUserLibrary->getById(currentRuleSet->getId(), true);

    if (!currentRuleSet || !originalRuleSet) return;

    RuleSetModel *currentRuleSetModel = nullptr, *originalRuleSetModel = nullptr;

    if (Policy::isA(currentRuleSet)) {
        currentRuleSetModel = new PolicyModel(Policy::cast(currentRuleSet), this);
        originalRuleSetModel = new PolicyModel(Policy::cast(originalRuleSet), this);
    }

    if (NAT::isA(currentRuleSet)) {
        currentRuleSetModel = new NatModel(NAT::cast(currentRuleSet), this);
        originalRuleSetModel = new NatModel(NAT::cast(originalRuleSet), this);
    }

    if (Routing::isA(currentRuleSet)) {
        currentRuleSetModel = new RoutingModel(Routing::cast(currentRuleSet), this);
        originalRuleSetModel = new RoutingModel(Routing::cast(originalRuleSet), this);
    }

    RuleSetDiffModel *originalDiffModel = new RuleSetDiffModel(this, originalRuleSetModel, m_project);
    RuleSetDiffDelegate *originalDelegate = new RuleSetDiffDelegate(originalDiffModel, new FWObjectSelectionModel(), originalDiffModel, m_project);
    ui->originalRuleSetView->setModel(originalDiffModel);
    ui->originalRuleSetView->setItemDelegate(originalDelegate);
    originalDiffModel->displayUnmodifiedRules(ui->displayUnmodifiedRulesBtn->isChecked());
    ui->originalRuleSetView->expandAll();

    RuleSetDiffModel *currentDiffModel = new RuleSetDiffModel(this, currentRuleSetModel, m_project);
    RuleSetDiffDelegate *currentDelegate = new RuleSetDiffDelegate(currentDiffModel, new FWObjectSelectionModel(), currentDiffModel, m_project);
    ui->currentRuleSetView->setModel(currentDiffModel);
    ui->currentRuleSetView->setItemDelegate(currentDelegate);
    currentDiffModel->displayUnmodifiedRules(ui->displayUnmodifiedRulesBtn->isChecked());
    ui->currentRuleSetView->expandAll();

    setLabelColor(ui->addColorLbl, st->getDiffColor(FWBSettings::ADD_COLOR));
    ui->addTextLbl->setText(QString(tr("Add (%1)"))
                            .arg(QString::number(m_project->getStatistics(currentRuleSet->getId(), DiffType::Add))));

    setLabelColor(ui->editColorLbl, st->getDiffColor(FWBSettings::EDIT_COLOR));
    ui->editTextLbl->setText(QString(tr("Edit (%1)"))
                             .arg(QString::number(m_project->getStatistics(currentRuleSet->getId(), DiffType::Edit))));

    setLabelColor(ui->moveColorLbl, st->getDiffColor(FWBSettings::MOVE_COLOR));
    ui->moveTextLbl->setText(QString(tr("Move (%1)"))
                             .arg(QString::number(m_project->getStatistics(currentRuleSet->getId(), DiffType::Move))));

    setLabelColor(ui->removeColorLbl, st->getDiffColor(FWBSettings::REMOVE_COLOR));
    ui->removeTextLbl->setText(QString(tr("Remove (%1)"))
                               .arg(QString::number(m_project->getStatistics(currentRuleSet->getId(), DiffType::Remove))));
}

void RuleSetDiffDialog::setLabelColor(QLabel *label, const QString &color)
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(color));
    label->setAutoFillBackground(true);
    label->setPalette(palette);
}

void RuleSetDiffDialog::horizontalScrollBarValueChanged(int val)
{
    ui->originalRuleSetView->horizontalScrollBar()->setValue(val);
    ui->currentRuleSetView->horizontalScrollBar()->setValue(val);
}

void RuleSetDiffDialog::itemClicked(const QModelIndex &index)
{
    if (sender() == ui->currentRuleSetView)
        selectItem(index, true);
    else if (sender() == ui->originalRuleSetView)
        selectItem(index, false);
}

void RuleSetDiffDialog::itemDoubleClicked(const QModelIndex &index)
{
    if (sender() == ui->currentRuleSetView)
        selectItem(index, true, true);
    else if (sender() == ui->originalRuleSetView)
        selectItem(index, false, true);
}

void RuleSetDiffDialog::selectItem(const QModelIndex &index, bool sourceIsCurrentRuleSetView, bool doubleClick)
{
    RuleSetDiffModel *srcProxyModel, *dstProxyModel;
    RuleSetModel *ruleSetModel;
    QTreeView *view;

    if (sourceIsCurrentRuleSetView) {
        srcProxyModel = static_cast<RuleSetDiffModel *>(ui->currentRuleSetView->model());
        dstProxyModel = static_cast<RuleSetDiffModel *>(ui->originalRuleSetView->model());
        ruleSetModel = static_cast<RuleSetModel *>(dstProxyModel->sourceModel());
        view = ui->originalRuleSetView;
    } else {
        srcProxyModel = static_cast<RuleSetDiffModel *>(ui->originalRuleSetView->model());
        dstProxyModel = static_cast<RuleSetDiffModel *>(ui->currentRuleSetView->model());
        ruleSetModel = static_cast<RuleSetModel *>(dstProxyModel->sourceModel());
        view = ui->currentRuleSetView;
    }

    RuleNode *node = static_cast<RuleNode *>(srcProxyModel->mapToSource(index).internalPointer());

    if (node->type == RuleNode::Rule) {
        Rule *rule = Rule::cast(ruleSetModel->getRuleSet()->getById(node->rule->getId(), true));
        if (rule) {
            view->setCurrentIndex(dstProxyModel->mapFromSource(
                        ruleSetModel->indexForPosition(
                            rule->getPosition())));
            if (doubleClick) {
                ui->originalRuleSetView->scrollTo(ui->originalRuleSetView->currentIndex(),
                                                  QAbstractItemView::PositionAtCenter);
                ui->currentRuleSetView->scrollTo(ui->currentRuleSetView->currentIndex(),
                                                 QAbstractItemView::PositionAtCenter);
            }
            return;
        }
    }

    if (node->type == RuleNode::Group) {
        if (sourceIsCurrentRuleSetView) {
            view->setCurrentIndex(dstProxyModel->mapFromSource(
                ruleSetModel->index(
                    m_project->getRenamedGroups().key(node->name, node->name))));
        } else {
            view->setCurrentIndex(dstProxyModel->mapFromSource(
                ruleSetModel->index(
                    m_project->getRenamedGroups().value(node->name, node->name))));
        }
        if (doubleClick) {
            ui->originalRuleSetView->scrollTo(ui->originalRuleSetView->currentIndex(),
                                              QAbstractItemView::PositionAtCenter);
            ui->currentRuleSetView->scrollTo(ui->currentRuleSetView->currentIndex(),
                                             QAbstractItemView::PositionAtCenter);
        }
        return;
    }

    view->clearSelection();
}

void RuleSetDiffDialog::itemExpanded(const QModelIndex &index)
{
    expandCollapseItem(index, true);
}

void RuleSetDiffDialog::itemCollapsed(const QModelIndex &index)
{
    expandCollapseItem(index);
}

void RuleSetDiffDialog::expandCollapseItem(const QModelIndex &index, bool expand)
{
    RuleSetDiffModel *srcProxyModel, *dstProxyModel;
    RuleSetModel *ruleSetModel;
    QTreeView *view;

    if (sender() == ui->currentRuleSetView) {
        srcProxyModel = static_cast<RuleSetDiffModel *>(ui->currentRuleSetView->model());
        dstProxyModel = static_cast<RuleSetDiffModel *>(ui->originalRuleSetView->model());
        ruleSetModel = static_cast<RuleSetModel *>(dstProxyModel->sourceModel());
        view = ui->originalRuleSetView;
    } else if (sender() == ui->currentRuleSetView) {
        srcProxyModel = static_cast<RuleSetDiffModel *>(ui->originalRuleSetView->model());
        dstProxyModel = static_cast<RuleSetDiffModel *>(ui->currentRuleSetView->model());
        ruleSetModel = static_cast<RuleSetModel *>(dstProxyModel->sourceModel());
        view = ui->currentRuleSetView;
    } else {
        return;
    }

    RuleNode *node = static_cast<RuleNode *>(srcProxyModel->mapToSource(index).internalPointer());

    if (node->type == RuleNode::Group) {
        if (expand) {
            view->expand(dstProxyModel->mapFromSource(ruleSetModel->index(node->name)));
        } else {
            view->collapse(dstProxyModel->mapFromSource(ruleSetModel->index(node->name)));
        }
    }
}

void RuleSetDiffDialog::displayUnmodifiedRules()
{
    RuleSetDiffModel *originalDiffModel = static_cast<RuleSetDiffModel *>(ui->originalRuleSetView->model());
    RuleSetDiffModel *currentDiffModel = static_cast<RuleSetDiffModel *>(ui->currentRuleSetView->model());

    if (!originalDiffModel || !currentDiffModel) return;

    originalDiffModel->displayUnmodifiedRules(ui->displayUnmodifiedRulesBtn->isChecked());
    ui->originalRuleSetView->expandAll();
    currentDiffModel->displayUnmodifiedRules(ui->displayUnmodifiedRulesBtn->isChecked());
    ui->currentRuleSetView->expandAll();
}
