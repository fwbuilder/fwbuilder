/*

                          Firewall Builder

                 Copyright (C) 2003-2009 NetCitadel, LLC

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


  This module has been completely rewritten by yalovoy@gmail.com in 2009

*/

#include "../../config.h"
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

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"

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

    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    header()->setResizeMode(QHeaderView::Interactive);
    header()->setMovable(false);

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

    compileRuleAction = new QAction(tr("Compile rule"), this);
    compileRuleAction->setShortcut(QKeySequence(Qt::Key_X));

    connect (compileRuleAction, SIGNAL(triggered()),
              this, SLOT(compileCurrentRule()));

    addAction(compileRuleAction );
    popup_menu = new QMenu(this);
}

RuleSetView::~RuleSetView()
{
    if (fwbdebug) qDebug("RuleSetView::~RuleSetView");
    delete fwosm;
    delete compileRuleAction;
}

void RuleSetView::init()
{
    if (fwbdebug) qDebug("RuleSetView::init");

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
    return NULL;
}

void RuleSetView::selectRE(QModelIndex index)
{
    if (fwbdebug)
        qDebug() << "RuleSetView::selectRE(QModelIndex index)";
    if (fwosm->index != index)
    {
        fwosm->selectedObject = NULL;
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
    assert(rule!=NULL);

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
    if (fwbdebug) qDebug() << "RuleSetView::mousePressEvent";

    //TODO: provide custom implementation of QTreeView::mousePressEvent( ev ); for column != 0
    QTreeView::mousePressEvent( ev );

    const QModelIndex index = currentIndex();//indexAt (ev->pos());

    if (index.column() == 0) return;

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
        fwosm->setSelected(NULL, index);
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
            if (column < 1)
            {
                addRowMenuItemsToContextMenu(popup_menu, node);
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

    if (obj==NULL) return NULL;

    // TODO: use FWBTree::setObjectIcon()
    QString icn = (":/Icons/" + obj->getTypeName() + "/icon").c_str();
    list<FWObject*> dragobj;
    dragobj.push_back(obj);
    FWObjectDrag *drag = new FWObjectDrag(dragobj, this, NULL);
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
                    if (Resources::isTargetActionSupported(platform,"Route"))
                    {
                        action_name = getActionNameForPlatform(
                            f, PolicyRule::getActionAsString(PolicyRule::Route));
                        menu->addAction( QIcon(LoadPixmap(":/Icons/Route/icon")),
                                         action_name,
                                         this, SLOT( changeActionToRoute() ));
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
                if (rule!=NULL)
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
                if (re==NULL) return;
                FWObject *object = getObject(pos, index);

                QAction *editID = menu->addAction(
                        tr("Edit")   , this , SLOT( editSelected() ) );
                menu->addSeparator();
                QAction *copyID = menu->addAction(
                        tr("Copy")   , this , SLOT( copySelectedObject() ) );
                QAction *cutID  = menu->addAction(
                        tr("Cut")    , this , SLOT( cutSelectedObject() ) );
                menu->addAction( tr("Paste")  , this , SLOT( pasteObject() ) );

                QAction *delID  =menu->addAction(
                        tr("Delete") ,  this , SLOT( deleteSelectedObject() ) );
                menu->addSeparator();
                QAction *fndID = menu->addAction(
                        tr("Where used") , this , SLOT( findWhereUsedSlot()));
                QAction *revID = menu->addAction(
                        tr("Reveal in tree") ,this , SLOT( revealObjectInTree() ) );
                menu->addSeparator();
                QAction *negID  = menu->addAction(
                        tr("Negate") , this , SLOT( negateRE() ) );

                if (object == NULL || re->isAny())
                    editID->setEnabled(false);
                copyID->setEnabled(!re->isAny());
                cutID->setEnabled(!re->isAny());
                delID->setEnabled(!re->isAny());

                string cap_name;
                if (Policy::cast(md->getRuleSet())!=NULL) cap_name="negation_in_policy";
                if (NAT::cast(md->getRuleSet())!=NULL) cap_name="negation_in_nat";

                Firewall *f = md->getFirewall();

                bool supports_neg=false;
                try  {
                    supports_neg = Resources::getTargetCapabilityBool(
                        f->getStr("platform"), cap_name);
                } catch (FWException &ex)
                {
                    QMessageBox::critical( NULL , "Firewall Builder",
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
        menu->addAction(tr("Insert Rule"), this, SLOT( insertRule() ));
    else
    {
        menu->addAction(tr("Add new rule on top"), this, SLOT( insertNewRuleOnTop()));
        menu->addAction(tr("Add new rule at the bottom"), this, SLOT( insertNewRuleAtBottom()));
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

    QPixmap pcolor(16,16);
    pcolor.fill(QColor(255,255,255));
    subcolor->addAction(QIcon(pcolor), tr("No color"),
                         this, SLOT(setColorEmpty() ));

    pcolor.fill(st->getLabelColor(FWBSettings::RED));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::RED),
                         this, SLOT(setColorRed() ));

    pcolor.fill(st->getLabelColor(FWBSettings::ORANGE));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::ORANGE),
                         this, SLOT(setColorOrange() ));

    pcolor.fill(st->getLabelColor(FWBSettings::YELLOW));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::YELLOW),
                         this, SLOT(setColorYellow() ));

    pcolor.fill(st->getLabelColor(FWBSettings::GREEN));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::GREEN),
                         this, SLOT(setColorGreen() ));

    pcolor.fill(st->getLabelColor(FWBSettings::BLUE));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::BLUE),
                         this, SLOT(setColorBlue() ));

    pcolor.fill(st->getLabelColor(FWBSettings::PURPLE));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::PURPLE),
                         this, SLOT(setColorPurple() ));

    pcolor.fill(st->getLabelColor(FWBSettings::GRAY));
    subcolor->addAction(QIcon(pcolor),
                         st->getLabelText(FWBSettings::GRAY),
                         this, SLOT(setColorGray() ));

}

void RuleSetView::addRowMenuItemsToContextMenu(QMenu *menu, RuleNode* node) const
{
    QString label;
    RuleSetModel* md = ((RuleSetModel*)model());
    QModelIndexList selectedIndexes = getSelectedRows();
    int selectionSize = selectedIndexes.size();

    if (node->isInGroup())
    {
        // only outermost rules in group could removed from this group
        if (node->isOutermost())
            menu->addAction( tr("Remove from the group"), this, SLOT( removeFromGroup() ));
    }
    else if (selectedIndexes.size() > 0 && isOnlyTopLevelRules(selectedIndexes))
    {
        menu->addAction( tr("New group"), this, SLOT( newGroup() ));

        QString nn =
            md->nodeFromIndex(selectedIndexes.first())->nameOfPredecessorGroup();

        if (!nn.isEmpty())
        {
            label = tr("Add to the group ") + nn;
            menu->addAction( label, this, SLOT( addToGroupAbove() ));
        }

        nn = md->nodeFromIndex(selectedIndexes.last())->nameOfSuccessorGroup();

        if (!nn.isEmpty())
        {
            label = tr("Add to the group ") + nn;
            menu->addAction( label, this, SLOT( addToGroupBelow() ));
        }
    }

    menu->addSeparator();

    addChangeColorSubmenu(menu);

    menu->addSeparator();

    menu->addAction( tr("Insert Rule"), this, SLOT( insertRule() ) );
    menu->addAction( tr("Add Rule Below"), this, SLOT( addRuleAfterCurrent() ) );

    label = (selectionSize==1)?tr("Remove Rule"):tr("Remove Rules");
    menu->addAction( label, this, SLOT( removeRule()));
    label = (selectionSize==1)?tr("Move Rule up"):tr("Move Rules up");
    menu->addAction( label, this, SLOT( moveRuleUp()));
    label = (selectionSize==1)?tr("Move Rule down"):tr("Move Rules down");
    menu->addAction( label, this, SLOT( moveRuleDown()));

    menu->addSeparator();

    menu->addAction( tr("Copy Rule"), this, SLOT( copyRule() ) );
    menu->addAction( tr("Cut Rule"), this, SLOT( cutRule() ) );
    menu->addAction( tr("Paste Rule Above"), this, SLOT( pasteRuleAbove() ) );
    menu->addAction( tr("Paste Rule Below"), this, SLOT( pasteRuleBelow() ) );

    menu->addSeparator();

    Rule *r =  node->rule;
    if (r->isDisabled())
    {
        label = (selectionSize==1)?tr("Enable Rule"):tr("Enable Rules");
        menu->addAction( label, this, SLOT( enableRule() ) );
    }else{
        label = (selectionSize==1)?tr("Disable Rule"):tr("Disable Rules");
        menu->addAction( label, this, SLOT( disableRule() ) );
    }

}

void RuleSetView::itemDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) return;
    if (index.row()<0) return;
    if ( fwosm->selectedObject!=NULL)
    {
        QCoreApplication::postEvent(
            mw,
            new showObjectInTreeEvent(project->getFileName(), fwosm->selectedObject->getId()));

        //openObjectInTree(fwosm->selectedObject);
    }
    editSelected(index);
}

void RuleSetView::editSelected(const QModelIndex& index)
{
    if (!mw->isEditorVisible()) mw->showEditor();
    switchObjectInEditor(index);
}

void RuleSetView::editSelected()
{
    editSelected(currentIndex());
}

bool RuleSetView::switchObjectInEditor(const QModelIndex& index, bool validate)
{
//    qDebug() << "RuleSetView::switchObjectInEditor";
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return false;

    if ( index.column()<=0 || index.row()==-1 ) return false;

    FWObject *object = NULL;
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
            if ( fwosm->selectedObject!=NULL)
            {
                object=fwosm->selectedObject;
                break;
            }
        }
    }

    if (!mw->requestEditorOwnership(this, object, operation, validate))
        return false;

    if (object==mw->getOpenedEditor() && operation==mw->getOpenedOptEditor())
    {
        if (fwbdebug)
            qDebug("RuleSetView::switchObjectInEditor  same object is already opened in the editor");
        return true;
    }

    if (object == NULL)
    {
        mw->blankEditor();
    } else if (operation==ObjectEditor::optNone)
    {
        mw->openEditor(object);
    } else if(Rule::cast(object)!=NULL)
    {
        mw->openOptEditor(object, operation);
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

void RuleSetView::removeRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    mw->findObjectWidget->reset();

    QModelIndexList selection = getSelectedRows();

    if (!selection.isEmpty())
    {
        QMap<QModelIndex,QList<int> > itemsInGroups;
        QList<int> topLevelItems;


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
            else
            {
                topLevelItems << index.row();
            }

        }

        // Remove items in groups
        QList<QModelIndex> groups = itemsInGroups.keys();
        if (!groups.isEmpty())
        {
            foreach(QModelIndex group, groups)
            {
                qSort(itemsInGroups[group]);
                for (int i = itemsInGroups[group].size() - 1; i>=0 ; i--)
                {
                    md->removeRow(itemsInGroups[group].at(i),group);
                }

                // if group gets empty it should be removed as well
                if (md->rowCount(group) == 0)
                    topLevelItems << group.row();

            }
        }

        // Remove top level items
        if (!topLevelItems.isEmpty())
        {
            QModelIndex parent;
            qSort(topLevelItems);
            for(int i = topLevelItems.size() - 1; i>=0; i--)
            {
                md->removeRow(topLevelItems.at(i), parent);
            }

        }

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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
        tr("Enter group name:"), QLineEdit::Normal,
        oldGroupName, &ok);

        if (ok && !newGroupName.isEmpty() && newGroupName != oldGroupName)
        {
            md->renameGroup(index, newGroupName);
        }
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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

            FWCmdRuleChange* cmd = new FWCmdRuleChange(project,  md->getRuleSet(), rule, (flag)?tr("enable rule"):tr("disable rule"));
            Rule* newState = Rule::cast(cmd->getNewState());
            if (flag)
                newState->enable();
            else
                newState->disable();
            project->undoStack->push(cmd);
        }
    }
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
            QModelIndex index = md->createNewGroup(newGroupName, selection.first().row(), selection.last().row());

            setFirstColumnSpanned(index.row(), QModelIndex(), true);

            QCoreApplication::postEvent(
                mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
        }
    }
}

void RuleSetView::addToGroupAbove()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOnlyTopLevelRules(selection))
    {

        md->addToGroupAbove(selection.first().row(), selection.last().row());

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
    }
}

void RuleSetView::addToGroupBelow()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    QModelIndexList selection = getSelectedRows();

    // we cannot perform this action if the selection contains groups or rules assigned to groups

    if (!selection.isEmpty() && isOnlyTopLevelRules(selection))
    {

        md->addToGroupBelow(selection.first().row(), selection.last().row());

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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

        md->moveRuleUp(selection.first().parent() , selection.first().row(), selection.last().row());

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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

        md->moveRuleDown(selection.first().parent() , selection.first().row(), selection.last().row());

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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
        ProjectPanel *proj_p = i->second;
        FWObject *co = proj_p->db()->findInIndex(i->first);
        Rule *rule = Rule::cast(co);

        if (!rule || !md->checkRuleType(rule)) continue;

        if (proj_p!=project)
        {
            // rule is being copied from another project file
            map<int,int> map_ids;
            co = project->db()->recursivelyCopySubtree(md->getRuleSet(), co, map_ids);
            // Note that FWObjectDatabase::recursivelyCopySubtree adds
            // a copy it creates to the end of the list of children of
            // the object passed as its first arg., which is in this
            // case ruleset. This works only if we paste rule at the
            // bottom of ruleset, otherwise need to move them to the
            // proper location.
            co->ref();
            md->getRuleSet()->remove(co);

            project->m_panel->om->reload();
        }

        project->undoStack->push(new FWCmdRuleInsert(project, md->getRuleSet(), md->getRulePosition(index), false, Rule::cast(co)));
    }
}

void RuleSetView::pasteRuleBelow()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;

    QModelIndexList selection = getSelectedRows();
    QModelIndex index = currentIndex();

    vector<std::pair<int,ProjectPanel*> >::reverse_iterator i;
    for (i= FWObjectClipboard::obj_clipboard->rbegin();
         i!=FWObjectClipboard::obj_clipboard->rend(); ++i)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co = proj_p->db()->findInIndex(i->first);
        Rule *rule = Rule::cast(co);

        if (!rule || !md->checkRuleType(rule)) continue;

        if (proj_p!=project)
        {
            // rule is being copied from another project file
            map<int,int> map_ids;
            co = project->db()->recursivelyCopySubtree(md->getRuleSet(), co, map_ids);
            // Note that FWObjectDatabase::recursivelyCopySubtree adds
            // a copy it creates to the end of the list of children of
            // the object passed as its first arg., which is in this
            // case ruleset. This works only if we paste rule at the
            // bottom of ruleset, otherwise need to move them to the
            // proper location.
            co->ref();
            md->getRuleSet()->remove(co);

            project->m_panel->om->reload();
        }

        project->undoStack->push(new FWCmdRuleInsert(project, md->getRuleSet(), md->getRulePosition(index), true, Rule::cast(co)));
    }
}

bool RuleSetView::canChange(RuleSetModel* md)
{
    if(!isTreeReadWrite(this,md->getRuleSet())) return false;
    if (md->getFirewall()==NULL) return false;
    return true;
}

void RuleSetView::insertRule(QModelIndex index,bool isAfter) {
    RuleSetModel* md = ((RuleSetModel*)model());
    if (!canChange(md)) return;
    project->undoStack->push(new FWCmdRuleInsert(project, md->getRuleSet(), md->getRulePosition(index), isAfter));
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

    project->undoStack->push(new FWCmdRuleInsert(project, md->getRuleSet(), posRule->getPosition(), true));
}

void RuleSetView::removeFromGroup()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

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
        md->removeFromGroup(group, itemsInGroups[group].first(), itemsInGroups[group].last());
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
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
    if (re==NULL) return 0;

    int   oy=0;

    FWObject *o1=NULL;
    FWObject *obj=NULL;
    FWObject *prev=NULL;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
        if (relativeY>oy && relativeY<oy+itemHeight)
        {
            obj=o1;
            break;
        }
        oy+=itemHeight;

        prev=o1;
    }
    if (obj==NULL) obj=prev;
    return obj;
}

FWObject *RuleSetView::getObject(int number, const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return 0;

    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node->type == RuleNode::Group) return 0;

    RuleElement *re = getRE(index);
    if (re==NULL) return 0;

    int n=1;

    FWObject *o1=NULL;
    FWObject *obj=NULL;
    FWObject *prev=NULL;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
        if (n == number)
        {
            obj=o1;
            break;
        }
        n++;

        prev=o1;
    }
    if (obj==NULL) obj=prev;
    return obj;
}

int RuleSetView::getObjectNumber(FWObject *object, const QModelIndex &index)
{
    if (!index.isValid() || index.column() == 0) return 0;

    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node->type == RuleNode::Group) return 0;

    RuleElement *re = getRE(index);
    if (re==NULL) return 0;

    int   n=1;

    FWObject *o1=NULL;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        o1= *i;
        if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
        if (object == o1) break;

        n++;
    }

    return n;
}

void RuleSetView::selectObject(FWObject *object, const QModelIndex &index)
{
    fwosm->setSelected(object, index);
    setCurrentIndex(index);
    viewport()->update((viewport()->rect()));

    //openObjectInTree(object);
}

void RuleSetView::openObjectInTree(FWObject *obj)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    FWObject *oo = obj;
    if (obj==NULL || Rule::cast(obj)!=NULL)  oo = md->getFirewall();
    if (oo==NULL) return;

    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(QString::fromUtf8(oo->getRoot()->getFileName().c_str()),
                                      oo->getId()));
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
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();

    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);

    if (node->type != RuleNode::Rule) return;

    PolicyRule *rule = PolicyRule::cast( node->rule );
    PolicyRule::Direction old_dir=rule->getDirection();
    if (dir!=old_dir)
    {
        FWCmdRuleChange* cmd = new FWCmdRuleChange(project,  md->getRuleSet(), rule, tr("change direction"));
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
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();

    if (!index.isValid()) return;

    RuleNode *node = md->nodeFromIndex(index);

    if (node->type != RuleNode::Rule) return;

    std::auto_ptr<FWCmdRuleChange> cmd( new FWCmdRuleChange(project,  md->getRuleSet(), node->rule, tr("change action")));
    
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

    QCoreApplication::postEvent(
        mw, new openOptObjectInEditorEvent(
            project->getFileName(), node->rule->getId(), ObjectEditor::optAction));
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

void RuleSetView::changeActionToTag()
{
    changeAction( PolicyRule::Tag );
}

void RuleSetView::changeActionToClassify()
{
    changeAction( PolicyRule::Classify );
}

void RuleSetView::changeActionToCustom()
{
    changeAction( PolicyRule::Custom );
}

void RuleSetView::changeActionToRoute()
{
    changeAction( PolicyRule::Route );
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

    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);
    if (node->type != RuleNode::Rule) return;

    PolicyRule *rule = PolicyRule::cast( node->rule );

    if (rule->getLogging() == flag) return;

    FWCmdRuleChange* cmd = new FWCmdRuleChange(project,  md->getRuleSet(), rule, tr("change logging"));
    PolicyRule* newState = PolicyRule::cast(cmd->getNewState());

    newState->setLogging( flag );

    project->undoStack->push(cmd);
}

void RuleSetView::negateRE()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode *node = md->nodeFromIndex(index);
    if (node->type != RuleNode::Rule) return;

    RuleElement *re = getRE(index);
    if (re==NULL) return;

    re->toggleNeg();

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
}

void RuleSetView::revealObjectInTree()
{
    FWObject* selectedObject = fwosm->selectedObject;
    if (selectedObject!=NULL)
        QCoreApplication::postEvent(
            mw, new showObjectInTreeEvent(selectedObject->getRoot()->getFileName().c_str(),
                                          selectedObject->getId()));
}

void RuleSetView::findWhereUsedSlot()
{
    if ( fwosm->selectedObject!=NULL)
        mw->findWhereUsed(fwosm->selectedObject, project);
}

void RuleSetView::deleteSelectedObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;

    if ( fwosm->selectedObject!=NULL)
    {
        md->deleteObject(index, fwosm->selectedObject);
        fwosm->reset();

        mw->findObjectWidget->reset();

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
    }
}

void RuleSetView::copySelectedObject()
{
    if ( fwosm->selectedObject!=NULL)
    {
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( fwosm->selectedObject, project );
    }
}

void RuleSetView::cutSelectedObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());

    if(!isTreeReadWrite(this,md->getRuleSet())) return;

    if ( fwosm->selectedObject!=NULL)
    {
        QModelIndex index = currentIndex();
        FWObjectClipboard::obj_clipboard->clear();
        FWObjectClipboard::obj_clipboard->add( fwosm->selectedObject, project );
        md->deleteObject(index, fwosm->selectedObject);
    }
}

void RuleSetView::pasteObject()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    vector<std::pair<int,ProjectPanel*> >::iterator i;
    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        ProjectPanel *proj_p = i->second;
        FWObject *co= proj_p->db()->findInIndex(i->first);
        if (Rule::cast(co)!=NULL)  pasteRuleAbove();
        else
        {
            QModelIndex index = currentIndex();
            if (index.isValid())
                copyAndInsertObject(index, co);
        }
    }
    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
}

void RuleSetView::dragEnterEvent( QDragEnterEvent *ev)
{
    ev->setAccepted( ev->mimeData()->hasFormat(FWObjectDrag::FWB_MIME_TYPE) );
}

void RuleSetView::dropEvent(QDropEvent *ev)
{

    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    QModelIndex index = indexAt (ev->pos());
    if (!index.isValid()) return;

    list<FWObject*> dragol;
    if (!FWObjectDrag::decode(ev, dragol)) return;

    for (list<FWObject*>::iterator i=dragol.begin(); i!=dragol.end(); ++i)
    {
        FWObject *dragobj = *i;
        assert(dragobj!=NULL);

        if (fwbdebug) qDebug("RuleSetView::dropEvent dragobj=%s",
                             dragobj->getName().c_str());

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
                md->insertObject(index, dragobj);
            }
            else //move
            {

                if (md->insertObject(index, dragobj) )
                {
                    md->deleteObject(fwosm->index, dragobj);
                }
            }
        }
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));

    setCurrentIndex(index);
    ev->accept();
}

void RuleSetView::copyAndInsertObject(QModelIndex &index, FWObject *object)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    bool need_to_reload_tree = false;
    if (md->getRuleSet()->getRoot()!=object->getRoot())
    {
        // object is being copied from another project file
        FWObject *target = project->getFWTree()->getStandardSlotForObject(
            md->getRuleSet()->getLibrary(), object->getTypeName().c_str());
        map<int,int> map_ids;
        object = project->db()->recursivelyCopySubtree(target, object, map_ids);
        need_to_reload_tree = true;
    }

    md->insertObject(index,object);

    if (need_to_reload_tree)
    {
        project->m_panel->om->reload();
        project->m_panel->om->openObject(object);
        // but still need to reopen this ruleset
        project->m_panel->om->openObject(md->getRuleSet());
    }
}


void RuleSetView::dragMoveEvent( QDragMoveEvent *ev)
{
    RuleSetModel* md = ((RuleSetModel*)model());

    QWidget *fromWidget = ev->source();

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
            if (re==NULL)
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
                    FWObject *dragobj = NULL;
                    dragobj = dynamic_cast<FWObject*>(*i);
                    if(dragobj!=NULL)
                    {
                        acceptE &= re->validateChild(dragobj);
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
    fwosm->setSelected(NULL, QModelIndex());
}

FWObject* RuleSetView::getSelectedObject()
{
    return fwosm->selectedObject;
}

void RuleSetView::saveCurrentRowColumn(SelectionMemento &memento)
{
    QModelIndex index = fwosm->index;

    memento.column = index.column();
    memento.row = index.row();
    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());

    if (node!=NULL && node->parent !=NULL && !node->parent->isRoot())
    {
        memento.groupName = node->parent->name;
    }
}

void RuleSetView::restoreCurrentRowColumn(SelectionMemento &memento)
{
    RuleSetModel* md = ((RuleSetModel*)model());
    QModelIndex index = md->index(memento.row, memento.column, memento.groupName);
    selectRE(index);
}

void RuleSetView::updateCurrentCell()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    md->rowChanged(fwosm->index);
    updateColumnSizeForIndex(fwosm->index);
}

void RuleSetView::saveCollapsedGroups()
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
    st->setCollapsedRuleGroups(
        filename,
        md->getFirewall()->getName().c_str(),
        md->getRuleSet()->getName().c_str(),
        collapsed_groups);
}

void RuleSetView::restoreCollapsedGroups()
{
    QTime t;
    t.start();
    RuleSetModel* md = ((RuleSetModel*)model());
    QStringList collapsed_groups;
    QString filename = project->getRCS()->getFileName();
    if (fwbdebug)
        qDebug("restoreCollapsedGroups begin: %d ms", t.restart());
    st->getCollapsedRuleGroups(
        filename,
        md->getFirewall()->getName().c_str(),
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
    QHelpEvent *he = (QHelpEvent*) event;

    QPoint pos = viewport()->mapFromGlobal(he->globalPos());

    QModelIndex index = indexAt(pos);
    if (!index.isValid()) return false;

    RuleSetModel* md = ((RuleSetModel*)model());
    RuleNode *node = md->nodeFromIndex(index);

    QString toolTip="";

    if (node->type == RuleNode::Rule)
    {
        QVariant v = index.data(Qt::DisplayRole);
        ColDesc colDesc = index.data(Qt::UserRole).value<ColDesc>();

        switch (colDesc.type)
        {
            case ColDesc::Comment:
                if (!st->getClipComment()) return false;
                toolTip = v.value<QString>();
                break;

            case ColDesc::Options:
                {
                    Rule* rule = node->rule;
                    if (PolicyRule::cast(rule)!=NULL )
                    {
                        if (!isDefaultPolicyRuleOptions(rule->getOptionsObject()))
                            toolTip = FWObjectPropertiesFactory::getPolicyRuleOptions(rule);
                    }
                    if (NATRule::cast(rule)!=NULL )
                    {
                        if (!isDefaultNATRuleOptions( rule->getOptionsObject()))
                            toolTip = FWObjectPropertiesFactory::getNATRuleOptions(rule);
                    }
                }
                break;

            case ColDesc::Direction:
                toolTip = v.value<QString>();
                break;

            case ColDesc::Action:
                toolTip = v.value<ActionDesc>().tooltip;
                break;

            default:
                FWObject *object = getObject(pos, index);
                if (object == 0) return true;
                toolTip = FWObjectPropertiesFactory::getObjectPropertiesDetailed(object, true, true);
        }
    }
    else
    {
        toolTip = node->name;
    }

    if (toolTip.isEmpty()) return true;

    QRect   cr = visualRect(index);

    cr = QRect(
            cr.left() - horizontalOffset() - 2,
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

void RuleSetView::setModel(QAbstractItemModel *model)
{
    connect (model, SIGNAL (dataChanged(QModelIndex,QModelIndex)),
             this, SLOT (updateAllColumnsSize()));
    connect (model, SIGNAL (layoutChanged()),
             this, SLOT (updateAllColumnsSize()));

    QTreeView::setModel(model);
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
    if (md->getFirewall()==NULL) return;

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

        QModelIndex newIndex = md->index(oldIndex.row(), newColumn, oldIndex.parent());

        re = getRE(newIndex);
        if (re==NULL)
        {
            fwosm->setSelected(NULL, newIndex);
            setCurrentIndex(newIndex);
            if (mw->isEditorVisible() && !switchObjectInEditor(newIndex))
            {
                ev->accept();
            }
            return;
        }

        FWObject *newObj = getObject(objno, newIndex);

        selectObject(newObj, newIndex);

        if (mw->isEditorVisible() && !switchObjectInEditor(newIndex))
        {
            ev->accept();
        }
        return;
    }

    if (ev->key()==Qt::Key_PageDown || ev->key()==Qt::Key_PageUp ||
        ev->key()==Qt::Key_End || ev->key()==Qt::Key_Home)
    {
        QTreeView::keyPressEvent(ev);
        QModelIndex newIndex = md->index(currentIndex().row(), oldIndex.column(), currentIndex().parent());

        re = getRE(newIndex);
        FWObject *object = NULL;
        if (re != NULL)
        {
            object=re->front();
            if (FWReference::cast(object)!=NULL) object=FWReference::cast(object)->getPointer();
            selectObject(object, newIndex);
        }
        else
        {
            fwosm->setSelected(NULL, newIndex);
            setCurrentIndex(newIndex);
        }
        if (mw->isEditorVisible() && !switchObjectInEditor(newIndex)) ev->accept();
        return;
    }

    if (ev->key()==Qt::Key_Down || ev->key()==Qt::Key_Up)
    {
        re = getRE(oldIndex);
        FWObject *object = md->getFirewall();
        QModelIndex newIndex = oldIndex;
        FWObject::iterator i;

        if (re == NULL && !md->isGroup(oldIndex))
        {
            // Non-object column. Just move focus up or down;
            QTreeView::keyPressEvent(ev);
            newIndex = md->index(currentIndex().row(), oldIndex.column(), currentIndex().parent());
            if (!md->isGroup(newIndex)) 
            {
                setCurrentIndex(newIndex);
                fwosm->setSelected(NULL, newIndex);
                if (mw->isEditorVisible()) switchObjectInEditor(newIndex);
                ev->accept();
            }
            return;
        }
        else
        {
            if (md->isGroup(oldIndex))
            {
                object = NULL;
            }
            else
            {
                FWObject *prev = NULL;
                for (i=re->begin(); i!=re->end(); ++i)
                {
                    object = *i;
                    if (FWReference::cast(object) != NULL) object = FWReference::cast(object)->getPointer();
                    if (ev->key()==Qt::Key_Up   && object==fwosm->selectedObject)   break;
                    if (ev->key()==Qt::Key_Down && prev==fwosm->selectedObject) break;
                    prev=object;
                }
                if (ev->key()==Qt::Key_Up) object = prev;
                if (ev->key()==Qt::Key_Down && i == re->end()) object = NULL;
            }

            if (object == NULL)
            {
                // It needs to move to another row
                QTreeView::keyPressEvent(ev);
                newIndex = md->index(currentIndex().row(), oldIndex.column(), currentIndex().parent());

                if (oldIndex.row() == newIndex.row())
                {
                    // we are stuck! It's very first or last row.
                    object = fwosm->selectedObject;
                }
                else
                {
                    re = getRE(newIndex);
                    if (re != NULL)
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
                        object = *i;
                        if (FWReference::cast(object) != NULL) object = FWReference::cast(object)->getPointer();
                    }
                    else
                    {
                        if (!md->isGroup(newIndex))
                        {
                            setCurrentIndex(newIndex);
                            fwosm->setSelected(NULL, newIndex);
                            if (mw->isEditorVisible()) switchObjectInEditor(newIndex);
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
        if (mw->isEditorVisible()) switchObjectInEditor(newIndex);
        ev->accept();
        return;
    }

    if (ev->key()==Qt::Key_Delete)
    {
        deleteSelectedObject();
    }

    QTreeView::keyPressEvent(ev);
}

void RuleSetView::compileCurrentRule()
{
    RuleSetModel* md = ((RuleSetModel*)model());
    if(!isTreeReadWrite(this,md->getRuleSet())) return;
    if (md->getFirewall()==NULL) return;

    QModelIndex index = currentIndex();
    if (!index.isValid()) return;
    RuleNode* node = static_cast<RuleNode *>(index.internalPointer());
    if (node == 0 || node->type != RuleNode::Rule || node->rule == 0) return;

    // if (mw->isEditorVisible() &&
    //     !mw->requestEditorOwnership(this, node->rule, ObjectEditor::optRuleCompile, true))
    //     return;

    mw->singleRuleCompile(node->rule);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PolicyView
//////////////////////////////////////////////////////////////////////////////////////////////////////////

PolicyView::PolicyView(ProjectPanel *project, Policy *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new PolicyModel(p,this);
    setModel(model);
    delete sm;
    setItemDelegate(new RuleSetViewDelegate(this,fwosm));
    init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// NATView
//////////////////////////////////////////////////////////////////////////////////////////////////////////

NATView::NATView(ProjectPanel *project, NAT *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new NatModel(p,this);
    setModel(model);
    delete sm;
    setItemDelegate(new RuleSetViewDelegate(this,fwosm));
    init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RoutingView
//////////////////////////////////////////////////////////////////////////////////////////////////////////

RoutingView::RoutingView(ProjectPanel *project, Routing *p, QWidget *parent):RuleSetView(project, parent)
{
    QItemSelectionModel *sm = QTreeView::selectionModel();
    RuleSetModel* model = new RoutingModel(p,this);
    setModel(model);
    delete sm;
    setItemDelegate(new RuleSetViewDelegate(this,fwosm));
    init();
}
