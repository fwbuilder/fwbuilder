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

#ifndef FWBUILDER_EVENTS_HH
#define FWBUILDER_EVENTS_HH

#include "global.h"
#include "ObjectEditor.h"
#include "ColDesc.h"

#include <QEvent>
#include <QString>

/* Custom event types.  QEvent::registerEventType is only available
   starting QT 4.4 but we have to support QT 4.3 for Ubuntu Hardy so
   need to allocate types manually for now. Event types are defined as
   increments above QEvent::User
*/

enum EVENT_CODES {
    DATA_MODIFIED_EVENT  ,
    UPDATE_OBJECT_IN_TREE_EVENT  ,
    UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT  ,
    UPDATE_OBJECT_AND_SUBTREE_IMMEDIATELY_EVENT  ,
    UPDATE_OBJECT_EVERYWHERE_EVENT  ,
    UPDATE_LAST_COMPILED_TIMESTAMP_EVENT ,
    UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT ,
    SHOW_OBJECT_IN_TREE_EVENT ,
    SHOW_OBJECT_IN_RULESET_EVENT ,
    SELECT_RULE_ELEMENT_EVENT,
    INSERT_OBJECT_IN_TREE_EVENT,
    REMOVE_OBJECT_FROM_TREE_EVENT,
    ADD_TREE_PAGE_EVENT,
    REMOVE_TREE_PAGE_EVENT,
    RELOAD_OBJECT_TREE_EVENT ,
    RELOAD_OBJECT_TREE_IMMEDIATELY_EVENT ,
    OPEN_RULESET_EVENT ,
    OPEN_RULESET_IMMEDIATELY_EVENT ,
    RELOAD_RULESET_EVENT ,
    RELOAD_RULESET_IMMEDIATELY_EVENT ,
    MAKE_CURRENT_RULE_VISIBLE_IN_RULESET_EVENT,
    OPEN_OBJECT_IN_EDITOR_EVENT ,
    OPEN_OPT_OBJECT_IN_EDITOR_EVENT ,
    CLOSE_OBJECT_EVENT ,
    OBJECT_NAME_CHANGED_EVENT,
    OPEN_LIBRARY_FOR_OBJECT_EVENT,
    UPDATE_SUBWINDOW_TITLES_EVENT,
    UPDATE_GUI_STATE_EVENT,
    EXPAND_OBJECT_IN_TREE,
    CLOSE_EDITOR_PANEL_EVENT ,
    CLEAR_EDITOR_PANEL_EVENT ,
    ADD_USER_FOLDER_EVENT,
    REMOVE_USER_FOLDER_EVENT,
    MOVE_TOFROM_USER_FOLDER_EVENT,
};


class fwbUpdateEvent : public QEvent {
    QString data_file_name;
    int object_id;
    QString event_name;
public:
    fwbUpdateEvent(const QString &file_name, int obj_id, QEvent::Type event_type,
                   const QString &ev_name) : QEvent(event_type), event_name(ev_name)
    {
        data_file_name = file_name;
        object_id = obj_id;
    }
    QString getEventName() { return event_name; }
    QString getFileName() { return data_file_name; }
    int getObjectId() { return object_id; }
};


class dataModifiedEvent : public fwbUpdateEvent {
public:
    dataModifiedEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + DATA_MODIFIED_EVENT),
                     "dataModifiedEvent")
      {}
};


class updateObjectInTreeEvent : public fwbUpdateEvent {
public:
    updateObjectInTreeEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_IN_TREE_EVENT),
                     "updateObjectInTreeEvent")
      {}
};


class updateObjectAndSubtreeInTreeEvent : public fwbUpdateEvent {
public:
    updateObjectAndSubtreeInTreeEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT),
                     "updateObjectAndSubtreeInTreeEvent")
      {}
};


class updateObjectAndSubtreeImmediatelyEvent : public fwbUpdateEvent {
public:
    updateObjectAndSubtreeImmediatelyEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_AND_SUBTREE_IMMEDIATELY_EVENT),
                     "updateObjectAndSubtreeImmediatelyEvent")
      {}
};


class updateObjectEverywhereEvent : public fwbUpdateEvent {
public:
    updateObjectEverywhereEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_EVERYWHERE_EVENT),
                     "updateObjectEverywhereEvent")
      {}
};


class updateLastCompiledTimestampEvent : public fwbUpdateEvent {
public:
    updateLastCompiledTimestampEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_LAST_COMPILED_TIMESTAMP_EVENT),
                     "updateLastCompiledTimestampEvent")
      {}
};


class updateLastInstalledTimestampEvent : public fwbUpdateEvent {
public:
    updateLastInstalledTimestampEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT),
                     "updateLastInstalledTimestampEvent")
      {}
};


class showObjectInTreeEvent : public fwbUpdateEvent {
public:
    showObjectInTreeEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + SHOW_OBJECT_IN_TREE_EVENT),
                   "showObjectInTreeEvent")
    {}
};


class showObjectInRulesetEvent : public fwbUpdateEvent {
public:
    showObjectInRulesetEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + SHOW_OBJECT_IN_RULESET_EVENT),
                     "showObjectInRulesetEvent")
      {}
};


class selectRuleElementEvent : public fwbUpdateEvent {
public:
    ColDesc::ColumnType column_type;
    selectRuleElementEvent(const QString &file_name, int obj_id, ColDesc::ColumnType ct) :
    fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + SELECT_RULE_ELEMENT_EVENT),
                     "selectRuleElementEvent")
    { column_type = ct; }
};


class insertObjectInTreeEvent : public fwbUpdateEvent {
public:
    int parent_id;
    insertObjectInTreeEvent(const QString &file_name, int p_id, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + INSERT_OBJECT_IN_TREE_EVENT),
                   "insertObjectInTreeEvent")
    { parent_id = p_id; }
};


class removeObjectFromTreeEvent : public fwbUpdateEvent {
public:
    removeObjectFromTreeEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + REMOVE_OBJECT_FROM_TREE_EVENT),
                   "removeObjectFromTreeEvent")
    {}
};


class addTreePageEvent : public fwbUpdateEvent {
public:
    addTreePageEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + ADD_TREE_PAGE_EVENT),
                   "addTreePageEvent")
    {}
};


class removeTreePageEvent : public fwbUpdateEvent {
public:
    removeTreePageEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + REMOVE_TREE_PAGE_EVENT),
                   "removeTreePageEvent")
    {}
};


class reloadObjectTreeEvent : public fwbUpdateEvent {
public:
    reloadObjectTreeEvent(const QString &file_name) :
    fwbUpdateEvent(file_name, -1,
                   QEvent::Type(QEvent::User + RELOAD_OBJECT_TREE_EVENT),
                   "reloadObjectTreeEvent")
    {}
};


class reloadObjectTreeImmediatelyEvent : public fwbUpdateEvent {
public:
    reloadObjectTreeImmediatelyEvent(const QString &file_name) :
    fwbUpdateEvent(file_name, -1,
                   QEvent::Type(QEvent::User + RELOAD_OBJECT_TREE_IMMEDIATELY_EVENT),
                   "reloadObjectTreeImmediatelyEvent")
    {}
};


class reloadRulesetEvent : public fwbUpdateEvent {
public:
    reloadRulesetEvent(const QString &file_name) :
    fwbUpdateEvent(file_name, -1,
                   QEvent::Type(QEvent::User + RELOAD_RULESET_EVENT),
                   "reloadRulesetEvent")
    {}
};


class reloadRulesetImmediatelyEvent : public fwbUpdateEvent {
public:
    reloadRulesetImmediatelyEvent(const QString &file_name) :
    fwbUpdateEvent(file_name, -1,
                   QEvent::Type(QEvent::User + RELOAD_RULESET_IMMEDIATELY_EVENT),
                   "reloadRulesetImmediatelyEvent")
    {}
};

class makeCurrentRuleVisibleInRulesetEvent : public fwbUpdateEvent {
public:
    makeCurrentRuleVisibleInRulesetEvent(const QString &file_name) :
    fwbUpdateEvent(file_name, -1,
                   QEvent::Type(QEvent::User + MAKE_CURRENT_RULE_VISIBLE_IN_RULESET_EVENT),
                   "makeCurrentRuleVisibleInRulesetEvent")
    {}
};



class openRulesetEvent : public fwbUpdateEvent {
public:
    openRulesetEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_RULESET_EVENT),
                   "openRulesetEvent")
    {}
};


class openRulesetImmediatelyEvent : public fwbUpdateEvent {
public:
    openRulesetImmediatelyEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_RULESET_IMMEDIATELY_EVENT),
                   "openRulesetImmediatelyEvent")
    {}
};


class openObjectInEditorEvent : public fwbUpdateEvent {
public:
    openObjectInEditorEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_OBJECT_IN_EDITOR_EVENT),
                   "openObjectInEditorEvent")
    {}
};


class openOptObjectInEditorEvent : public fwbUpdateEvent {
public:
    ObjectEditor::OptType opt_code;
    openOptObjectInEditorEvent(const QString &file_name, int obj_id, ObjectEditor::OptType opt) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_OPT_OBJECT_IN_EDITOR_EVENT),
                   "openOptObjectInEditorEvent")
    { opt_code = opt; }
};


class closeObjectEvent : public fwbUpdateEvent {
public:
    closeObjectEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + CLOSE_OBJECT_EVENT),
                   "closeObjectEvent")
    {}
};


class objectNameChangedEvent : public fwbUpdateEvent {
public:
    QString old_name;
    QString new_name;
    bool rename_children;
    objectNameChangedEvent(const QString &file_name, int obj_id,
                           const QString &_old_name, const QString &_new_name,
                           bool _rename_children) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OBJECT_NAME_CHANGED_EVENT),
                   "objectNameChangedEvent")
    { old_name = _old_name; new_name = _new_name; rename_children = _rename_children; }
};


class openLibraryForObjectEvent : public fwbUpdateEvent {
public:
    QString old_name;
    QString new_name;
    openLibraryForObjectEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_LIBRARY_FOR_OBJECT_EVENT),
                   "openLibraryForObjectEvent")
    {}
};

/*
 * This event is processed by FWWindow class and updates titles of all
 * ProjectPanel windows.
 */
class updateSubWindowTitlesEvent : public fwbUpdateEvent {
public:
    updateSubWindowTitlesEvent() :
    fwbUpdateEvent("", -1,
                   QEvent::Type(QEvent::User + UPDATE_SUBWINDOW_TITLES_EVENT),
                   "updateSubWindowTitlesEvent")
    {}
};

/*
 * This event is processed by FWWindow class and updates all menus and
 * toolbar buttins
 */
class updateGUIStateEvent : public fwbUpdateEvent {
public:
    updateGUIStateEvent() :
    fwbUpdateEvent("", -1,
                   QEvent::Type(QEvent::User + UPDATE_GUI_STATE_EVENT),
                   "updateGUIStateEvent")
    {}
};

class expandObjectInTreeEvent : public fwbUpdateEvent {
public:
    int parent_id;
    expandObjectInTreeEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + EXPAND_OBJECT_IN_TREE),
                   "expandObjectInTreeEvent")
    {}
};

class closeEditorPanelEvent : public fwbUpdateEvent {
public:
    closeEditorPanelEvent() :
    fwbUpdateEvent("", -1,
                   QEvent::Type(QEvent::User + CLOSE_EDITOR_PANEL_EVENT),
                   "closeEditorPanelEvent")
    {}
};

class clearEditorPanelEvent : public fwbUpdateEvent {
public:
    clearEditorPanelEvent() :
    fwbUpdateEvent("", -1,
                   QEvent::Type(QEvent::User + CLEAR_EDITOR_PANEL_EVENT),
                   "clearEditorPanelEvent")
    {}
};


class addUserFolderEvent : public fwbUpdateEvent {
public:
    QString m_userFolder;
    addUserFolderEvent(const QString &fileName, int objId,
                       const QString &userFolder) :
        fwbUpdateEvent(fileName, objId,
                       QEvent::Type(QEvent::User + ADD_USER_FOLDER_EVENT),
                       "addUserFolderEvent")
    { m_userFolder = userFolder; }
};

class removeUserFolderEvent : public fwbUpdateEvent {
public:
    QString m_userFolder;
    removeUserFolderEvent(const QString &fileName, int objId,
                          const QString &userFolder) :
        fwbUpdateEvent(fileName, objId,
                       QEvent::Type(QEvent::User + REMOVE_USER_FOLDER_EVENT),
                       "removeUserFolderEvent")
    { m_userFolder = userFolder; }
};

class moveToFromUserFolderEvent : public fwbUpdateEvent {
public:
    int m_objIdToMove;
    QString m_oldFolder;
    QString m_newFolder;
    moveToFromUserFolderEvent(const QString &fileName, int objId,
                              int objIdToMove, const QString &oldFolder,
                              const QString &newFolder) :
        fwbUpdateEvent(fileName, objId,
                       QEvent::Type(QEvent::User+MOVE_TOFROM_USER_FOLDER_EVENT),
                       "removeUserFolderEvent"),
        m_objIdToMove(objIdToMove),
        m_oldFolder(oldFolder),
        m_newFolder(newFolder)
    {}
};

#endif
