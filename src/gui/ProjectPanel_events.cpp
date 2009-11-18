/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

#include "../../config.h"
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

#include <QMdiSubWindow>
#include <QMdiArea>


using namespace Ui;
using namespace libfwbuilder;
using namespace std;

bool ProjectPanel::event(QEvent *event)
{
    if (event->type() >= QEvent::User)
    {
        fwbUpdateEvent *ev = dynamic_cast<fwbUpdateEvent*>(event);
        QString data_file = ev->getFileName();
        int obj_id = ev->getObjectId();
        FWObject *obj = db()->findInIndex(obj_id);
        if (obj == NULL) return false;
        if ((rcs && rcs->getFileName() == data_file) ||
            (!rcs && data_file.isEmpty()))
        {
            switch (event->type() - QEvent::User)
            {
            case DATA_MODIFIED_EVENT:
            {
                if (fwbdebug)
                    qDebug("ProjectPanel::event  dataModified  obj: %s",
                           obj->getName().c_str());
                // This event does not trigger any updates in the UI,
                // this purely data structure update event. However,
                // we post updateObjectInTreeEvent even here to
                // trigger UI updates.

                // This should enable "Save" action since something has changed
                mw->prepareFileMenu();

                FWObject *p = obj;
                while (p && Firewall::cast(p)==NULL) p = p->getParent();
                Firewall *f = Firewall::cast(p);
                if (f)
                {
                    f->updateLastModifiedTimestamp();
                    QCoreApplication::postEvent(
                        this, new updateObjectInTreeEvent(data_file,
                                                          f->getId()));
                }
                QCoreApplication::postEvent(
                    this, new updateObjectInTreeEvent(data_file, obj->getId()));

                registerModifiedObject(obj);
                ev->accept();
                return true;
            }

            case UPDATE_OBJECT_EVERYWHERE_EVENT:
                if (fwbdebug)
                    qDebug("ProjectPanel::event  updateObjectEverywhereEvent  obj: %s",
                           obj->getName().c_str());
                QCoreApplication::postEvent(
                    this, new updateObjectInTreeEvent(data_file, obj_id));
                QCoreApplication::postEvent(
                    this, new updateObjectInRulesetEvent(data_file, obj_id));
                if (Library::cast(obj))
                {
                    m_panel->om->updateLibName(obj);
                    m_panel->om->updateLibColor(obj);
                }
                ev->accept();
                return true;

            case OBJECT_NAME_CHANGED_EVENT:
                objectNameChangedEvent *name_change_event =
                    dynamic_cast<objectNameChangedEvent*>(event);
                // This performs automatic renaming of child objects if necessary
                updateObjName(obj, name_change_event->old_name, true);
                ev->accept();
                return true;

            case UPDATE_LAST_COMPILED_TIMESTAMP_EVENT:
                if (Firewall::cast(obj))
                {
                    Firewall::cast(obj)->updateLastCompiledTimestamp();
                    QCoreApplication::postEvent(
                        this, new updateObjectInTreeEvent(data_file, obj_id));
                    ev->accept();
                    return true;
                }
                break;

            case UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT:
                if (Firewall::cast(obj))
                {
                    Firewall::cast(obj)->updateLastInstalledTimestamp();
                    QCoreApplication::postEvent(
                        this, new updateObjectInTreeEvent(data_file, obj_id));
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

            if (mdiWindow == NULL) return false;

            switch (event->type() - QEvent::User)
            {
            case UPDATE_OBJECT_IN_TREE_EVENT:
                if (fwbdebug)
                    qDebug("ProjectPanel::event  updateObjectInTreeEvent  obj: %s",
                           obj->getName().c_str());
                registerObjectToUpdateInTree(obj, false);
                ev->accept();
                return true;

            case UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT:
                if (fwbdebug)
                    qDebug("ProjectPanel::event  updateObjectAndSubtreeInTreeEvent  obj: %s",
                           obj->getName().c_str());
                registerObjectToUpdateInTree(obj, true);
                ev->accept();
                return true;

            case UPDATE_OBJECT_IN_RULESET_EVENT:
            {
                if (fwbdebug)
                    qDebug("ProjectPanel %p: updateObjectInRulesetEvent received for file %s",
                           this, data_file.toLatin1().constData());
                RuleSetView* rv=dynamic_cast<RuleSetView*>(
                    m_panel->ruleSets->currentWidget());
                if (rv!=NULL && getSelectedObject() != rv->getSelectedObject())
                {
                    rv->repaintSelection();
                } else
                    scheduleRuleSetRedraw();
                // update rule set title as well
                updateFirewallName();
                ev->accept();
                return true;
            }

            case SHOW_OBJECT_IN_TREE_EVENT:
                if (fwbdebug)
                    qDebug("ProjectPanel %p: showObjectInTreeEvent received for file %s",
                           this, data_file.toLatin1().constData());
                m_panel->om->openObject(obj);

                ev->accept();
                return true;

            case CLOSE_OBJECT_EVENT:
                if (RuleSet::cast(obj))
                {
                    clearFirewallTabs();
                    closeRuleSet(obj);
                } else
                {
                    m_panel->om->closeObject();
                    mdiWindow->update();
                }
                ev->accept();
                return true;
            }
        }
        return false;
    }

    return QWidget::event(event);
}

