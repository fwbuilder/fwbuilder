/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "global.h"

#include <fwbuilder/Library.h>
#include <fwbuilder/Cluster.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/RuleSet.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Routing.h>
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include "ProjectPanel.h"
#include "events.h"

#include "FWBTree.h"
#include "FWWindow.h"
#include "RCS.h"
#include "RuleSetView.h"
#include "RuleSetModel.h"
#include "ColDesc.h"

#include <QMdiSubWindow>
#include <QMdiArea>
#include <QtDebug>


using namespace Ui;
using namespace libfwbuilder;
using namespace std;

bool ProjectPanel::event(QEvent *event)
{
    if (event->type() >= QEvent::User)
    {
        fwbUpdateEvent *ev = dynamic_cast<fwbUpdateEvent*>(event);
        int event_code = event->type() - QEvent::User;
        QString data_file = ev->getFileName();
        int obj_id = ev->getObjectId();
        FWObject *obj = db()->findInIndex(obj_id);

        if (fwbdebug)
            qDebug() << this
                     << "rcs:"
                     << rcs
                     << "rcs->getFileName():"
                     << QString((rcs!=nullptr) ? rcs->getFileName() : "")
                     << "file:"
                     << data_file
                     << "event:"
                     << ev->getEventName()
                     << "object:"
                     << ((obj!=nullptr) ? QString::fromUtf8(obj->getName().c_str()) : "")
                     << "(" << ((obj!=nullptr) ? obj->getTypeName().c_str() : "") << ")"
                     << "id=" << ((obj!=nullptr) ? obj->getId() : -1);

        if (event_code == UPDATE_GUI_STATE_EVENT && mdiWindow != nullptr)
        {
            m_panel->om->updateCreateObjectMenu(getCurrentLib());
            ev->accept();
            return true;
        }

        if ((rcs && rcs->getFileName() == data_file) ||
            (!rcs && data_file.isEmpty()))
        {
            switch (event_code)
            {
            case RELOAD_OBJECT_TREE_EVENT:
                registerTreeReloadRequest();
                ev->accept();
                return true;
                
            case RELOAD_OBJECT_TREE_IMMEDIATELY_EVENT:
                m_panel->om->reload();
                ev->accept();
                return true;
                
            case RELOAD_RULESET_EVENT:
                registerRuleSetRedrawRequest();
                // update rule set title as well
                //updateFirewallName();
                ev->accept();
                return true;

            case MAKE_CURRENT_RULE_VISIBLE_IN_RULESET_EVENT:
            {
                RuleSetView* rsv = getCurrentRuleSetView();
                if (rsv) rsv->makeCurrentRuleVisible();
                ev->accept();
                return true;
            }
   
            case RELOAD_RULESET_IMMEDIATELY_EVENT:
                redrawRuleSets();
                //reopenFirewall();
                // update rule set title as well
                //updateFirewallName();
                ev->accept();
                return true;
            }

            if (obj == nullptr) return false;

            switch (event_code)
            {
            case DATA_MODIFIED_EVENT:
            {
                // This event does not trigger any updates in the UI,
                // this purely data structure update event. 

                FWObject *p = obj;
                while (p && Firewall::cast(p)==nullptr) p = p->getParent();
                Firewall *f = Firewall::cast(p);
                // when user locks firewall object, this code tries to
                // update last_modified timestamp in it because it
                // depends on itself. Dont.
                if (f && !f->isReadOnly())
                {
                    f->updateLastModifiedTimestamp();
                    QCoreApplication::postEvent(
                        mw, new updateObjectInTreeEvent(data_file, f->getId()));
                }
                registerModifiedObject(obj);

                QCoreApplication::postEvent(mw, new updateGUIStateEvent());

                ev->accept();
                return true;
            }

            case UPDATE_OBJECT_EVERYWHERE_EVENT:
            {
                Rule *rule = nullptr;
                RuleSet* current_ruleset = nullptr;
                RuleSetView* rsv = getCurrentRuleSetView();
                RuleSetModel* md = nullptr;
                if (rsv)
                {
                    md = (RuleSetModel*)rsv->model();
                    current_ruleset = md->getRuleSet();
                }
                if (RuleElement::cast(obj)) rule = Rule::cast(obj->getParent());
                if (Rule::cast(obj)) rule = Rule::cast(obj);

                if (rule && current_ruleset && md && rule->isChildOf(current_ruleset))
                {
                    md->rowChanged(md->index(rule, 0));
                    ev->accept();
                    return true;
                }

                if (rule)
                {
                    QCoreApplication::postEvent(
                        this, new showObjectInRulesetEvent(data_file, obj_id));
                    ev->accept();
                    return true;
                }

                if (rsv) rsv->updateObject(obj);

                if (Library::cast(obj))
                {
                    m_panel->om->updateLibName(obj);
                    m_panel->om->updateLibColor(obj);
                }

                QCoreApplication::postEvent(
                    mw, new updateObjectInTreeEvent(data_file, obj_id));

                // QCoreApplication::postEvent(
                //     this, new reloadRulesetEvent(data_file));

                ev->accept();
                return true;
            }

            case OBJECT_NAME_CHANGED_EVENT:
            {
                objectNameChangedEvent *name_change_event =
                    dynamic_cast<objectNameChangedEvent*>(event);
                m_panel->om->updateObjectInTree(obj);
                if (name_change_event->rename_children)
                {
                    // This performs automatic renaming of child objects if necessary
                    m_panel->om->autoRenameChildren(obj, name_change_event->old_name);
                }
                ev->accept();
                return true;
            }

            case UPDATE_LAST_COMPILED_TIMESTAMP_EVENT:
                if (rcs && !rcs->isRO() &&
                    Firewall::cast(obj) && !obj->isReadOnly())
                {
                    Firewall::cast(obj)->updateLastCompiledTimestamp();
                    QCoreApplication::postEvent(
                        mw, new updateObjectInTreeEvent(data_file, obj_id));
                    ev->accept();
                    return true;
                }
                break;

            case UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT:
                if (rcs && !rcs->isRO() &&
                    Firewall::cast(obj) && !obj->isReadOnly())
                {
                    Firewall::cast(obj)->updateLastInstalledTimestamp();
                    QCoreApplication::postEvent(
                        mw, new updateObjectInTreeEvent(data_file, obj_id));
                    ev->accept();
                    return true;
                }
                break;
            }

            // Events below this should only be processed if
            // ProjectPanel has been attached to an MDI window.  There
            // is no MDI window right after project panel is created
            // but some operations may already be performed. See
            // FWWindow::fileOpen where ProjectPanel is cfeated and
            // file is opened before MDI window is attached. So the UI
            // update events below will only be processed if MDI
            // window exists.

            if (mdiWindow == nullptr) return false;

            switch (event->type() - QEvent::User)
            {
            case INSERT_OBJECT_IN_TREE_EVENT:
            {
                FWObject *parent =
                    db()->findInIndex(
                        dynamic_cast<insertObjectInTreeEvent*>(event)->parent_id);
                m_panel->om->insertSubtree(parent, obj);
                ev->accept();
                return true;
            }

            case REMOVE_OBJECT_FROM_TREE_EVENT:
            {
                m_panel->om->removeObjectFromTreeView(obj);
                ev->accept();
                return true;
            }

            case ADD_TREE_PAGE_EVENT:
                m_panel->om->addLib(obj);
                ev->accept();
                return true;

            case REMOVE_TREE_PAGE_EVENT:
                m_panel->om->removeLib(obj);
                ev->accept();
                return true;

            case UPDATE_OBJECT_IN_TREE_EVENT:
                registerObjectToUpdateInTree(obj, false);
                ev->accept();
                return true;

            case UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT:
                registerObjectToUpdateInTree(obj, true);
                ev->accept();
                return true;

            case UPDATE_OBJECT_AND_SUBTREE_IMMEDIATELY_EVENT:
                m_panel->om->updateObjectInTree(obj, true);
                ev->accept();
                return true;

            case OPEN_RULESET_EVENT:
                openRuleSet(obj);
                // update rule set title as well
                //updateFirewallName();
                ev->accept();
                return true;

            case OPEN_RULESET_IMMEDIATELY_EVENT:
                openRuleSet(obj, true);
                // update rule set title as well
                //updateFirewallName();
                ev->accept();
                return true;

            case SELECT_RULE_ELEMENT_EVENT:
            {
                RuleSetView* rsv = getCurrentRuleSetView();
                rsv->selectRE(Rule::cast(obj),
                              dynamic_cast<selectRuleElementEvent*>(event)->column_type);
                rsv->setFocus(Qt::OtherFocusReason);
                ev->accept();
                return true;
            }

            case SHOW_OBJECT_IN_RULESET_EVENT:
            {
                // if obj is child of RuleElement (i.e. a reference object)
                FWReference *ref = FWReference::cast(obj);
                if (ref)
                {
                    RuleSet* current_ruleset = nullptr;
                    RuleSetView* rsv = getCurrentRuleSetView();
                    RuleSetModel* md = nullptr;
                    if (rsv)
                    {
                        md = (RuleSetModel*)rsv->model();
                        current_ruleset = md->getRuleSet();
                    }

                    if (current_ruleset && obj->isChildOf(current_ruleset))
                    {
                        clearManipulatorFocus();
                        rsv->selectRE(ref);
                        rsv->setFocus(Qt::OtherFocusReason);
                    } else
                    {
                        FWObject *rs = obj;
                        while (rs && RuleSet::cast(rs)==nullptr) rs = rs->getParent();
                        if (rs)
                        {
                            // reopen rule set right now, before we post event
                            // to show the object in it.
                            openRuleSet(rs);
                            QCoreApplication::postEvent(
                                this, new showObjectInRulesetEvent(data_file, obj_id));
                        }
                    }
                    ev->accept();
                    return true;
                }

                // if obj is RuleElement - select its first element
                RuleElement *re = RuleElement::cast(obj);
                if (re && re->size() > 0)
                {
                    QCoreApplication::postEvent(
                        this, new showObjectInRulesetEvent(data_file, obj->front()->getId()));
                    ev->accept();
                    return true;
                }

                // if obj is Rule - select its comment (the only common rule element)
                Rule *rule = Rule::cast(obj);
                if (rule)
                {
                    RuleSet* current_ruleset = nullptr;
                    RuleSetView* rsv = getCurrentRuleSetView();
                    RuleSetModel* md = nullptr;
                    if (rsv)
                    {
                        md = (RuleSetModel*)rsv->model();
                        current_ruleset = md->getRuleSet();
                    }
                    if (current_ruleset && rule->isChildOf(current_ruleset))
                    {
                        rsv->selectRE(rule, ColDesc::Comment);
                        rsv->setFocus(Qt::OtherFocusReason);
                        ev->accept();
                        return true;
                    } else
                    {
                        // this rule does not belong to the current ruleset
                        // reopen rule set right now, before we post event
                        // to show the object in it.
                        openRuleSet(rule->getParent(), true);
                        QCoreApplication::postEvent(
                            this, new showObjectInRulesetEvent(data_file, obj->getId()));
                    }
                    ev->accept();
                    return true;
                }

                ev->accept();
                return true;
            }

            case SHOW_OBJECT_IN_TREE_EVENT:
                //m_panel->om->setFocus();
                m_panel->om->openObjectInTree(obj);
                ev->accept();
                return true;

            case EXPAND_OBJECT_IN_TREE:
                m_panel->om->expandObjectInTree(obj);
                ev->accept();
                return true;

            case OPEN_LIBRARY_FOR_OBJECT_EVENT:
                m_panel->om->openLibForObject(obj);
                ev->accept();
                return true;

            case CLOSE_OBJECT_EVENT:
                if (RuleSet::cast(obj))
                {
                    if (visibleRuleSet == obj)
                    {
                        clearFirewallTabs();
                        closeRuleSet(obj);
                    }
                } else
                {
                    m_panel->om->closeObject();
                    mdiWindow->update();
                }
                ev->accept();
                return true;

            case ADD_USER_FOLDER_EVENT:
                m_panel->om->addUserFolderToTree(obj, dynamic_cast<addUserFolderEvent *>(event)->m_userFolder);
                ev->accept();
                return true;

            case REMOVE_USER_FOLDER_EVENT:
                m_panel->om->removeUserFolderFromTree(obj, dynamic_cast<removeUserFolderEvent *>(event)->m_userFolder);
                ev->accept();
                return true;

            case MOVE_TOFROM_USER_FOLDER_EVENT:
                moveToFromUserFolderEvent *moveEvent =
                    dynamic_cast<moveToFromUserFolderEvent *>(event);
                m_panel->om->moveToFromUserFolderInTree(obj, db()->findInIndex(moveEvent->m_objIdToMove), moveEvent->m_oldFolder, moveEvent->m_newFolder);
                ev->accept();
                return true;
            }
        }
        return false;
    }

    //if (fwbdebug) qDebug() << this << "event:" << event;
    
    return QWidget::event(event);
}

