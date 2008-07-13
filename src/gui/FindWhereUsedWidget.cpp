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


FindWhereUsedWidget::FindWhereUsedWidget(QWidget*p,
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
        connect (m_widget->dropArea,SIGNAL(objectDeleted()),this,SLOT(init()));
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
    FWObject *o;
    o = mapping[item];

    if (flShowObject && o!=NULL)
    {
        showObject(o);
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
    object=obj;
    m_widget->resListView->clear();
    mapping.clear();
    resset.clear();


    mw->db()->findWhereUsed(obj, mw->db(), resset);

    set<FWObject*>::iterator i=resset.begin();
    QTreeWidgetItem *item;
    QString c1;
    QString c2;
    FWObject* o;
    Rule* r;
    RuleSet* rs;
    FWObject* fw=NULL;
    for(;i!=resset.end();++i)
    {
        o=*i;
        fw=NULL;
        r=NULL;
        rs=NULL;

//        if (findRef(object,o)==NULL) continue;

        if (mw->isSystem(o) || RuleSet::cast(o) ||
            Firewall::cast(o) || Library::cast(o)) continue;

        c1 = QString::fromUtf8(o->getName().c_str());
        c2 = tr("Type: ")+QString::fromUtf8(o->getTypeName().c_str());

        if (RuleElement::cast(o)!=NULL || Rule::cast(o)!=NULL)
        {
            fw = o;
            while (fw!=NULL && !Firewall::isA(fw))
            {
                if (Rule::cast(fw))
                {
                    r=Rule::cast(fw);
                } else if (RuleSet::cast(fw))
                {
                    rs=RuleSet::cast(fw);
                }
                fw=fw->getParent();
            }
            if (fw==NULL || r==NULL || rs==NULL) continue;

            c1 = QString::fromUtf8(fw->getName().c_str());

            if (NAT::isA(rs))
            {
                c2=tr("NAT");
            } else if (Policy::isA(rs))
            {
                c2=tr("Policy");
            } else if (Routing::isA(rs))
            {
                c2=tr("Routing");
            } else
            {
                c2=tr("Unknown rule set");
            }
            c2+=tr("/Rule%1").arg(r->getPosition());
        }

        FWObject *pixobj=(fw==NULL)?o:fw;

        QString icn_file =
            (":/Icons/"+pixobj->getTypeName()+"/icon-tree").c_str();

        QPixmap pm;
        if ( ! QPixmapCache::find( icn_file, pm) )
        {
            pm.load( icn_file );
            QPixmapCache::insert( icn_file, pm);
        }

        QStringList qsl;
        qsl << c1 << c2;
        item = new QTreeWidgetItem(m_widget->resListView, qsl);
        item->setIcon(0, QIcon(pm));
        mapping[item] = o;
    }
    show();
}

void FindWhereUsedWidget::init()
{
    object=NULL;
    m_widget->resListView->clear();
    mapping.clear();
    resset.clear();

}

void FindWhereUsedWidget::findFromDrop()
{
    _find(m_widget->dropArea->getObject());
}

void FindWhereUsedWidget::showObject(FWObject* o)
{
    if (fwbdebug) qDebug("FindWhereUsedWidget::showObject");

    if (object==NULL || o==NULL) return;

    FWReference* ref=NULL;


    if (RuleElement::cast(o)!=NULL)
    {
        ref = findRef(object, o);
        if (ref==NULL) return;
        mw->activeProject()->openRuleSet(o->getParent()->getParent());
        mw->clearManipulatorFocus();
        mw->ensureObjectVisibleInRules( ref );
        RuleSetView *rsv = mw->activeProject()->getCurrentRuleSetView();
        rsv->selectRE( ref );
        rsv->setFocus ( Qt::MouseFocusReason );
//        mw->selectRules();
        if (mw->isEditorVisible())
        {
            mw->editObject( object );
        }
        return;
    }

    if (Rule::cast(o)!=NULL)
    {
        mw->activeProject()->openRuleSet(o->getParent());
        mw->clearManipulatorFocus();
        mw->ensureRuleIsVisible( Rule::cast(o) );
        RuleSetView *rsv = mw->activeProject()->getCurrentRuleSetView();
        rsv->selectRE( Rule::cast(o)->getPosition(),
                       rsv->getColByType(RuleSetView::Action));

        if (mw->isEditorVisible())
        {
            mw->editObject( object );
        }
        return;
    }

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
        return;
    }

    //mw->closeEditor();
    //mw->openObject( o );
}
