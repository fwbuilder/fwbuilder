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
#include <QEvent>

/* Custom event types.  QEvent::registerEventType is only available
   starting QT 4.4 but we have to support QT 4.3 for Ubuntu Hardy so
   need to allocate types manually for now. Event types are defined as
   increments above QEvent::User
*/

#define DATA_MODIFIED_EVENT  1
#define UPDATE_OBJECT_IN_TREE_EVENT  2
#define UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT  3
#define UPDATE_OBJECT_IN_RULESET_EVENT  4
#define UPDATE_OBJECT_EVERYWHERE_EVENT  5
#define UPDATE_LAST_COMPILED_TIMESTAMP_EVENT 6
#define UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT 7
#define SHOW_OBJECT_IN_TREE_EVENT 8
#define OPEN_OBJECT_IN_EDITOR_EVENT 9
#define CLOSE_OBJECT_EVENT 10
#define OBJECT_NAME_CHANGED_EVENT  11


class fwbUpdateEvent : public QEvent {
    QString data_file_name;
    int object_id;
public:
    fwbUpdateEvent(const QString &file_name, int obj_id, QEvent::Type event_type) :
      QEvent(event_type) {
          data_file_name = file_name;
          object_id = obj_id;
      }
    QString getFileName() { return data_file_name; }
    int getObjectId() { return object_id; }
};


class dataModifiedEvent : public fwbUpdateEvent {
public:
    dataModifiedEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + DATA_MODIFIED_EVENT))
      {}
};


class updateObjectInTreeEvent : public fwbUpdateEvent {
public:
    updateObjectInTreeEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_IN_TREE_EVENT))
      {}
};


class updateObjectAndSubtreeInTreeEvent : public fwbUpdateEvent {
public:
    updateObjectAndSubtreeInTreeEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_AND_SUBTREE_IN_TREE_EVENT))
      {}
};


class updateObjectInRulesetEvent : public fwbUpdateEvent {
public:
    updateObjectInRulesetEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_IN_RULESET_EVENT))
      {}
};


class updateObjectEverywhereEvent : public fwbUpdateEvent {
public:
    updateObjectEverywhereEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_OBJECT_EVERYWHERE_EVENT))
      {}
};


class updateLastCompiledTimestampEvent : public fwbUpdateEvent {
public:
    updateLastCompiledTimestampEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_LAST_COMPILED_TIMESTAMP_EVENT))
      {}
};


class updateLastInstalledTimestampEvent : public fwbUpdateEvent {
public:
    updateLastInstalledTimestampEvent(const QString &file_name, int obj_id) :
      fwbUpdateEvent(file_name, obj_id,
                     QEvent::Type(QEvent::User + UPDATE_LAST_INSTALLED_TIMESTAMP_EVENT))
      {}
};


class showObjectInTreeEvent : public fwbUpdateEvent {
public:
    showObjectInTreeEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + SHOW_OBJECT_IN_TREE_EVENT))
    {}
};


class openObjectInEditorEvent : public fwbUpdateEvent {
public:
    openObjectInEditorEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OPEN_OBJECT_IN_EDITOR_EVENT))
    {}
};


class closeObjectEvent : public fwbUpdateEvent {
public:
    closeObjectEvent(const QString &file_name, int obj_id) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + CLOSE_OBJECT_EVENT))
    {}
};


class objectNameChangedEvent : public fwbUpdateEvent {
public:
    QString old_name;
    QString new_name;
    objectNameChangedEvent(const QString &file_name, int obj_id,
                           const QString &_old_name, const QString &_new_name) :
    fwbUpdateEvent(file_name, obj_id,
                   QEvent::Type(QEvent::User + OBJECT_NAME_CHANGED_EVENT))
    { old_name = _old_name; new_name = _new_name; }
};




#endif
