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

#include "../../config.h"
#include "../../definitions.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"

#include "FindWhereUsedWidget.h"
#include "FWWindow.h"
#include "FWObjectDropArea.h"
#include "ObjectManipulator.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "ObjectTreeView.h"
#include "RuleSetView.h"
#include "ProjectPanel.h"

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

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;


FindWhereUsedWidget::FindWhereUsedWidget(QWidget *p,
                                         const char * n,
                                         Qt::WindowFlags f,
                                         bool f_mini) : QWidget(p)
{
    m_widget = new Ui::findWhereUsedWidget_q;
    m_widget->setupUi(this);

    setObjectName(n);
    setWindowFlags(f);

    flShowObject=true;
    if (f_mini)
    {
        m_widget->buttonsBox->hide();
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

void FindWhereUsedWidget::itemActivated(QTreeWidgetItem* item)
{
    FWObject *container = (FWObject*)(qVariantValue<void*>(item->data(1, Qt::UserRole)));

    if (flShowObject && container!=NULL)
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

    if (fwbdebug) qDebug("FindWhereUsedWidget: initiate search for %s",
                         obj->getName().c_str());

    mw->db()->findWhereObjectIsUsed(obj, mw->db(), resset);
    humanizeSearchResults(resset);

    set<FWObject*>::iterator i=resset.begin();

    for (;i!=resset.end();++i)
    {
        FWObject *o = *i;

        if (fwbdebug) qDebug("Search result object id=%d type=%s name=%s",
                             o->getId(),
                             o->getTypeName().c_str(),
                             o->getName().c_str());

        QTreeWidgetItem *item = createQTWidgetItem(obj, o);
        if (item==NULL) continue;
        m_widget->resListView->addTopLevelItem(item);
    }
    m_widget->resListView->resizeColumnToContents(0);
    m_widget->resListView->resizeColumnToContents(1);
    show();
}


void FindWhereUsedWidget::init()
{
    object=NULL;
    m_widget->resListView->clear();
    resset.clear();
}

void FindWhereUsedWidget::findFromDrop()
{
    _find(m_widget->dropArea->getObject());
}

void FindWhereUsedWidget::showObject(FWObject* o)
{
    if (fwbdebug) qDebug("FindWhereUsedWidget::showObject  o=%s (%s)",
                         o->getName().c_str(), o->getTypeName().c_str());

    if (object==NULL || o==NULL) return;

    if (RuleElement::cast(o)!=NULL)
    {
        mw->activeProject()->openRuleSet(o->getParent()->getParent());
        mw->clearManipulatorFocus();
        RuleSetView *rsv = mw->activeProject()->getCurrentRuleSetView();
        rsv->selectRE(RuleElement::cast(o), object);
        rsv->setFocus(Qt::MouseFocusReason);

        if (mw->isEditorVisible()) mw->editObject( object );
    } else
    {
        if (Rule::cast(o)!=NULL)
        {
            mw->activeProject()->openRuleSet(o->getParent());
            mw->clearManipulatorFocus();
            RuleSetView *rsv = mw->activeProject()->getCurrentRuleSetView();
            rsv->selectRE(Rule::cast(o),rsv->getColByType(RuleSetView::Action));

            if (mw->isEditorVisible()) mw->editObject( object );
        } else
        {
            mw->unselectRules();

            if (Group::cast(o)!=NULL)
            {
                mw->openObject( o );
                mw->unselectRules();

                if (mw->isEditorVisible())
                {
                    mw->editObject( o );
                    mw->selectObjectInEditor( object);
                }
            } else
            {
                mw->openObject( object );
                mw->unselectRules();

                if (mw->isEditorVisible()) mw->editObject( object );
            }
        }
    }
    //mw->closeEditor();
    //mw->openObject( o );
}

void FindWhereUsedWidget::humanizeSearchResults(std::set<FWObject *> &resset)
{
    set<FWObject*> tmp_res;  // set deduplicates items automatically
    set<FWObject*>::iterator i = resset.begin();
    for (;i!=resset.end();++i)
    {
        FWReference  *ref = FWReference::cast(*i);

        if (ref)
        {
            FWObject *o = ref->getParent();  // NB! We need parent of this ref.
            tmp_res.insert(o);
            if (fwbdebug)
                qDebug("humanizeSearchResults: adding %s (%s)",
                       o->getName().c_str(), o->getTypeName().c_str());
#if 0
            if (RuleElement::cast(o)!=NULL)
            {
                tmp_res.insert(o->getParent());  // rule
                continue;
            }
            if (Group::cast(o)!=NULL)
            {
                tmp_res.insert(o);
                continue;
            }
#endif
        } else
            tmp_res.insert(*i);
    }
    resset.clear();
    resset = tmp_res;
}

QTreeWidgetItem* FindWhereUsedWidget::createQTWidgetItem(FWObject* o,
                                                         FWObject *container)
{
    QString c1, c2;
    FWObject *fw = NULL;
    Rule *r = NULL;
    RuleSet *rs = NULL;
    QPixmap object_icon;
    QPixmap parent_icon;

    if (mw->isSystem(container) || Library::cast(container)) return NULL;

    if (RuleElement::cast(container)!=NULL || Rule::cast(container)!=NULL)
    {
        fw = container;
        while (fw!=NULL && !Firewall::isA(fw))
        {
            if (Rule::cast(fw)) r = Rule::cast(fw);
            if (RuleSet::cast(fw)) rs = RuleSet::cast(fw);
            fw = fw->getParent();
        }
        if (fw==NULL || r==NULL || rs==NULL) return NULL;

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
        if (RuleElement::cast(container)!=NULL)
            rule_element_name = 
                getReadableRuleElementName(container->getTypeName());
        if (Rule::cast(container)!=NULL)
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

