/*

                          Firewall Builder

                 Copyright (C) 2003-2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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


  This module has been completely rewritten by yalovoy@gmail.com in 2009

*/

#include "global.h"
#include "utils.h"

#include "platforms.h"
#include "RuleSetView.h"
#include "RuleSetModel.h"
#include "ColDesc.h"
#include "FWObjectSelectionModel.h"
#include "RuleSetViewDelegate.h"
#include "FWBSettings.h"
#include "FWObjectClipboard.h"
#include "FWObjectPropertiesFactory.h"
#include "FWObjectDrag.h"
#include "FWWindow.h"
#include "FWBTree.h"
#include "FWCmdRule.h"
#include "ProjectPanel.h"
#include "FindObjectWidget.h"
#include "events.h"
#include "DialogFactory.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Network.h"

#include <memory>

#include <QtDebug>
#include <QMouseEvent>
#include <QtAlgorithms>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QTime>

using namespace libfwbuilder;
using namespace std;



RuleSetView::RuleSetView(ProjectPanel *project, QWidget *parent):QTreeView(parent)
{
    if (fwbdebug) qDebug("RuleSetView::RuleSetView");

    this->project = project;
    fwosm = new FWObjectSelectionModel();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setAllColumnsShowFocus(false);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setSectionsMovable(false);

    connect (this, SIGNAL (customContextMenuRequested(const QPoint&)),
             this, SLOT (showContextMenu(const QPoint&)));
    connect (this, SIGNAL( doubleClicked(const QModelIndex&) ),
             this, SLOT( itemDoubleClicked(const QModelIndex&) ) );

    connect (this, SIGNAL (collapsed(QModelIndex)),
             this, SLOT (saveCollapsedGroups()));
    connect (this, SIGNAL (expanded(QModelIndex)),
             this, SLOT (saveCollapsedGroups()));
    connect (this, SIGNAL (collapsed(QModelIndex)),
             this, SLOT (updateAllColumnsSize()));
    connect (this, SIGNAL (expanded(QModelIndex)),
             this, SLOT (updateAllColumnsSize()));

    initActions();

    popup_menu = new QMenu(this);

    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->header()->setStretchLastSection(false);
}

RuleSetView::~RuleSetView()
{
    if (fwbdebug) qDebug("RuleSetView::~RuleSetView");
    delete fwosm;
    delete compileRuleAction;
    delete moveRuleUpAction;
    delete moveRuleDownAction;
    delete removeFromGroupAction;
    delete newGroupAction;
    delete insertRuleAction;
    delete addRuleAfterCurrentAction;
    delete addToGroupAboveAction;
    delete addToGroupBelowAction;
    delete removeRuleAction;

    delete copyRuleAction;
    delete cutRuleAction;
    delete pasteRuleAboveAction;
    delete pasteRuleBelowAction;
    delete disableRuleAction;
    delete enableRuleAction;

    delete setColorEmptyAction;
    delete setColorRedAction;
    delete setColorBlueAction;
    delete setColorOrangeAction;
    delete setColorPurpleAction;
    delete setColorGrayAction;
    delete setColorYellowAction;
    delete setColorGreenAction;
}

void RuleSetView::init()
{
    if (fwbdebug) qDebug("RuleSetView::init");

    /*
     * #2474 : I want standard Qt classes to paint column 0 and area
     * to the left of the clumn 0 because that is where [+] and [-]
     * appear when we have rule groups. However I want to avoid using
     * the same standard color used to highlight currently selected
     * object in rules. To do that, I reset QPalette::Highlight color
     * in the palette used to draw everything in the rules. I am going
     * to have to reset palette back to the standard color in
     * RuleSetViewDelegate when I paint individual cells.
     */
    QPalette updated_palette = palette();
    updated_palette.setColor(QPalette::Highlight, QColor("silver"));
    setPalette(updated_palette);


    setUpdatesEnabled(false);
    QTime t; t.start();
    configureGroups();
    if (fwbdebug)
        qDebug("RuleSetView configureGroups: %d ms", t.restart());
    restoreCollapsedGroups();
    if (fwbdebug)
        qDebug("RuleSetView restoreCollapsedGroups: %d ms", t.restart());
    resizeColumns();
    if (fwbdebug)
        qDebug("RuleSetView resizeColumns: %d ms", t.restart());
    setUpdatesEnabled(true);
}

void RuleSetView::configureGroups()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    QList<QModelIndex> list;
    md->getGroups(list);
    QModelIndex parent;
    foreach(QModelIndex index, list)
    {
        setFirstColumnSpanned(index.row(), parent, true);
    }
}

void RuleSetView::initActions()
{
    // Compile rule
    compileRuleAction = createAction(tr("Compile Rule"), SLOT(compileCurrentRule()),
                                     QKeySequence(Qt::Key_X));
    addAction(compileRuleAction );
    compileRuleAction->setVisible(true);
    compileRuleAction->setEnabled(true);

    // Move rule up
    moveRuleUpAction = createAction(tr("Move Rule Up"), SLOT( moveRuleUp()),
                                    QKeySequence(Qt::CTRL + Qt::Key_PageUp));
    addAction(moveRuleUpAction );

    // Move rule down
    moveRuleDownAction = createAction(tr("Move Rule Down"), SLOT( moveRuleDown()),
                                      QKeySequence(Qt::CTRL + Qt::Key_PageDown));
    addAction(moveRuleDownAction );

    // Remove rules from group
    removeFromGroupAction = createAction(tr("Remove From the Group"),
                                         SLOT( removeFromGroup()));

    // New group
    newGroupAction = createAction(tr("New Group"), SLOT( newGroup()));

    // Insert Rule
    insertRuleAction = createAction( tr("Insert New Rule"), SLOT( insertRule() ) );

    addRuleAfterCurrentAction = createAction(tr("Add New Rule Below"),
                                             SLOT( addRuleAfterCurrent() ));

    addToGroupAboveAction = createAction("addToGroupAboveAction",
                                         SLOT( addToGroupAbove() ));
    addToGroupBelowAction = createAction("addToGroupBelowAction",
                                         SLOT( addToGroupBelow() ));

    // Remove rule
    removeRuleAction = createAction(tr("Remove Rule"), SLOT( removeRule()));

    // Clipboard operations
    copyRuleAction = createAction(tr("Copy Rule"), SLOT( copyRule() ));
    cutRuleAction = createAction(tr("Cut Rule"), SLOT( cutRule() ));
    pasteRuleAboveAction = createAction(tr("Paste Rule Above"),
                                        SLOT( pasteRuleAbove() ));
    pasteRuleBelowAction = createAction(tr("Paste Rule Below"),
                                        SLOT( pasteRuleBelow() ));

    //Disable or Enable rules
    disableRuleAction = createAction(tr("Enable Rule"), SLOT( disableRule() ));
    enableRuleAction = createAction(tr("Disable Rule"), SLOT( enableRule() ));

    //Change color actions

    QPixmap pcolor(16,16);

    setColorEmptyAction = createAction(tr("No Color"), SLOT( setColorEmpty() ));
    pcolor.fill(QColor(255,255,255));
    setColorEmptyAction->setIcon(QIcon(pcolor));
    setColorEmptyAction->setVisible(true);

    setColorRedAction = createAction(st->getLabelText(FWBSettings::RED),
                                     SLOT( setColorRed() ));
    pcolor.fill(st->getLabelColor(FWBSettings::RED));
    setColorRedAction->setIcon(QIcon(pcolor));
    setColorRedAction->setVisible(true);

    setColorBlueAction = createAction(st->getLabelText(FWBSettings::BLUE),
                                      SLOT( setColorBlue() ));
    pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
    setColorBlueAction->setIcon(QIcon(pcolor));
    setColorBlueAction->setVisible(true);

    setColorOrangeAction = createAction(st->getLabelText(FWBSettings::ORANGE),
                                        SLOT( setColorOrange() ));
    pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
    setColorOrangeAction->setIcon(QIcon(pcolor));
    setColorOrangeAction->setVisible(true);

    setColorPurpleAction = createAction(st->getLabelText(FWBSettings::PURPLE),
                                        SLOT( setColorPurple() ));
    pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
    setColorPurpleAction->setIcon(QIcon(pcolor));
    setColorPurpleAction->setVisible(true);

    setColorGrayAction = createAction(st->getLabelText(FWBSettings::GRAY),
                                      SLOT( setColorGray() ));
    pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
    setColorGrayAction->setIcon(QIcon(pcolor));
    setColorGrayAction->setVisible(true);

    setColorYellowAction = createAction(st->getLabelText(FWBSettings::YELLOW),
                                        SLOT( setColorYellow() ));
    pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
    setColorYellowAction->setIcon(QIcon(pcolor));
    setColorYellowAction->setVisible(true);

    setColorGreenAction = createAction(st->getLabelText(FWBSettings::GREEN),
                                       SLOT( setColorGreen() ));
    pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
    setColorGreenAction->setIcon(QIcon(pcolor));
    setColorGreenAction->setVisible(true);

}


QAction* RuleSetView::createAction(QString label, const char* member,
                                   const QKeySequence &shortcut)
{
    QAction* action = new QAction(label, this);
    action->setShortcut(shortcut);

    connect (action, SIGNAL(triggered()), this, member);

    action->setEnabled(false);
    action->setVisible(false);

    return action;
}

RuleSetView* RuleSetView::getRuleSetViewByType(ProjectPanel *project,
                                               RuleSet *ruleset,
                                               QWidget *parent)
{
    if (fwbdebug) qDebug("RuleSetView::getRuleSetViewByType");
    if (Policy::isA(ruleset))
        return new PolicyView(project, Policy::cast(ruleset), parent);
    if (NAT::isA(ruleset))
        return new NATView(project, NAT::cast(ruleset), parent);
    if (Routing::isA(ruleset))
        return new RoutingView(project, Routing::cast(ruleset), parent);
    return nullptr;
}

void RuleSetView::makeCurrentRuleVisible()
{
    scrollTo( currentIndex(), QAbstractItemView::PositionAtCenter);
}

void RuleSetView::selectRE(QModelIndex index)
{
    if (fwbdebug)
        qDebug() << "RuleSetView::selectRE(QModelIndex index)"
                 << index;

    if (fwosm->index != index)
    {
        fwosm->selectedObject = nullptr;
        fwosm->index = index;
        setCurrentIndex(index);

        scrollTo( index, QAbstractItemView::PositionAtCenter);
    }
}

void RuleSetView::selectRE(libfwbuilder::FWReference *ref)
{
    if (fwbdebug) qDebug() << "RuleSetView::selectRE(libfwbuilder::FWReference *ref)";

    /* need to find row and column this object is in and show it */
    RuleElement *re = RuleElement::cast(ref->getParent());
    assert(re);
    selectRE(re, ref->getPointer());
}

void RuleSetView::selectRE(libfwbuilder::Rule *rule, int col)
{
    if (fwbdebug) qDebug() << "RuleSetView::selectRE(libfwbuilder::Rule *rule, int col)";
    RuleSetModel* md = ((RuleSetModel*)model());
    selectRE(md->index(rule, col));
}

void RuleSetView::selectRE(libfwbuilder::Rule *rule, ColDesc::ColumnType type)
{
    if (fwbdebug)
        qDebug() << "RuleSetView::selectRE(libfwbuilder::Rule *rule, ColDesc::ColumnType type)";
    RuleSetModel* md = ((RuleSetModel*)model());
    int col = md->columnByType(type);

    selectRE(rule, col);
}

void RuleSetView::selectRE(libfwbuilder::RuleElement *re, libfwbuilder::FWObject *obj)
{
    if (fwbdebug)
        qDebug() << "RuleSetView::selectRE(libfwbuilder::RuleElement *re, libfwbuilder::FWObject *obj)";

    Rule *rule = Rule::cast(re->getParent());
    assert(rule!=nullptr);

    RuleSetModel* md = ((RuleSetModel*)model());

    QModelIndex index = md->index(rule, re);

    selectRE(index);

    setCurrentIndex(index);
    fwosm->setSelected(obj, index);

}

int RuleSetView::getColByType(ColDesc::ColumnType type) const
{
    RuleSetModel* md = ((RuleSetModel*)model());
    return md->columnByType(type);
}

void RuleSetView::mousePressEvent( QMouseEvent* ev )
{
    //TODO: provide custom implementation of QTreeView::mousePressEvent( ev ); for column != 0
    QTreeView::mousePressEvent( ev );

    const QModelIndex index = currentIndex();//indexAt (ev->pos());

    if (index.column() == 0)
    {
        fwosm->setSelected(nullptr, index);
        return;
    }

    FWObject *object = getObject(ev->pos(), index);

    // if (fwbdebug) qDebug("RuleSetView::contentsMousePressEvent  "
    //        "obj=%s  row=%d  col=%d",
    //        (object)?object->getName().c_str():"NULL", index.row(), index.column());

    if (object)
    {
        selectObject(object, index);
        startingDrag = (fwosm->index.row()==index.row() &&
                        fwosm->index.column()==index.column() &&
                        fwosm->selectedObject==object);
    } else {
        fwosm->setSelected(nullptr, index);
    }

}

void RuleSetView::mouseReleaseEvent( QMouseEvent* ev )
{
    //if (fwbdebug) qDebug() << "RuleSetView::mouseReleaseEvent";
    QTreeView::mouseReleaseEvent(ev);

    const QModelIndex index = indexAt (ev->pos());

    if (index.column() == 0) return;

    RuleSetModel* md = ((RuleSetModel*)model());

    if (md->getRuleSet()->size()!=0)
    {
        ev->accept();
    };

    // if (md->getRuleSet()->size()!=0 &&
    //     mw->isEditorVisible() && !switchObjectInEditor( currentIndex()) )
    // {
    //     ev->accept();
    // };

}

/*
 * Why using persistent QMenu object for the popup menu:
 *
 * If user hits Cmd-Q on Mac while popup menu is open, we get a
 * crash. The problem is that when close event propagates and
 * eventually closes the popup menu, RuleSetView::showContextMenu()
 * tries to delete QMenu object and we get the following error and
 * crash: "QObject: Do not delete object, 'qt_scrollarea_viewport',
 * during its event handler!". Instead of making sure we do not delete
 * the object in some circumstances, or find a way to delete it
 * safely, it is much easier to just avoid having to delete it at all.
 */
void RuleSetView::showContextMenu(const QPoint& pos)
{
    popup_menu->clear();

    const QModelIndex index = indexAt ( pos);
    if (index.isValid())
    {
        int column = index.column();
        RuleNode* node = static_cast<RuleNode *>(index.internalPointer());

        if (node->type == RuleNode::Group)
        {
            addGroupMenuItemsToContextMenu(popup_menu);
        }
        else
        {
            compileRuleAction->setEnabled(!node->rule->isDisabled());

            if (column < 1)
            {
                addRowMenuItemsToMenu(popup_menu);
            }
            else
            {
                addColumnRelatedMenu(popup_menu, index, node, pos);
            }

            addCommonRowItemsToContextMenu(popup_menu);
        }
    }
    else
    {
        addGenericMenuItemsToContextMenu(popup_menu);
    }

    popup_menu->exec(mapToGlobal(pos));
//    delete menu;
}

void RuleSetView::addCommonRowItemsToContextMenu(QMenu *menu) const
{
    menu->addSeparator();
    menu->addAction(compileRuleAction);
}

void RuleSetView::mouseMoveEvent( QMouseEvent* ev )
{
    if (startingDrag)
    {
        QDrag* drag = dragObject();
        if (drag)
            drag->start(Qt::CopyAction | Qt::MoveAction); //just start dragging
        startingDrag = false;
        return;
    }
    QTreeView::mouseMoveEvent(ev);
}

QDrag* RuleSetView::dragObject()
{
    FWObject *obj = fwosm->selectedObject;

    if (obj==nullptr) return nullptr;

    // TODO: use FWBTree::setObjectIcon()
    QString icn = (":/Icons/" + obj->getTypeName() + "/icon").c_str();
    list<FWObject*> dragobj;
    dragobj.push_back(obj);
    FWObjectDrag *drag = new FWObjectDrag(dragobj, this, nullptr);
    QPixmap pm = LoadPixmap(icn);
    drag->setPixmap( pm );
    return drag;
}

void RuleSetView::addColumnRelatedMenu(QMenu *menu, const QModelIndex &index,
                                       RuleNode* node, const QPoint& pos)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();
    switch (colDesc.type)
    {
        case ColDesc::Action:
            {
                Firewall *f = md->getFirewall();
                if (f == nullptr) break;
                string platform = f->getStr("platform");
                QString action_name;

                if (NATRule::isA(node->rule))
                {
                    if (Resources::isTargetActionSupported(platform,"Translate"))
                    {
                        action_name = getActionNameForPlatform(
                            f, NATRule::getActionAsString(NATRule::Translate));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Continue/icon")),
                                         action_name,
                                         this, SLOT( changeActionToTranslate() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Branch"))
                    {
                        action_name = getActionNameForPlatform(
                            f, NATRule::getActionAsString(NATRule::Branch));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/NATBranch/icon")),
                                         action_name,
                                         this, SLOT( changeActionToNATBranch() ));
                    }

                } else
                {
                    if (Resources::isTargetActionSupported(platform,"Accept"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Accept));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Accept/icon")),
                                         action_name,
                                         this, SLOT( changeActionToAccept() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Deny"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Deny));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Deny/icon")),
                                         action_name,
                                         this, SLOT( changeActionToDeny() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Reject"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Reject));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Reject/icon")),
                                         action_name,
                                         this, SLOT( changeActionToReject() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Accounting"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Accounting));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Accounting/icon")),
                                         action_name,
                                         this, SLOT( changeActionToAccounting() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Pipe"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Pipe));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Pipe/icon")),
                                         action_name,
                                         this, SLOT( changeActionToPipe() ));
                    }

/*
 * #2367

                    if (Resources::isTargetActionSupported(platform,"Tag"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Tag));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Tag/icon")),
                                         action_name,
                                         this, SLOT( changeActionToTag() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Classify"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Classify));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Classify/icon")),
                                         action_name,
                                         this, SLOT( changeActionToClassify() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Route"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Route));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Route/icon")),
                                         action_name,
                                         this, SLOT( changeActionToRoute() ));
                    }
*/


                    if (Resources::isTargetActionSupported(platform,"Custom"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Custom));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Custom/icon")),
                                         action_name,
                                         this, SLOT( changeActionToCustom() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Branch"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Branch));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Branch/icon")),
                                         action_name,
                                         this, SLOT( changeActionToBranch() ));
                    }
                    if (Resources::isTargetActionSupported(platform,"Continue"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Continue));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Continue/icon")),
                                         action_name,
                                         this, SLOT( changeActionToContinue() ));
                    }
                }

                menu->addSeparator ();
                QAction *paramID;
                paramID = menu->addAction( tr("Parameters"),
                                            this, SLOT( editSelected() ));

                PolicyRule *rule = PolicyRule::cast( node->rule );
                if (rule!=nullptr)
                {
                    string act = rule->getActionAsString();
                    if (Resources::getActionEditor(platform,act)=="None")
                        paramID->setEnabled(false);
                }

                break;
            }

        case ColDesc::Direction:
            menu->addAction( QIcon(LoadPixmap(":/Icons/Inbound/icon")),
                              tr("Inbound"),
                              this, SLOT( changeDirectionToIn() ));
            menu->addAction( QIcon(LoadPixmap(":/Icons/Outbound/icon")),
                              tr("Outbound"),
                              this, SLOT( changeDirectionToOut() ));
            menu->addAction( QIcon(LoadPixmap(":/Icons/Both/icon")),
                              tr("Both"),
                              this, SLOT( changeDirectionToBoth() ));
            break;

        case ColDesc::Comment:
            menu->addAction( tr("Edit")   , this , SLOT( editSelected() ) );
            break;

        case ColDesc::Metric:
            menu->addAction( tr("Edit")   , this , SLOT( editSelected() ) );
            break;

        case ColDesc::Options:
            menu->addAction( QIcon(LoadPixmap(":/Icons/Options/icon")),
                              tr("Rule Options"),
                              this, SLOT( editSelected() ));

            if (md->getRuleSet()->getTypeName() == Policy::TYPENAME)
            {
                menu->addAction( QIcon(LoadPixmap(":/Icons/Log/icon")),
                                  tr("Logging On"),
                                  this, SLOT( changeLogToOn() ));
                menu->addAction( QIcon(LoadPixmap(":/Icons/Blank/icon")),
                                  tr("Logging Off"),
                                  this, SLOT( changeLogToOff() ));
            }
            break;
        case ColDesc::Object:
        case ColDesc::Time:
            {
                RuleElement *re = getRE(index);
                if (re==nullptr) return;
                FWObject *object = getObject(pos, index);

                QAction *editID = menu->addAction(
                    tr("Edit")   , this , SLOT( editSelected() ) );

                menu->addSeparator();

                QAction *copyID = menu->addAction(
                    tr("Copy")   , this , SLOT( copySelectedObject() ) );

                QAction *cutID  = menu->addAction(
                    tr("Cut")    , this , SLOT( cutSelectedObject() ) );

                QAction *pasteID = menu->addAction(
                    tr("Paste")  , this , SLOT( pasteObject() ) );

                QAction *delID  =menu->addAction(
                    tr("Delete") ,  this , SLOT( deleteSelectedObject() ) );

                menu->addSeparator();

                QAction *fndID = menu->addAction(
                    tr("Where used") , this , SLOT( findWhereUsedSlot()));

                QAction *revID = menu->addAction(
                    tr("Reveal in tree") ,this , SLOT( revealObjectInTree() ) );

                menu->addSeparator();

                if (re->isDummy()) {
                    if (isDummy(re, getObjectNumber(fwosm->selectedObject, index)))
                        menu->addAction(
                                tr("Convert to Any"), this, SLOT ( convertToAny() ) );
                }

                QAction *negID  = menu->addAction(
                    tr("Negate") , this , SLOT( negateRE() ) );

                if (object == nullptr || re->isAny()) editID->setEnabled(false);
                copyID->setEnabled(!re->isAny());
                cutID->setEnabled(!re->isAny());
                delID->setEnabled(!re->isAny());

                // see #1976 do not allow pasting object that has been deleted
                // also disable "Paste" if object in the clipboard is a rule

                FWObject *obj_in_clipboard =
                    FWObjectClipboard::obj_clipboard->getObject();

                pasteID->setEnabled(true);
                        
                if (obj_in_clipboard == nullptr) pasteID->setEnabled(false);
                else
                {
                    if (Rule::cast(obj_in_clipboard) != nullptr)
                        pasteID->setEnabled(false);

                    FWObject *lib = obj_in_clipboard->getLibrary();
                    if (lib != nullptr && lib->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
                        pasteID->setEnabled(false);
                }

                string cap_name;
                if (Policy::cast(md->getRuleSet())!=nullptr) cap_name="negation_in_policy";
                if (NAT::cast(md->getRuleSet())!=nullptr) cap_name="negation_in_nat";

                Firewall *f = md->getFirewall();
                if (f == nullptr) break;

                bool supports_neg=false;
                try  {
                    supports_neg = Resources::getTargetCapabilityBool(
                        f->getStr("platform"), cap_name);
                } catch (FWException &ex)
                {
                    QMessageBox::critical( nullptr , "Firewall Builder",
                                           ex.toString().c_str(),
                                           QString::null,QString::null);
                }
                negID->setEnabled(supports_neg &&  !re->isAny());
                fndID->setEnabled(!re->isAny());
                revID->setEnabled(!re->isAny());

                break;
            }

        default :
            menu->addAction( tr("Edit")   , this , SLOT( editRE() ) );
    }
}

void RuleSetView::addGenericMenuItemsToContextMenu(QMenu *menu) const
{
    if (((RuleSetModel*)model())->isEmpty())
        menu->addAction(tr("Insert New Rule"), this, SLOT( insertRule() ));
    else
    {
        menu->addAction(tr("Add New Rule on Top"), this,
                        SLOT( insertNewRuleOnTop()));
        menu->addAction(tr("Add New Rule at the Bottom"), this,
                        SLOT( insertNewRuleAtBottom()));
    }
    menu->addSeparator();
    menu->addAction(tr("Paste Rule"), this, SLOT( pasteRuleBelow()));
}

void RuleSetView::addGroupMenuItemsToContextMenu(QMenu *menu) const
{
    menu->addAction( tr("Rename group"), this, SLOT( renameGroup() ));
    menu->addSeparator();
    addChangeColorSubmenu(menu);
}

void RuleSetView::addChangeColorSubmenu(QMenu *menu) const
{
    QMenu *subcolor = menu->addMenu(tr("Change color") );

    subcolor->addAction(setColorEmptyAction);
    subcolor->addAction(setColorRedAction);
    subcolor->addAction(setColorOrangeAction);
    subcolor->addAction(setColorYellowAction);
    subcolor->addAction(setColorGreenAction);
    subcolor->addAction(setColorBlueAction);
    subcolor->addAction(setColorPurpleAction);
    subcolor->addAction(setColorGrayAction);
}

void RuleSetView::addRowMenuItemsToMenu(QMenu *menu) const
{
    if (fwbdebug) qDebug() << "RuleSetView::addRowMenuItemsToMenu menu=" << menu;

    menu->addAction(removeFromGroupAction);
    menu->addAction(newGroupAction);
    menu->addAction(addToGroupAboveAction);
    menu->addAction(addToGroupBelowAction);

    menu->addSeparator();

    addChangeColorSubmenu(menu);

    menu->addSeparator();

    menu->addAction( insertRuleAction );
    menu->addAction( addRuleAfterCurrentAction );
    menu->addAction( removeRuleAction );

    menu->addSeparator();

    menu->addAction( moveRuleUpAction);
    menu->addAction( moveRuleDownAction);

    menu->addSeparator();

    menu->addAction(copyRuleAction);
    menu->addAction(cutRuleAction);
    menu->addAction(pasteRuleAboveAction);
    menu->addAction(pasteRuleBelowAction);

    menu->addSeparator();

    menu->addAction(enableRuleAction);
    menu->addAction(disableRuleAction);

}

void RuleSetView::itemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) return;
    if (index.row() < 0) return;
    if (index.column() == 0) return; // double click on rule number does nothing

    // ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();
    // if (fwosm->selectedObject!=nullptr)
    // {
    //     QCoreApplication::postEvent(
    //         mw,
    //         new showObjectInTreeEvent(
    //             project->getFileName(), fwosm->selectedObject->getId()));
    // }

    editSelected(index);
}

void RuleSetView::editSelected(const QModelIndex& index)
{
    ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();
    FWObject *obj = fwosm->selectedObject;

    // see #2454 -- we do not want to switch object tree view to the standard
    // objects library when user double clicks on object "any" 
    if (obj != nullptr &&
        (obj->getId() != FWObjectDatabase::ANY_ADDRESS_ID &&
         obj->getId() != FWObjectDatabase::ANY_SERVICE_ID &&
         obj->getId() != FWObjectDatabase::ANY_INTERVAL_ID))
    {
        QCoreApplication::postEvent(
            mw,
            new showObjectInTreeEvent(
                project->getFileName(), fwosm->selectedObject->getId()));
    }

    if (!mw->isEditorVisible()) mw->showEditor();
    switchObjectInEditor(index);
}

void RuleSetView::editSelected()
{
    editSelected(currentIndex());
}

bool RuleSetView::switchObjectInEditor(const QModelIndex& index, bool validate)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!isTreeReadWrite(this,md->getRuleSet())) return false;

    if ( index.column()<=0 || index.row()==-1 ) return false;

    FWObject *object = nullptr;
    ObjectEditor::OptType operation = ObjectEditor::optNone;

    /*
    *  We need to know WHAT we are going to edit

    1. Object
    2. OptType

    * Object == null, OptType = optNone => blank
    * Object == Rule, OptType = optNone => Rule Options
    * Object == Rule, OptType != optNone => Virtual Object (Action, Comment ...)
    * Object != Rule, OptType = optNone => Regular Object Editor

    Then we compare our object 'obj' and OptType with what we already
    have in ObjectEditor/ If they are the same, then we do nothing,
    otherwise we open obj in the  Object Editor

    */

    ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();

    RuleNode *node = md->nodeFromIndex(index);
    if (node->type != RuleNode::Rule) return false;
    Rule *rule = node->rule;

    switch (colDesc.type)
    {
        case ColDesc::Comment:
            object = rule;
            operation = ObjectEditor::optComment;
            break;

        case ColDesc::Metric:
            object = rule;
            operation = ObjectEditor::optMetric;
            break;

        case ColDesc::Direction:
            break;

        case ColDesc::Action:
        {
            //PolicyRule *prule = PolicyRule::cast( rule );
            object = rule;
            operation = ObjectEditor::optAction;
            break;
        }
        case ColDesc::Options:
        {
            /* both policy and routing rules have options. so cast to
             * Rule here. */

            assert(rule);
            object = rule;
            operation = ObjectEditor::optNone;
            break;
        }

        default:
        {
            if ( fwosm->selectedObject!=nullptr)
            {
                object = fwosm->selectedObject;
                break;
            }
        }
    }

    if (!mw->isEditorVisible()) mw->showEditor();

    if (!mw->requestEditorOwnership(this, object, operation, validate))
        return false;

    if (object==mw->getOpenedEditor() && operation==mw->getOpenedOptEditor())
    {
        if (fwbdebug)
            qDebug("RuleSetView::switchObjectInEditor  same object is already opened in the editor");
        return true;
    }

    if (object == nullptr)
    {
        QCoreApplication::postEvent(mw, new clearEditorPanelEvent());
        //mw->blankEditor();
    } else if (operation==ObjectEditor::optNone)
    {
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(
                mw->activeProject()->getFileName(), object->getId()));
        //mw->openEditor(object);
    } else if(Rule::cast(object)!=nullptr)
    {
        QCoreApplication::postEvent(
            mw, new openOptObjectInEditorEvent(project->getFileName(),
                                               object->getId(),
                                               operation));
        //mw->openOptEditor(object, operation);
    }

    return true;
}

QModelIndexList RuleSetView::getSelectedRows() const
{
    QModelIndexList selection = selectedIndexes();
    QModelIndexList res;
    for (QList<QModelIndex>::iterator i = selection.begin(); i != selection.end(); ++i)
    {
        if(!(*i).column())
        {
            res.append(*i);
        }
    }
    return res;
}

void RuleSetView::setSelectedRows(const QModelIndex firstIndex, const QModelIndex lastIndex)
{
    fwosm->reset();
    selectionModel()->clear();
    setCurrentIndex(firstIndex);
    selectionModel()->select(QItemSelection(firstIndex, lastIndex), QItemSelectionModel::Rows | QItemSelectionModel::Select);

    fwosm->setSelected(0, firstIndex);
}

void RuleSetView::removeRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    mw->findObjectWidget->reset();

    QModelIndexList selection = getSelectedRows();

    if (!selection.isEmpty())
    {
        QMap<QModelIndex,QList<int> > itemsInGroups;
        QList<Rule*> rulesToDelete;

// Sort rules to the corresponding groups
        foreach (QModelIndex index, selection)
        {
            if (!index.isValid() || !md->isIndexRule(index)) continue;

            if (mw->isEditorVisible() && mw->getOpenedEditor()==md->nodeFromIndex(index)->rule)
                mw->closeEditor();

            QModelIndex parent = index.parent();
            if (parent.isValid())
            {
                itemsInGroups[parent] << index.row();
            }

            RuleNode* node = md->nodeFromIndex(index);
            if (node->type == RuleNode::Rule)
                rulesToDelete << node->rule;
        }

        //Special case - all rows are inside one group (excluding the
        //first item of the group)

        if (itemsInGroups.size() == 1 &&
            itemsInGroups[itemsInGroups.keys().first()].size() == rulesToDelete.size())
        {
            bool containsFirstRow = false;
            foreach(int row, itemsInGroups[itemsInGroups.keys().first()])
            {
                if (0 == row)
                {
                    containsFirstRow = true;
                    break;
                }
            }

            if (!containsFirstRow)
            {
                FWCmdRuleDeleteFromGroup* cmd =
                    new FWCmdRuleDeleteFromGroup(project,
                                                 md->getRuleSet(), rulesToDelete);
                project->undoStack->push(cmd);
                return;
            }
        }

        FWCmdMacro* macro = new FWCmdMacro("delete rules");

        // Remove items from groups
        QList<QModelIndex> groups = itemsInGroups.keys();
        if (!groups.isEmpty())
        {
            foreach(QModelIndex group, groups)
            {
                qSort(itemsInGroups[group]);

                Rule* first = md->nodeFromIndex(md->index(itemsInGroups[group].at(0), 0, group))->rule;
                Rule* last = md->nodeFromIndex(md->index(itemsInGroups[group].at(itemsInGroups[group].size() - 1), 0, group))->rule;
                QString groupName = md->nodeFromIndex(group)->name;

                FWCmdRuleRemoveFromGroup* cmd = new FWCmdRuleRemoveFromGroup(project, md->getRuleSet(), first, last, groupName, macro);
                Q_UNUSED(cmd);
            }
        }


        // Remove rows
        if (!rulesToDelete.isEmpty())
        {
            FWCmdRuleDelete* cmd =  new FWCmdRuleDelete(project, md->getRuleSet(), rulesToDelete, macro);
            Q_UNUSED(cmd);
        }
        project->undoStack->push(macro);
    }
}

void RuleSetView::renameGroup()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    foreach(QModelIndex index, selection)
    {
        if (!index.isValid()) continue;
        RuleNode *group = md->nodeFromIndex(index);

        // Process only groups. Skip all rules.
        if(group->type != RuleNode::Group) continue;

        QString oldGroupName = group->name;
        bool ok = false;

        QString newGroupName = QInputDialog::getText(
        this, "Rename group",
        tr("Enter group name:"),
        QLineEdit::Normal,
        oldGroupName, &ok);

        if (ok && !newGroupName.isEmpty() && newGroupName != oldGroupName)
        {            
            project->undoStack->push(new FWCmdRuleRenameGroup(project, md->getRuleSet(), oldGroupName, newGroupName));
        }
    }
}

void RuleSetView::setRuleColor(const QString &c)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndexList selection = getSelectedRows();

    // Current behaviour is following:
    // if we have only groups selected then recolor groups
    // if there are rules in selection then selected groups will be ignored and only selected rules will be recolored

    QList<QModelIndex> rules;
    QList<QModelIndex> groups;

    foreach (QModelIndex index, selection)
    {
        if (md->isIndexRule(index))
        {
            rules << index;
        }
        else
        {
            groups << index;
        }
    }

    QList<Rule*> ruleList;
    if (rules.isEmpty())
    {
        // Let's recolor groups - there are no rules in the selection
        foreach(QModelIndex grpIndex, groups)
        {
            foreach(RuleNode* node, md->nodeFromIndex(grpIndex)->children)
            {
                ruleList.append(node->rule);
            }
        }
    }
    else
    {
        // There are rules in selection, so recolor them

        foreach (QModelIndex index, rules)
        {
            ruleList.append(md->nodeFromIndex(index)->rule);
        }
    }
    project->undoStack->push(new FWCmdRuleColor(project, md->getRuleSet(), ruleList, c));
}

void RuleSetView::setColorEmpty()
{
    setRuleColor("");
}

void RuleSetView::setColorRed()
{
    setRuleColor(st->getLabelColor(FWBSettings::RED));
}

void RuleSetView::setColorBlue()
{
    setRuleColor(st->getLabelColor(FWBSettings::BLUE));
}

void RuleSetView::setColorOrange()
{
    setRuleColor(st->getLabelColor(FWBSettings::ORANGE));
}

void RuleSetView::setColorPurple()
{
    setRuleColor(st->getLabelColor(FWBSettings::PURPLE));
}

void RuleSetView::setColorGray()
{
    setRuleColor(st->getLabelColor(FWBSettings::GRAY));
}

void RuleSetView::setColorYellow()
{
    setRuleColor(st->getLabelColor(FWBSettings::YELLOW));
}

void RuleSetView::setColorGreen()
{
    setRuleColor(st->getLabelColor(FWBSettings::GREEN));
}

void RuleSetView::enableRule()
{
    setEnabledRow(true);
}

void RuleSetView::disableRule()
{
    setEnabledRow(false);
}

void RuleSetView::setEnabledRow(bool flag)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    if (!selection.isEmpty())
    {
        foreach (QModelIndex index, selection)
        {
            if (!index.isValid()) continue;

            RuleNode *node = md->nodeFromIndex(index);
            if (node->type != RuleNode::Rule) continue;
            Rule* rule = node->rule;
            if (!rule->isDisabled() == flag) continue;

            FWCmdRuleChange* cmd =
                new FWCmdRuleChange(
                    project,  md->getRuleSet(), rule,
                    (flag)?tr("Enable rule"):tr("Disable rule"));
            Rule* newState = Rule::cast(cmd->getNewState());
            if (flag)
                newState->enable();
            else
                newState->disable();
            project->undoStack->push(cmd);
        }
    }

    updateSelectionSensitiveActions();
}

void RuleSetView::newGroup()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOnlyTopLevelRules(selection))
    {
        bool ok;

        QString newGroupName = QInputDialog::getText(
        this, "Rename group",
        tr("Enter group name:"), QLineEdit::Normal,
        tr("New Group"), &ok);

        if (ok && !newGroupName.isEmpty())
        {

            FWCmdRuleNewGroup* cmd = new FWCmdRuleNewGroup(
                    project, md->getRuleSet(),
                    md->nodeFromIndex(selection.first())->rule,
                    md->nodeFromIndex(selection.last())->rule,
                    newGroupName);
            project->undoStack->push(cmd);
        }
    }
}

void RuleSetView::addToGroupAbove()
{
    addToGroup(true);
}

void RuleSetView::addToGroupBelow()
{
    addToGroup(false);
}

void RuleSetView::addToGroup(bool isAbove)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOnlyTopLevelRules(selection))
    {
        FWCmdRuleAddToGroup* cmd = new FWCmdRuleAddToGroup(
                project, md->getRuleSet(),
                md->nodeFromIndex(selection.first())->rule,
                md->nodeFromIndex(selection.last())->rule,
                isAbove);
        project->undoStack->push(cmd);
    }
}

void RuleSetView::moveRuleUp()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOneLevelRules(selection))
    {

        RuleSetModelIterator it = md->begin();
        QModelIndex top = it.index();
        if (top.parent() == selection.first().parent() && top.row() == selection.first().row()) return;


        FWCmdRuleMove* cmd =
            new FWCmdRuleMove(project, md->getRuleSet(),
                              md->nodeFromIndex(selection.first())->rule->getId(),
                              md->nodeFromIndex(selection.last())->rule->getId());
        project->undoStack->push(cmd);
    }
}

void RuleSetView::moveRuleDown()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOneLevelRules(selection))
    {
        RuleSetModelIterator it = md->end();
        --it;
        QModelIndex bottom = it.index();
        if (bottom.parent() == selection.last().parent() && bottom.row() == selection.last().row()) return;

        FWCmdRuleMove* cmd =
            new FWCmdRuleMove(project, md->getRuleSet(),
                              md->nodeFromIndex(selection.first())->rule->getId(),
                              md->nodeFromIndex(selection.last())->rule->getId(), false);
        project->undoStack->push(cmd);
    }
}

bool RuleSetView::isOnlyTopLevelRules(const QModelIndexList &list) const
{
    foreach (QModelIndex index, list)
    {
        if (!index.isValid()) return false;
        RuleNode* node = static_cast<RuleNode *>(index.internalPointer());

        if (node==0 || node->type != RuleNode::Rule || node->parent->type != RuleNode::Root)
            return false;
    }

    return true;
}

bool RuleSetView::isOneLevelRules(const QModelIndexList &list)
{
    RuleNode *parent = 0;
    foreach (QModelIndex index, list)
    {
        if (!index.isValid()) return false;
        RuleNode* node = static_cast<RuleNode *>(index.internalPointer());

        if (node==0 || node->type != RuleNode::Rule)
            return false;

        if (parent == 0)
            parent = node->parent;
        else
            if (parent != node->parent)
                return false;

    }

    return true;
}

void RuleSetView::copyRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    QModelIndexList selection = getSelectedRows();

    if ( !selection.isEmpty() )
    {
        FWObjectClipboard::obj_clipboard->clear();
        foreach (QModelIndex index, selection)
        {
            RuleNode *node = md->nodeFromIndex(index);
            if (node->type != RuleNode::Rule) continue;
            FWObject *rule = node->rule;

            if (rule) FWObjectClipboard::obj_clipboard->add( rule, project );
        }
    }
}

void RuleSetView::cutRule()
{
    copyRule();
    removeRule();
}

void RuleSetView::pasteRuleAbove()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndexList selection = getSelectedRows();
    QModelIndex index = currentIndex();

    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        Rule *rule = Rule::cast(createInsertTemplate(i->second, i->first));
        if (!rule || !md->checkRuleType(rule)) continue;
        project->undoStack->push(
            new FWCmdRuleInsert(
                project, md->getRuleSet(), md->getRulePosition(index), false, rule));
    }
}

void RuleSetView::pasteRuleBelow()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = currentIndex();

    vector<std::pair<int,ProjectPanel*> >::reverse_iterator i;
    for (i= FWObjectClipboard::obj_clipboard->rbegin();
         i!=FWObjectClipboard::obj_clipboard->rend(); ++i)
    {
        Rule *rule = Rule::cast(createInsertTemplate(i->second, i->first));

        if (!rule || !md->checkRuleType(rule)) continue;

        project->undoStack->push(
            new FWCmdRuleInsert(
                project, md->getRuleSet(), md->getRulePosition(index), true, rule));
    }
}

FWObject* RuleSetView::createInsertTemplate(ProjectPanel* proj_p, int id)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    FWObject* co = proj_p->db()->findInIndex(id);
    FWObject* t = 0;

    if (!Rule::cast(co)) return 0;

    if (proj_p!=project)
    {
        // rule is being copied from another project file
        map<int,int> map_ids;
        t = project->db()->recursivelyCopySubtree(md->getRuleSet(), co, map_ids);
        // Note that FWObjectDatabase::recursivelyCopySubtree adds
        // a copy it creates to the end of the list of children of
        // the object passed as its first arg., which is in this
        // case ruleset. This works only if we paste rule at the
        // bottom of ruleset, otherwise need to move them to the
        // proper location.
        t->ref();
        md->getRuleSet()->remove(t);

        project->m_panel->om->reload();

    } else
    {
        t = proj_p->db()->create(co->getTypeName());
        t->duplicate(co);
    }

    if (fwbdebug)
    {
        cerr << "rulesrt->getRoot()=" << md->getRuleSet()->getRoot()
             << " "
             << "proj_p->db()=" << proj_p->db()
             << " "
             << "proj_p file=" << proj_p->getFileName().toStdString()
             << " " 
             << "id=" << id
             << " " 
             << "co=" << co
             << " " 
             << "co->getRoot()=" << co->getRoot()
             << endl;

        cerr << "Validating database index" << endl;
        proj_p->db()->getRoot()->validateIndex();
    }

    return t;
}

bool RuleSetView::canChange(RuleSetModel* md)
{
    if(!isTreeReadWrite(this,md->getRuleSet())) return false;
    if (md->getFirewall()==nullptr) return false;
    return true;
}

void RuleSetView::insertRule(QModelIndex index, bool isAfter)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;
    project->undoStack->push(
        new FWCmdRuleInsert(
            project, md->getRuleSet(), md->getRulePosition(index), isAfter));
}

void RuleSetView::insertRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndexList selection = getSelectedRows();

    QModelIndex index;
    if (!selection.isEmpty())
    {
        index = selection.first();
    }

    insertRule(index);
}

void RuleSetView::addRuleAfterCurrent()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndexList selection = getSelectedRows();

    if (selection.isEmpty())
    {
        insertRule(QModelIndex());
    }
    else
    {
        QModelIndex lastSelectedIndex = selection.last();
        insertRule(lastSelectedIndex, true);
    }
}

void RuleSetView::insertNewRuleOnTop()
{
    insertRule(QModelIndex());
}

void RuleSetView::insertNewRuleAtBottom()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    RuleSetModelIterator it = md->end();
    --it;
    Rule* posRule = md->nodeFromIndex(it.index())->rule;

    project->undoStack->push(
        new FWCmdRuleInsert(
            project, md->getRuleSet(), posRule->getPosition(), true));
}

void RuleSetView::removeFromGroup()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==nullptr) return;

    QModelIndexList selection = getSelectedRows();

    QMap<QModelIndex,QList<int> > itemsInGroups;

    // Get all rules sorted by groups
    foreach (QModelIndex index, selection)
    {
        if (!index.isValid() || !md->isIndexRule(index)) continue;

        QModelIndex parent = index.parent();
        if (parent.isValid())
        {
            itemsInGroups[parent] << index.row();
        }

    }

    // Remove groups from the end to the begin

    QList<QModelIndex> groups = itemsInGroups.keys();
    qSort(groups);

    QListIterator<QModelIndex> i(groups);
    i.toBack();
    while (i.hasPrevious())
    {
        QModelIndex group = i.previous();
        qSort(itemsInGroups[group]);
        QModelIndex first = md->index(itemsInGroups[group].first(), 0, group);
        QModelIndex last = md->index(itemsInGroups[group].last(), 0, group);

        FWCmdRuleRemoveFromGroup *cmd =
            new FWCmdRuleRemoveFromGroup(project, md->getRuleSet(),
                                         md->nodeFromIndex(first)->rule,
                                         md->nodeFromIndex(last)->rule,
                                         md->nodeFromIndex(group)->name);
        project->undoStack->push(cmd);
//        md->removeFromGroup(group, itemsInGroups[group].first(), itemsInGroups[group].last());
    }

//    QCoreApplication::postEvent(
//        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
}

FWObject *RuleSetView::getObject(const QPoint &pos, const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return 0;

    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node->type == RuleNode::Group) return 0;

    QRect vrect = visualRect(index);

    if (!vrect.isValid()) return 0;

    const int relativeY = pos.y() - vrect.top();
    if (relativeY < 0 || relativeY > vrect.height()) return 0;

    const int itemHeight = RuleSetViewDelegate::getItemHeight();

    RuleElement *re = getRE(index);
    if (re==nullptr) return 0;

    int   oy=0;

    FWObject *o1=nullptr;
    FWObject *obj=nullptr;
    FWObject *prev=nullptr;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=nullptr) o1=FWReference::cast(o1)->getPointer();
        if (relativeY>oy && relativeY<oy+itemHeight)
        {
            obj=o1;
            break;
        }
        oy+=itemHeight;

        prev=o1;
    }
    if (obj==nullptr) obj=prev;
    return obj;
}

FWObject *RuleSetView::getObject(int number, const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return 0;

    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node->type == RuleNode::Group) return 0;

    RuleElement *re = getRE(index);
    if (re==nullptr) return 0;

    int n=1;

    FWObject *o1=nullptr;
    FWObject *obj=nullptr;
    FWObject *prev=nullptr;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=nullptr) o1=FWReference::cast(o1)->getPointer();
        if (n == number)
        {
            obj=o1;
            break;
        }
        n++;

        prev=o1;
    }
    if (obj==nullptr) obj=prev;
    return obj;
}

int RuleSetView::getObjectNumber(FWObject *object, const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return 0;

    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node->type == RuleNode::Group) return 0;

    RuleElement *re = getRE(index);
    if (re==nullptr) return 0;

    int   n=1;

    FWObject *o1=nullptr;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=nullptr) o1=FWReference::cast(o1)->getPointer();
        if (object == o1) break;

        n++;
    }

    return n;
}

void RuleSetView::selectObject(int position, int column, int number)
{
    RuleSetModel* md = ((RuleSetModel*)model());

    QModelIndex index = md->indexForPosition(position);

    if (index.isValid())
    {
        index = md->index(index.row(), column, index.parent());
        FWObject* obj = getObject(number, index);
        selectObject(obj, index);
    } else
    {
        unselect();
    }
}

void RuleSetView::selectObject(FWObject *object, const QModelIndex &index)
{
    fwosm->setSelected(object, index);
    setCurrentIndex(index);
    viewport()->update((viewport()->rect()));
}

void RuleSetView::changeDirectionToIn()
{
    changeDitection( PolicyRule::Inbound );
}

void RuleSetView::changeDirectionToOut()
{
    changeDitection( PolicyRule::Outbound );
}

void RuleSetView::changeDirectionToBoth()
{
    changeDitection( PolicyRule::Both );
}

void RuleSetView::changeDitection(PolicyRule::Direction dir)
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==nullptr) return;

    QModelIndex index = currentIndex();

    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);

    if (node->type != RuleNode::Rule) return;

    PolicyRule *rule = PolicyRule::cast( node->rule );
    PolicyRule::Direction old_dir=rule->getDirection();
    if (dir!=old_dir)
    {
        FWCmdRuleChange* cmd =
            new FWCmdRuleChange(
                project,  md->getRuleSet(), rule, tr("Change direction"));
        PolicyRule* newState = PolicyRule::cast(cmd->getNewState());
        newState->setDirection( dir );
        project->undoStack->push(cmd);
    }
}

void RuleSetView::changeAction(int act)
{
    if (fwbdebug)
        qDebug() << "RuleSetView::changeAction act=" << act;

    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==nullptr) return;

    QModelIndex index = currentIndex();

    if (!index.isValid()) return;

    RuleNode *node = md->nodeFromIndex(index);

    if (node->type != RuleNode::Rule) return;

    std::unique_ptr<FWCmdRuleChange> cmd(
        new FWCmdRuleChange(project,
                            md->getRuleSet(), node->rule, tr("Change action")));
    
    Rule* newRule = dynamic_cast<Rule*>(cmd->getNewState());
    
    if (PolicyRule::isA(newRule))
    {
        PolicyRule *rule = PolicyRule::cast( newRule );
        FWOptions *ruleopt = rule->getOptionsObject();
        PolicyRule::Action old_act=rule->getAction();

        if (fwbdebug)
            qDebug() << "PolicyRule  old_action=" << old_act;

        if (act!=old_act)
        {
            rule->setAction(PolicyRule::Action(act));
            ruleopt->setBool("stateless", getStatelessFlagForAction(rule));
            project->undoStack->push(cmd.release());
        }

    } else if (NATRule::isA(newRule))
    {
        NATRule *rule = NATRule::cast( newRule );
        NATRule::NATAction old_act = rule->getAction();

        if (fwbdebug)
            qDebug() << "NATRule  old_action=" << old_act
                     << "NATRule::Translate=" << NATRule::Translate
                     << "NATRule::Branch=" << NATRule::Branch;

        if (act!=old_act)
        {
            rule->setAction(NATRule::NATAction(act));            
            project->undoStack->push(cmd.release());
        }
    }


    // See #957. It makes sense to open action in the edtor only
    // if this action has some parameters to edit.
    FWObject *fw = node->rule;
    while (fw && Firewall::cast(fw)==nullptr) fw = fw->getParent();
    if (fw)
    {
        QString editor =
            DialogFactory::getActionDialogPageName(
                Firewall::cast(fw), node->rule).c_str();
        editor = editor.toLower();
        // open action in the editor if the editor is already visible
        // or if it is not, only if there is something to edit in this
        // action
        if (mw->isEditorVisible() ||
            (!editor.isEmpty() && editor != "none"))
            QCoreApplication::postEvent(
                mw, new openOptObjectInEditorEvent(
                    project->getFileName(), node->rule->getId(),
                    ObjectEditor::optAction));
    }
}

void RuleSetView::changeActionToAccept()
{
    changeAction( PolicyRule::Accept );
}

void RuleSetView::changeActionToDeny()
{
    changeAction( PolicyRule::Deny );
}

void RuleSetView::changeActionToReject()
{
    changeAction( PolicyRule::Reject );
}

void RuleSetView::changeActionToAccounting()
{
    changeAction( PolicyRule::Accounting );
}

void RuleSetView::changeActionToPipe()
{
    changeAction( PolicyRule::Pipe );
}

void RuleSetView::changeActionToCustom()
{
    changeAction( PolicyRule::Custom );
}

void RuleSetView::changeActionToContinue()
{
    changeAction( PolicyRule::Continue );
}

void RuleSetView::changeActionToBranch()
{
    changeAction( PolicyRule::Branch );
}

void RuleSetView::changeActionToTranslate()
{
    changeAction( NATRule::Translate );
}

void RuleSetView::changeActionToNATBranch()
{
    changeAction( NATRule::Branch );
}


void RuleSetView::changeLogToOn()
{
    changeLogging(true);
}

void RuleSetView::changeLogToOff()
{
    changeLogging(false);
}

void RuleSetView::changeLogging(bool flag)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);
    if (node->type != RuleNode::Rule) return;

    PolicyRule *rule = PolicyRule::cast( node->rule );

    if (rule->getLogging() == flag) return;

    FWCmdRuleChange* cmd = new FWCmdRuleChange(
        project,  md->getRuleSet(), rule, tr("Change logging"));
    PolicyRule* newState = PolicyRule::cast(cmd->getNewState());

    newState->setLogging( flag );

    project->undoStack->push(cmd);
}

void RuleSetView::negateRE()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);
    if (node->type != RuleNode::Rule) return;

    RuleElement *re = getRE(index);
    if (re==nullptr) return;

    int position = Rule::cast(re->getParent())->getPosition();
    int column = index.column();

    project->undoStack->push(
        new FWCmdRuleNegateRE(project, md->getRuleSet(), re, position, column));
}

void RuleSetView::revealObjectInTree()
{
    FWObject* selectedObject = fwosm->selectedObject;
    if (selectedObject!=nullptr)
        QCoreApplication::postEvent(
            mw, new showObjectInTreeEvent(selectedObject->getRoot()->getFileName().c_str(),
                                          selectedObject->getId()));
}

void RuleSetView::findWhereUsedSlot()
{
    if ( fwosm->selectedObject!=nullptr)
        mw->findWhereUsed(fwosm->selectedObject, project);
}

void RuleSetView::deleteSelectedObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;

    if ( fwosm->selectedObject!=nullptr)
    {
        deleteObject(index, fwosm->selectedObject, tr("delete ")+QString::fromUtf8(fwosm->selectedObject->getName().c_str()));
    }
}

void RuleSetView::copySelectedObject()
{
    if ( fwosm->selectedObject!=nullptr)
    {
        FWObjectClipboard::obj_clipboard->clear();
        FWObject *obj =  fwosm->selectedObject;
        FWObjectClipboard::obj_clipboard->add(obj, project );
        mw->showStatusBarMessage(
            tr("Copy object '%1' to clipboard'").arg(
                QString::fromUtf8(obj->getName().c_str())));
    }
}

void RuleSetView::cutSelectedObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    if ( fwosm->selectedObject!=nullptr)
    {
        QModelIndex index = currentIndex();
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( fwosm->selectedObject, project );

        deleteObject(index, fwosm->selectedObject, tr("cut ") + QString::fromUtf8(fwosm->selectedObject->getName().c_str()));
    }
}

void RuleSetView::pasteObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co= proj_p->db()->findInIndex(i->first);
        if (Rule::cast(co)!=nullptr)  pasteRuleAbove();
        else
        {
            // object in the clipboard is not a rule
            QModelIndex index = currentIndex();
            if (index.isValid())
            {
                RuleNode *node = md->nodeFromIndex(index);
                if (node->type != RuleNode::Rule) return;
                copyAndInsertObject(index, co);
            }
        }
    }
}

void RuleSetView::convertToAny()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;

    FWObject *obj = fwosm->selectedObject;

    if (obj!=nullptr)
    {
        QString text = tr("convert to Any from ")+QString::fromUtf8(fwosm->selectedObject->getName().c_str());

        RuleElement *re = (RuleElement *)index.data(Qt::DisplayRole).value<void *>();

        int position = Rule::cast(re->getParent())->getPosition();
        int column = index.column();
        int number = getObjectNumber(obj, index);

        FWCmdRuleChangeRe* cmd = new  FWCmdRuleChangeRe(
                    project, ((RuleSetModel*)model())->getRuleSet(), re, position, column, number,
                    text, 0);
        RuleElement *newRe = RuleElement::cast(cmd->getNewState());
        newRe->removeRef(obj);
        if (newRe->isAny()) newRe->setNeg(false);
        project->undoStack->push(cmd);
    }
}

void RuleSetView::dragEnterEvent( QDragEnterEvent *ev)
{
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void RuleSetView::dropEvent(QDropEvent *ev)
{
    // only accept drops from the same instance of fwbuilder
    if (ev->source() == nullptr) return;

    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndex index = indexAt (ev->pos());
    if (!index.isValid()) return;

    list<FWObject*> dragol;
    if (!FWObjectDrag::decode(ev, dragol)) return;

    for (list<FWObject*>::iterator i=dragol.begin(); i!=dragol.end(); ++i)
    {
        FWObject *dragobj = *i;
        assert(dragobj!=nullptr);

        if (fwbdebug) qDebug("RuleSetView::dropEvent dragobj=%s",
                             dragobj->getName().c_str());

        if (!validateForInsertion(index, dragobj)) continue;

        if (ev->source()!=this)
        {
            // since ev->source()!=this, this is d&d of an object from
            // the tree into rule or from another file.

            copyAndInsertObject(index, dragobj);

        } else
        {
            // since ev->source()==this, this is d&d of an object from
            // one rule to another.

            clearSelection();

            if (ev->keyboardModifiers() & Qt::ControlModifier)
            {
                insertObject(
                    index, dragobj,
                    "copy-drop "+QString::fromUtf8(dragobj->getName().c_str()));
            }
            else //move
            {
                QModelIndex srcIndex = fwosm->index;
                // When object is dragged (moved) from one RE to
                // another, this should appear as single undo
                // command. Also, we should delete it first and insert
                // later so that we leave rule set view with the row
                // where it was inserted selected.

                FWCmdMacro* macro = new  FWCmdMacro(tr("Move object"));

                deleteObject(
                    srcIndex, dragobj,
                    "move-delete "+QString::fromUtf8(dragobj->getName().c_str()),
                    macro);
                insertObject(
                    index, dragobj,
                    "move-drop "+QString::fromUtf8(dragobj->getName().c_str()),
                    macro);

                project->undoStack->push(macro);
            }
        }
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));

    setCurrentIndex(index);
    ev->accept();
}



bool RuleSetView::useDummy(RuleElement *re) const
{
    if (re->getTypeName() == "Src")
        return (st->getInt("Objects/PolicyRule/defaultSource"));
    if (re->getTypeName() == "Dst")
        return (st->getInt("Objects/PolicyRule/defaultDestination"));
    if (re->getTypeName() == "Srv")
        return (st->getInt("Objects/PolicyRule/defaultService"));
    if (re->getTypeName() == "Itf")
        return (st->getInt("Objects/PolicyRule/defaultInterface"));
    return false;
}

void RuleSetView::setDummy(RuleElement *re)
{
    if (re->getDummyElementId() > 0)
        re->addRef(re->getRoot()->findInIndex(re->getDummyElementId()));
}

bool RuleSetView::isDummy(RuleElement *re, int child) const
{
    child--; //Convert from 1-index based to 0-index based
    if (child < 0) return false;
    if (!re) return false;

    FWObject::const_iterator i1 = re->begin();
    if (i1 == re->end()) return false;

    while (child--)
        if (i1++ == re->end())
            return false;

    if (FWReference *ref = FWReference::cast(*i1))
        if (ref->getPointerId() == re->getDummyElementId())
            return true;

    return false;
}

void RuleSetView::deleteObject(QModelIndex index, libfwbuilder::FWObject *obj, QString text, QUndoCommand* macro)
{

    RuleElement *re = (RuleElement *)index.data(Qt::DisplayRole).value<void *>();

    //if (re==nullptr || re->isAny()) return;
    if (re==nullptr) return;
    if (re->isAny() && !useDummy(re)) return;

    int position = Rule::cast(re->getParent())->getPosition();
    int column = index.column();
    int number = getObjectNumber(obj, index);

    if (isDummy(re, number) && useDummy(re)) return;
    FWCmdRuleChangeRe* cmd = new  FWCmdRuleChangeRe(
        project, ((RuleSetModel*)model())->getRuleSet(), re, position, column, number,
        text, macro);
    RuleElement *newRe = RuleElement::cast(cmd->getNewState());
    newRe->removeRef(obj);
    if (newRe->isAny()) newRe->setNeg(false);

    if (newRe->isAny() && useDummy(newRe))
        setDummy(newRe);

    if (macro == 0)
        project->undoStack->push(cmd);
    }

bool RuleSetView::insertObject(QModelIndex index, FWObject *obj, QString text, QUndoCommand* macro)
{
    RuleElement *re = (RuleElement *)index.data(Qt::DisplayRole).value<void *>();
    assert (re!=nullptr);

    int position = Rule::cast(re->getParent())->getPosition();
    int column = index.column();
    int number = getObjectNumber(obj, index);

    FWCmdRuleChangeRe* cmd = new FWCmdRuleChangeRe(
        project, ((RuleSetModel*)model())->getRuleSet(), re, position, column, number,
        text, macro);
    RuleElement *newRe = RuleElement::cast(cmd->getNewState());
    newRe->addRef(obj);

    newRe->removeRef(newRe->getRoot()->findInIndex(newRe->getDummyElementId()));

    if (macro == 0)
        project->undoStack->push(cmd);
    return true;
}

/* RuleElementItd::validateChild() accepts any Interface object.  We
 * should apply additional restriction though: only interface of the
 * same firewall should be allowed. It turns out to be very hard to
 * implement this restriction in RuleElementItd::validateChild()
 * because when the operation is performed via redo(), the
 * RuleElementItf object we have to use is not part of the tree and
 * therefore does not have any parent firewall to compare with.
 */
bool RuleSetView::validateForInsertionToInterfaceRE(RuleElementItf *re,
                                                    FWObject *obj)
{
    return re->validateChild(obj) && re->checkItfChildOfThisFw(obj);
}

bool RuleSetView::validateForInsertion(QModelIndex index, FWObject *obj)
{
    ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();
    if (colDesc.type != ColDesc::Object && colDesc.type != ColDesc::Time)
        return false;

    RuleElement *re = (RuleElement *)index.data(Qt::DisplayRole).value<void *>();
    assert (re!=nullptr);
    return validateForInsertion(re, obj);
}

bool RuleSetView::validateForInsertion(RuleElement *re, FWObject *obj, bool quiet)
{
    if (RuleSet::cast(obj)!=nullptr) return false;

    // see #1976 do not allow pasting object that has been deleted
    if (obj && obj->getLibrary()->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
        return false;

    if (! re->validateChild(obj) )
    {
        if (!quiet)
        {
            if (RuleElementRItf::cast(re))
            {
                QMessageBox::information(
                    nullptr , "Firewall Builder",
                    QObject::tr(
                        "A single interface belonging to this firewall is "
                        "expected in this field."),
                    QString::null,QString::null);
            }
            else if (RuleElementRGtw::cast(re))
            {
                QMessageBox::information(
                    nullptr , "Firewall Builder",
                    QObject::tr(
                        "A single ip adress is expected here. You may also "
                        "insert a host or a network adapter leading to a single "
                        "ip adress."),
                    QString::null,QString::null);
            }
        }
        return false;
    }

    if (re->getAnyElementId()==obj->getId()) return false;
    if (re->getDummyElementId()==obj->getId()) return false;

    if ( !re->isAny())
    {
        /* avoid duplicates */
        int cp_id = obj->getId();
        list<FWObject*>::iterator j;
        for(j=re->begin(); j!=re->end(); ++j)
        {
            FWObject *o=*j;
            if(cp_id==o->getId()) return false;

            FWReference *ref;
            if( (ref=FWReference::cast(o))!=nullptr &&
                 cp_id==ref->getPointerId()) return false;
        }
    }

    // This includes RuleElementItfInb and RuleElementItfOutb of nat rules
    if (RuleElementItf::cast(re) || RuleElementRItf::cast(re))
        return validateForInsertionToInterfaceRE(RuleElementItf::cast(re), obj);

    return true;
}

/* Call validateForInsertion() before calling this function to make
 * sure @object can be inserted in the RE the @index points to.
 */ 
void RuleSetView::copyAndInsertObject(QModelIndex &index, FWObject *object)
{
    if (!validateForInsertion(index, object)) return;

    RuleSetModel* md = ((RuleSetModel*)model());
    bool need_to_reload_tree = false;
    if (md->getRuleSet()->getRoot()!=object->getRoot())
    {
        // object is being copied from another project file
        FWObject *target = FWBTree().getStandardSlotForObject(
            md->getRuleSet()->getLibrary(), object->getTypeName().c_str());
        map<int,int> map_ids;
        object = project->db()->recursivelyCopySubtree(target, object, map_ids);
        need_to_reload_tree = true;
    }

    insertObject(
        index, object,
        "insert "+QString::fromUtf8(object->getName().c_str()));

    if (need_to_reload_tree)
    {
        project->m_panel->om->reload();
        project->m_panel->om->openObjectInTree(object);
        // but still need to reopen this ruleset
        project->m_panel->om->openObjectInTree(md->getRuleSet());
    }
}


void RuleSetView::dragMoveEvent( QDragMoveEvent *ev)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    QWidget *fromWidget = qobject_cast<QWidget*>(ev->source());

    // The source of DnD object must be the same instance of fwbuilder
    if (fromWidget)
    {
        if (ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) && !md->getRuleSet()->isReadOnly())
        {
            if (ev->keyboardModifiers() & Qt::ControlModifier)
                ev->setDropAction(Qt::CopyAction);
            else
                ev->setDropAction(Qt::MoveAction);

            QModelIndex index = indexAt (ev->pos());
            ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();

            if (index.column()<0 || ( colDesc.type != ColDesc::Object && colDesc.type != ColDesc::Time) )
            {
                ev->setAccepted(false);
                return;
            }

            RuleElement *re = getRE(index);
            if (re==nullptr)
            {
                ev->setAccepted(false);
                return;
            }

            bool  acceptE = true;
            list<FWObject*> dragol;

            if (FWObjectDrag::decode(ev, dragol))
            {
                for (list<FWObject*>::iterator i=dragol.begin();
                     i!=dragol.end(); ++i)
                {
                    FWObject *dragobj = nullptr;
                    dragobj = dynamic_cast<FWObject*>(*i);
                    if(dragobj!=nullptr)
                    {
                        acceptE &= validateForInsertion(re, dragobj, true);
                    }
                }
                ev->setAccepted( acceptE );
                return;
            }
        }
    }

    ev->setAccepted(false);
}

void RuleSetView::unselect()
{
    clearSelection();
    setCurrentIndex(QModelIndex());
    fwosm->setSelected(nullptr, QModelIndex());
}

FWObject* RuleSetView::getSelectedObject()
{
    return fwosm->selectedObject;
}

void RuleSetView::saveCurrentRowColumn(SelectionMemento &memento)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    QModelIndex index = fwosm->index;
    if (index.isValid())
    {
        RuleNode* node = md->nodeFromIndex(index);
        if (node && node->rule)
        {
            memento.column = index.column();
            FWObject *rule = node->rule;
            if (rule)
                memento.rule_id = node->rule->getId();
            else
                memento.rule_id = -1;
            return;
        }
    }
    memento.column = -1;
    memento.rule_id = -1;
}

void RuleSetView::restoreCurrentRowColumn(SelectionMemento &memento)
{
    if (memento.rule_id != -1)
    {
        RuleSetModel* md = ((RuleSetModel*)model());
        Rule *rule = Rule::cast(project->db()->findInIndex(memento.rule_id));
        QModelIndex index = md->index(rule, memento.column);
        selectRE(index);
    }
}

void RuleSetView::updateCurrentCell()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    md->rowChanged(fwosm->index);
    updateColumnSizeForIndex(fwosm->index);
}

/*
 * looks like this method can be called when we print from the command
 * line. Variable project==nullptr at that time.
 */
void RuleSetView::saveCollapsedGroups()
{
    if (project)
    {
        QStringList collapsed_groups;
        QString filename = project->getRCS()->getFileName();
        RuleSetModel* md = ((RuleSetModel*)model());
        QList<QModelIndex> groups;
        md->getGroups(groups);
        foreach (QModelIndex index, groups)
        {
            if (!isExpanded(index))
            {
                RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
                collapsed_groups.push_back(node->name);
            }
        }
        Firewall *f = md->getFirewall();
        if (f)
            st->setCollapsedRuleGroups(
                filename,
                f->getName().c_str(),
                md->getRuleSet()->getName().c_str(),
                collapsed_groups);
    }
}

/*
 * looks like this method can be called when we print from the command
 * line. Variable project==nullptr at that time.
 */
void RuleSetView::restoreCollapsedGroups()
{
    if (project)
    {
        QTime t;
        t.start();
        RuleSetModel* md = ((RuleSetModel*)model());
        QStringList collapsed_groups;
        QString filename;
        filename = project->getRCS()->getFileName();

        if (fwbdebug)
            qDebug("restoreCollapsedGroups begin: %d ms", t.restart());

        Firewall *f = md->getFirewall();
        if (f)
            st->getCollapsedRuleGroups(
                filename,
                f->getName().c_str(),
                md->getRuleSet()->getName().c_str(),
                collapsed_groups);

        if (fwbdebug)
            qDebug("restoreCollapsedGroups getCollapsedRuleGroups: %d ms", t.restart());

        QList<QModelIndex> groups;
        md->getGroups(groups);

        if (fwbdebug)
        {
            qDebug("restoreCollapsedGroups getGroups: %d ms", t.restart());
            qDebug() << "Groups:" << groups.size();
        }

        foreach (QModelIndex index, groups)
        {
            RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
            setExpanded(index,  !collapsed_groups.contains(node->name) );
        }

        if (fwbdebug)
            qDebug("restoreCollapsedGroups foreach setExpanded: %d ms", t.restart());
    }
}

int RuleSetView::rowHeight(const QModelIndex& index) const
{
    return QTreeView::rowHeight(index);
}

void RuleSetView::updateWidget()
{
    updateGeometries();
}

bool RuleSetView::showToolTip(QEvent *event)
{
    if (!st->getObjTooltips()) return true;

    QHelpEvent *he = (QHelpEvent*) event;

    QPoint pos = viewport()->mapFromGlobal(he->globalPos());
    QString toolTip="";

    QModelIndex index = indexAt(pos);
    if (!index.isValid())
    {
        if (st->getBool("UI/AdvancedTooltips")) return false;

        toolTip = QObject::tr(
            "<html>"
            "Policy, NAT and routing rules are shown here. "
            "<ul><li><b>Rules use objects</b>, if you want "
            "to use an object like IP address in a rule, "
            "you need to first create it in the object tree</li>"
            "<li><b>Drag and drop</b> objects from the tree to "
            "the desired field (source, destination, etc.) in "
            "the rule.</li>"
            "<li><b>To add a rule</b>, click the '+' button at "
            "the top of the window</li>"
            "<li><b>To open menu of operations</b> such as "
            "'add rule', 'remove rule' etc, click right mouse button</li>"
            "</ul></html>");

        QToolTip::showText(mapToGlobal( he->pos() ), toolTip, this);
        return true;
    }

    RuleSetModel* md = ((RuleSetModel*)model());
    RuleNode *node = md->nodeFromIndex(index);
    int column = index.column();

    if (node->type == RuleNode::Rule)
    {
        QVariant v = index.data(Qt::DisplayRole);
        ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();

        if (column == 0)
        {
            if (st->getBool("UI/AdvancedTooltips")) return false;

            // rule number column
            toolTip = QObject::tr(
                "<html>"
                "<b>To open menu of operations</b> such as 'add rule', "
                "'remove rule' etc, click right mouse button.<br> "
                "<b>To compile the rule</b> and see generated firewall "
                "configuration, first select it by clicking inside of it "
                "and then hit 'X' on keyboard html>");
            QToolTip::showText(mapToGlobal( he->pos() ), toolTip, this);
            return true;
        } else
        {
            switch (colDesc.type)
            {
            case ColDesc::Comment:
                if (!st->getClipComment()) return false;
                toolTip = v.value<QString>();
                break;

            case ColDesc::Options:
            {
                Rule* rule = node->rule;
                if (PolicyRule::cast(rule)!=nullptr )
                {
//                    if (!isDefaultPolicyRuleOptions(rule->getOptionsObject()))
                        toolTip =
                            FWObjectPropertiesFactory::getPolicyRuleOptions(rule);
                }
                if (NATRule::cast(rule)!=nullptr )
                {
//                    if (!isDefaultNATRuleOptions( rule->getOptionsObject()))
                        toolTip =
                            FWObjectPropertiesFactory::getNATRuleOptions(rule);
                }
            }
            break;

            case ColDesc::Direction:
                if (st->getBool("UI/AdvancedTooltips")) 
                {
                    toolTip = QObject::tr("<b>Direction:</b> %1 <br>")
                        .arg(v.value<QString>());
                } else
                {
                    toolTip = QObject::tr("<b>Direction:</b> %1 <br>"
                                          "<b>To change the direction</b>, "
                                          "click right mouse button to open "
                                          "the list of possible settings")
                        .arg(v.value<QString>());
                }
                break;

            case ColDesc::Action:
                if (st->getBool("UI/AdvancedTooltips")) 
                {
                    toolTip = v.value<ActionDesc>().tooltip;
                } else
                {
                    toolTip = QObject::tr("%1 <b>To change the action</b>, "
                                          "click right mouse button to open "
                                          "the list of possible settings")
                        .arg(v.value<ActionDesc>().tooltip);
                }
                break;

            default:
                FWObject *object = getObject(pos, index);
                if (object == 0) return true;
                toolTip = FWObjectPropertiesFactory::getObjectPropertiesDetailed(
                    object, true, true);

                if (st->getBool("UI/AdvancedTooltips"))
                {
                    if (object->getId() == FWObjectDatabase::ANY_ADDRESS_ID ||
                        object->getId() == FWObjectDatabase::ANY_SERVICE_ID ||
                        object->getId() == FWObjectDatabase::ANY_INTERVAL_ID)
                        return false;
                }
            }
        }
    }
    else
    {
        toolTip = node->name;
    }

    if (toolTip.isEmpty())
    {
        QToolTip::hideText();
        return true;
    }

    if (fwbdebug)
        qDebug() << "Toolip: " << toolTip;

    QRect cr = visualRect(index);

    cr = QRect(cr.left() - horizontalOffset() - 2,
               cr.top() - verticalOffset() - 2,
               cr.width() + 4,
               cr.height() + 4);

    QRect global = QRect(
        viewport()->mapToGlobal(cr.topLeft()),
        viewport()->mapToGlobal(cr.bottomRight()));

    QToolTip::showText(mapToGlobal( he->pos() ), toolTip, this, global);
    return true;

}

bool RuleSetView::event( QEvent * event )
{
    if (event->type() == QEvent::ToolTip)
    {
        return showToolTip(event);
    }

    return QTreeView::event(event);
}

void RuleSetView::resizeColumns()
{
    header()->resizeSections(QHeaderView::ResizeToContents);
}

void RuleSetView::updateAllColumnsSize()
{
    resizeColumns();
}

void RuleSetView::updateColumnSizeForIndex(QModelIndex index)
{
    ((RuleSetModel*)model())->nodeFromIndex(index)->resetSizes();
    //TODO: update only corresponding column
    resizeColumns();
}

void RuleSetView::updateSectionSizesForIndex(QModelIndex idx1, QModelIndex idx2)
{
    Q_UNUSED(idx1)
    Q_UNUSED(idx2)

    updateAllColumnsSize();
}

void RuleSetView::setModel(QAbstractItemModel *model)
{
    connect (model, SIGNAL (dataChanged(QModelIndex,QModelIndex)),
             this, SLOT (updateSectionSizesForIndex(QModelIndex,QModelIndex)));
    connect (model, SIGNAL (layoutChanged()),
             this, SLOT (updateAllColumnsSize()));

    QTreeView::setModel(model);

    connect (selectionModel(),
             SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
             this, SLOT(updateSelectionSensitiveActions()));
}

void RuleSetView::repaintSelection()
{
    QModelIndex index = currentIndex();
    fwosm->setSelected(project->getSelectedObject(), index);
    viewport()->update((viewport()->rect()));
}

void RuleSetView::updateAll()
{
    // May be it needs to invalidate all precalculated sizes.
    ((RuleSetModel*)model())->resetAllSizes();

    viewport()->update((viewport()->rect()));
    updateAllColumnsSize();
}

RuleElement* RuleSetView::getRE(QModelIndex index) {
    return (RuleElement *)index.data(Qt::DisplayRole).value<void *>();
}

void RuleSetView::keyPressEvent( QKeyEvent* ev )
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (md->getFirewall()==nullptr) return;

    project->selectRules();

    RuleElement *re;

    QModelIndex oldIndex = fwosm->index;

    int objno = getObjectNumber(fwosm->selectedObject, oldIndex);

    if (ev->key()==Qt::Key_Left || ev->key()==Qt::Key_Right)
    {
        int shift= (ev->key()==Qt::Key_Left) ? -1 : 1;
        int newColumn = oldIndex.column() + shift;
        if ((newColumn <= 0) || (newColumn > md->header.size()))
            return;

/* keyboard 'Left' or 'Right', switch to the object with the same
 * number in the cell to the left or to the right
 */

        QModelIndex newIndex = md->index(oldIndex.row(),
                                         newColumn,
                                         oldIndex.parent());

        re = getRE(newIndex);

        if (re==nullptr)
        {
            fwosm->setSelected(nullptr, newIndex);
            setCurrentIndex(newIndex);
            return;
        }

        FWObject *newObj = getObject(objno, newIndex);

        selectObject(newObj, newIndex);

        return;
    }

    if (ev->key()==Qt::Key_PageDown || ev->key()==Qt::Key_PageUp ||
        ev->key()==Qt::Key_End || ev->key()==Qt::Key_Home)
    {
        QTreeView::keyPressEvent(ev);
        QModelIndex newIndex = md->index(currentIndex().row(),
                                         oldIndex.column(),
                                         currentIndex().parent());

        re = getRE(newIndex);
        FWObject *object = nullptr;
        if (re != nullptr)
        {
            object = FWReference::getObject(re->front());
            selectObject(object, newIndex);
        }
        else
        {
            fwosm->setSelected(nullptr, newIndex);
            setCurrentIndex(newIndex);
        }

        return;
    }

    if (ev->key()==Qt::Key_Down || ev->key()==Qt::Key_Up)
    {
        re = getRE(oldIndex);
        FWObject *object = md->getFirewall();
        QModelIndex newIndex = oldIndex;
        FWObject::iterator i;

        if (re == nullptr && !md->isGroup(oldIndex))
        {
            // Non-object column. Just move focus up or down;
            QTreeView::keyPressEvent(ev);
            newIndex = md->index(currentIndex().row(),
                                 oldIndex.column(),
                                 currentIndex().parent());
            if (!md->isGroup(newIndex)) 
            {
                selectionModel()->select(
                    newIndex,
                    QItemSelectionModel::Rows | QItemSelectionModel::Select);

                setCurrentIndex(newIndex);

                fwosm->setSelected(nullptr, newIndex);

                ev->accept();
            }
            return;
        }
        else
        {
            if (md->isGroup(oldIndex))
            {
                object = nullptr;
            }
            else
            {
                FWObject *prev = nullptr;
                for (i=re->begin(); i!=re->end(); ++i)
                {
                    object = FWReference::getObject(*i);
                    if (ev->key()==Qt::Key_Up   && object==fwosm->selectedObject)
                        break;
                    if (ev->key()==Qt::Key_Down && prev==fwosm->selectedObject)
                        break;
                    prev = object;
                }
                if (ev->key()==Qt::Key_Up) object = prev;
                if (ev->key()==Qt::Key_Down && i == re->end()) object = nullptr;
            }

            if (object == nullptr)
            {
                // It needs to move to another row
                QTreeView::keyPressEvent(ev);
                newIndex = md->index(currentIndex().row(),
                                     oldIndex.column(),
                                     currentIndex().parent());
                selectionModel()->select(
                    newIndex,
                    QItemSelectionModel::Rows | QItemSelectionModel::Select);

                if (oldIndex.row() == newIndex.row())
                {
                    // we are stuck! It's very first or last row.
                    object = fwosm->selectedObject;
                }
                else
                {
                    re = getRE(newIndex);
                    if (re != nullptr)
                    {
                        // NOT a group
                        if (ev->key()==Qt::Key_Up)
                        {
                            i = re->end();
                            --i;
                        }
                        else
                        {
                            i = re->begin();
                        }
                        object = FWReference::getObject(*i);
                    }
                    else
                    {
                        if (!md->isGroup(newIndex))
                        {
                            setCurrentIndex(newIndex);
                            fwosm->setSelected(nullptr, newIndex);

                            ev->accept();
                            return;
                        }
                        object = md->getFirewall();
                    }

                }
            }
            else
            {
                // select other object in current cell
            }
        }

        selectObject(object, newIndex);

        ev->accept();
        return;
    }

    if (ev->key()==Qt::Key_Delete)
    {
        deleteSelectedObject();
    }

    if (ev->key()==Qt::Key_Enter || ev->key()==Qt::Key_Return)
    {
        editSelected();
    }

    QTreeView::keyPressEvent(ev);
}

void RuleSetView::compileCurrentRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    //if (!isTreeReadWrite(this, md->getRuleSet())) return;
    if (md->getFirewall()==nullptr) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node == 0 || node->type != RuleNode::Rule || node->rule == 0) return;

    mw->singleRuleCompile(node->rule);
}

void RuleSetView::updateSelectionSensitiveActions()
{
//    qDebug() << "RuleSetView::updateSelectionSensitiveActions(QItemSelection selected,QItemSelection deselected)";
    RuleSetModel* md = ((RuleSetModel*)model());
    QModelIndexList selectedIndexes = getSelectedRows();

    bool compileRuleActionEnabled = false;

    int selectionSize = selectedIndexes.size();

    if (selectionSize==1)
    {
        QModelIndex index = selectedIndexes.at(0);
        if (index.isValid())
        {
            RuleNode* node = md->nodeFromIndex(index);
            if (node!=0 && node->type == RuleNode::Rule && node->rule != 0)
            {
                compileRuleActionEnabled = !node->rule->isDisabled();
            }
        }
    }

    compileRuleAction->setEnabled(compileRuleActionEnabled);

    if (selectionSize == 0)
    {

        setActionState(removeFromGroupAction, false);
        setActionState(newGroupAction, false);
        setActionState(moveRuleUpAction, false);
        setActionState(moveRuleDownAction, false);
        setActionState(addToGroupAboveAction, false);
        setActionState(addToGroupBelowAction, false);
        setActionState(insertRuleAction, false);
        setActionState(addRuleAfterCurrentAction, false);
        setActionState(removeRuleAction, false);
        setActionState(disableRuleAction, false);
        setActionState(enableRuleAction, false);
        setActionState(copyRuleAction, false);
        setActionState(cutRuleAction, false);
        setActionState(pasteRuleAboveAction, false);
        setActionState(pasteRuleBelowAction, false);

        setColorEmptyAction->setEnabled(false);
        setColorRedAction->setEnabled(false);
        setColorBlueAction->setEnabled(false);
        setColorOrangeAction->setEnabled(false);
        setColorPurpleAction->setEnabled(false);
        setColorGrayAction->setEnabled(false);
        setColorYellowAction->setEnabled(false);
        setColorGreenAction->setEnabled(false);

    } else
    {
        bool inGroup = true;
        bool outermost = false;
        bool topLevelOnly = true;
        int disabled = 0;
        int enabled = 0;

        foreach(QModelIndex index, selectedIndexes)
        {
            if (index.isValid())
            {
                RuleNode* node = md->nodeFromIndex(index);

                topLevelOnly = topLevelOnly && (node->type == RuleNode::Rule);

                if (node!=0 && node->type == RuleNode::Rule && node->rule != 0)
                {
                    bool isInGroup = node->isInGroup();
                    inGroup = inGroup && isInGroup;
                    topLevelOnly = topLevelOnly && !isInGroup;
                    outermost = outermost || (node->isOutermost() && isInGroup);

                    Rule *r =  node->rule;

                    if (r->isDisabled())
                        disabled++;
                    else
                        enabled++;

                }
            }
        }

        setActionState(disableRuleAction, enabled > 0);
        setActionState(enableRuleAction, disabled > 0);

        enableRuleAction->setText(
            (disabled==1)?tr("Enable Rule"):tr("Enable Rules"));
        disableRuleAction->setText(
            (enabled==1)?tr("Disable Rule"):tr("Disable Rules"));

        if (selectionSize > 1)
        {
            copyRuleAction->setText(tr("Copy Rules"));
            cutRuleAction->setText(tr("Cut Rules"));
            moveRuleUpAction->setText(tr("Move Rules Up"));
            moveRuleDownAction->setText(tr("Move Rules Down"));
            removeRuleAction->setText(tr("Remove Rules"));

        } else
        {
            copyRuleAction->setText(tr("Copy Rule"));
            cutRuleAction->setText(tr("Cut Rule"));
            moveRuleUpAction->setText(tr("Move Rule Up"));
            moveRuleDownAction->setText(tr("Move Rule Down"));
            removeRuleAction->setText(tr("Remove Rule"));
        }

        if (topLevelOnly)
        {
            QString addToGroupLabel = tr("Add To the Group ");
            QString nn;

            nn = md->nodeFromIndex(selectedIndexes.first())->nameOfPredecessorGroup();
            if (!nn.isEmpty())
            {
                addToGroupAboveAction->setText(addToGroupLabel + nn);
                setActionState(addToGroupAboveAction, true);
            } else
            {
                setActionState(addToGroupAboveAction, false);
            }

            nn = md->nodeFromIndex(selectedIndexes.last())->nameOfSuccessorGroup();

            if (!nn.isEmpty())
            {
                addToGroupBelowAction->setText(addToGroupLabel + nn);
                setActionState(addToGroupBelowAction, true);
            } else
            {
                setActionState(addToGroupBelowAction, false);
            }

        } else
        {

            setActionState(addToGroupAboveAction, false);

            setActionState(addToGroupBelowAction, false);
        }

        setActionState(removeFromGroupAction, outermost);
        setActionState(newGroupAction, topLevelOnly);
        setActionState(moveRuleUpAction, true);
        setActionState(moveRuleDownAction, true);
        setActionState(insertRuleAction, true);
        setActionState(addRuleAfterCurrentAction, true);
        setActionState(removeRuleAction, true);
        setActionState(copyRuleAction, true);
        setActionState(cutRuleAction, true);
        setActionState(pasteRuleAboveAction, true);
        setActionState(pasteRuleBelowAction, true);

        setColorEmptyAction->setEnabled(true);
        setColorRedAction->setEnabled(true);
        setColorBlueAction->setEnabled(true);
        setColorOrangeAction->setEnabled(true);
        setColorPurpleAction->setEnabled(true);
        setColorGrayAction->setEnabled(true);
        setColorYellowAction->setEnabled(true);
        setColorGreenAction->setEnabled(true);
    }
}

void RuleSetView::updateObject(FWObject* object)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    md->objectChanged(object);
}

void RuleSetView::setActionState(QAction *action, bool state)
{
    action->setEnabled(state);
    action->setVisible(state);
}

////////////////////////////////////////////////////////////////////////////
// PolicyView
////////////////////////////////////////////////////////////////////////////

PolicyView::PolicyView(ProjectPanel *project, Policy *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new PolicyModel(p,this);
    setModel(model);
    delete sm;
    RuleSetViewDelegate *dlgt = new RuleSetViewDelegate(this, fwosm);
    dlgt->setStandardHighlightColor(palette().color(QPalette::Highlight));
    setItemDelegate(dlgt);
    init();
}

////////////////////////////////////////////////////////////////////////////
// NATView
////////////////////////////////////////////////////////////////////////////

NATView::NATView(ProjectPanel *project, NAT *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new NatModel(p,this);
    setModel(model);
    delete sm;
    RuleSetViewDelegate *dlgt = new RuleSetViewDelegate(this, fwosm);
    dlgt->setStandardHighlightColor(palette().color(QPalette::Highlight));
    setItemDelegate(dlgt);
    init();
}

////////////////////////////////////////////////////////////////////////////
// RoutingView
////////////////////////////////////////////////////////////////////////////

RoutingView::RoutingView(ProjectPanel *project, Routing *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new RoutingModel(p,this);
    setModel(model);
    delete sm;
    RuleSetViewDelegate *dlgt = new RuleSetViewDelegate(this, fwosm);
    dlgt->setStandardHighlightColor(palette().color(QPalette::Highlight));
    setItemDelegate(dlgt);
    init();
}
