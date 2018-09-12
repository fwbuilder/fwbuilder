/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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

#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"
#include "events.h"

#include "FindWhereUsedWidget.h"
#include "FWWindow.h"
#include "FWObjectDropArea.h"
#include "ObjectManipulator.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "ObjectTreeView.h"
#include "RuleSetView.h"
#include "ProjectPanel.h"
#include "ColDesc.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Resources.h"

#include <qstackedwidget.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qpixmapcache.h>
#include <QtDebug>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;


FindWhereUsedWidget::FindWhereUsedWidget(QWidget *p,
                                         ProjectPanel *pp,
                                         const char * n,
                                         Qt::WindowFlags f,
                                         bool f_mini) : QWidget(p)
{
    project_panel = pp;
    m_widget = new Ui::findWhereUsedWidget_q;
    m_widget->setupUi(this);

    setObjectName(n);
    setWindowFlags(f);

    flShowObject=true;
    if (f_mini)
    {
        m_widget->pushButton2->hide();
        m_widget->dropBox->hide();
    }
    else
    {
        connect(m_widget->dropArea,SIGNAL(objectDeleted()),this,SLOT(init()));
    }
}

FindWhereUsedWidget::~FindWhereUsedWidget()
{
    delete m_widget;
}

void FindWhereUsedWidget::setShowObject(bool fl)
{
    flShowObject=fl;
}

/**
 * This signal is emitted when the user activates an item by single-
 * or double-clicking (depending on the platform, i.e. on the
 * QStyle::SH_ItemView_ActivateItemOnSingleClick style hint) or
 * pressing a special key (e.g., Enter).
 */
void FindWhereUsedWidget::itemActivated(QTreeWidgetItem* item, int)
{
    FWObject *container = (FWObject*)(item->data(1, Qt::UserRole).value<void*>());

    if (flShowObject && container!=nullptr)
    {
        showObject(container);
    }
}

/**
 * This signal is emitted when the user clicks inside the widget.
 *
 * The specified item is the item that was clicked, or 0 if no item
 * was clicked. The column is the item's column that was clicked. If
 * no item was clicked, no signal will be emitted.
 *
 */
void FindWhereUsedWidget::itemClicked(QTreeWidgetItem* item, int)
{
    FWObject *container = (FWObject*)(item->data(1, Qt::UserRole).value<void*>());
    if (flShowObject && container!=nullptr)
    {
        showObject(container);
    }
}

void FindWhereUsedWidget::find()
{
    findFromDrop();
}

void FindWhereUsedWidget::find(FWObject *obj)
{
    m_widget->dropArea->insertObject(obj);
    find();
}

void FindWhereUsedWidget::_find(FWObject *obj)
{
    object = obj;
    m_widget->resListView->clear();
    resset.clear();

    if (fwbdebug)
        qDebug() << "FindWhereUsedWidget "
                 << this
                 << ": initiate search for "
                 << obj->getName().c_str()
                 << "  project_panel "
                 << project_panel;

    if (project_panel==nullptr) return;

    FWObjectDatabase *db = obj->getRoot();
    map<int, set<FWObject*> > reference_holders;
    UsageResolver().findAllReferenceHolders(obj, db, reference_holders);

    // rearrange reference holder object we just found to be able to sort them
    QMap<QString, QTreeWidgetItem*> widget_items;

    map<int, set<FWObject*> >::iterator it;
    for (it=reference_holders.begin(); it!=reference_holders.end(); ++it)
    {
        FWObject *c_obj = db->findInIndex(it->first);

        if (!m_widget->includeChildren->isChecked() && c_obj != obj) continue;

        foreach(FWObject *container, it->second)
        {
            QTreeWidgetItem *item = createQTWidgetItem(c_obj, container);
            if (item==nullptr) continue;
            QStringList item_str;
            item_str << item->text(0) << item->text(1) << item->text(2);
            widget_items[item_str.join("/")] = item;
        }
    }

    // TODO: This is not ideal because lines are sorted alphabetically.
    // Rules should be sorted by their numbers numerically.
    QStringList keys = widget_items.keys();
    qSort(keys);
    foreach(QString k, keys)
    {
        QTreeWidgetItem *item = widget_items[k];
        m_widget->resListView->addTopLevelItem(item);
    }

    m_widget->resListView->resizeColumnToContents(0);
    m_widget->resListView->resizeColumnToContents(1);
    show();
}

void FindWhereUsedWidget::init()
{
    object = nullptr;
    m_widget->resListView->clear();
    resset.clear();
}

void FindWhereUsedWidget::clear()
{
    m_widget->dropArea->deleteObject(); // this emits signal that calls init()
}

void FindWhereUsedWidget::findFromDrop()
{
    _find(m_widget->dropArea->getObject());
}

void FindWhereUsedWidget::showObject(FWObject* o)
{
    if (fwbdebug) qDebug("FindWhereUsedWidget::showObject  o=%s (%s)",
                         o->getName().c_str(), o->getTypeName().c_str());

    if (object==nullptr || o==nullptr) return;

    if (RuleElement::cast(o)!=nullptr || RuleElement::cast(o->getParent())!=nullptr)
    {
        QCoreApplication::postEvent(
            project_panel,
            new showObjectInRulesetEvent(project_panel->getFileName(), o->getId()));
        return;
    }

    if (Rule::cast(o)!=nullptr)
    {
        QCoreApplication::postEvent(
            project_panel,
            new openRulesetImmediatelyEvent(project_panel->getFileName(),
                                            o->getParent()->getId()));
        QCoreApplication::postEvent(
            project_panel,
            new selectRuleElementEvent(project_panel->getFileName(),
                                       o->getId(), ColDesc::Action));
        return;
    }

    project_panel->unselectRules();

    if (Group::cast(o)!=nullptr)
    {
        QCoreApplication::postEvent(
            project_panel,
            new showObjectInTreeEvent(project_panel->getFileName(), o->getId()));
        project_panel->unselectRules();
    } else
    {
        QCoreApplication::postEvent(
            project_panel,
            new showObjectInTreeEvent(project_panel->getFileName(), o->getId()));
        project_panel->unselectRules();
    }
}

QTreeWidgetItem* FindWhereUsedWidget::createQTWidgetItem(FWObject *o,
                                                         FWObject *container)
{
    if (fwbdebug)
        qDebug() << "FindWhereUsedWidget::createQTWidgetItem"
                 << "container:" << container->getName().c_str()
                 << "(" << container->getTypeName().c_str() << ")";

    QString c1, c2;
    FWObject *fw = nullptr;
    Rule *r = nullptr;
    RuleSet *rs = nullptr;
    QPixmap object_icon;
    QPixmap parent_icon;
    FWBTree tree_format;

    if (tree_format.isSystem(container) || Library::cast(container)) return nullptr;

    // container can be a Rule if user searched for an object used in action
    if (RuleElement::cast(container)!=nullptr || Rule::cast(container)!=nullptr)
    {
        fw = container;
        while (fw!=nullptr && Firewall::cast(fw)==nullptr) // Firewall::cast matches also Cluster
        {
            if (Rule::cast(fw)) r = Rule::cast(fw);
            if (RuleSet::cast(fw)) rs = RuleSet::cast(fw);
            fw = fw->getParent();
        }
        if (fw==nullptr || r==nullptr || rs==nullptr) return nullptr;

        c1 = QString::fromUtf8(fw->getName().c_str());
        QString ruleset_kind;

        if (NAT::isA(rs))
        {
            ruleset_kind = tr("NAT rule set");
        } else if (Policy::isA(rs))
        {
            ruleset_kind = tr("Policy rule set");
        } else if (Routing::isA(rs))
        {
            ruleset_kind = tr("Routing rule set");
        } else
        {
            ruleset_kind = tr("Rule set of unknown type");
        }

        QString rule_element_name;

        if (RuleElement::cast(container)!=nullptr)
            rule_element_name =
                getReadableRuleElementName(
                    fw->getStr("platform"), container->getParent()->getTypeName());

        if (Rule::cast(container)!=nullptr)
            rule_element_name = "Action";

        c2 += tr("%1 \"%2\" / Rule %3 / %4").
            arg(ruleset_kind).
            arg(rs->getName().c_str()).
            arg(Rule::cast(r)->getPosition()).
            arg(rule_element_name);

        loadIcon(parent_icon, fw);
    } else
    {
        c1 = QString::fromUtf8(container->getName().c_str());
        c2 = tr("Type: ")+QString::fromUtf8(container->getTypeName().c_str());

        loadIcon(parent_icon, container);
    }

    loadIcon(object_icon, o);

    QStringList qsl;
    qsl << QString::fromUtf8(o->getName().c_str()) << c1 << c2;
    QTreeWidgetItem* item = new QTreeWidgetItem(qsl);

    item->setIcon(1, QIcon(parent_icon));
    item->setIcon(0, QIcon(object_icon));

    item->setData(1, Qt::UserRole, qVariantFromValue((void*)container));

    return item;
}

