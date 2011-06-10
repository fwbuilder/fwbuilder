/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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

#ifndef FWBTREE_H
#define FWBTREE_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>


namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
};


class FWBTree
{

    static QStringList            standardFolders;

    static QMap<QString,QString>  systemGroupTypes;
    static QMap<QString,QString>  systemGroupNames;
    static QMap<QString,QString>  systemGroupPaths;
    static QSet<QString>          standardIDs;

    static QMap<QString,bool>     copyMenuState;
    static QMap<QString,bool>     cutMenuState;
    static QMap<QString,bool>     pasteMenuState;
    static QMap<QString,bool>     deleteMenuState;

    void init_statics();
    
 public:

    FWBTree();

    bool isSystem(libfwbuilder::FWObject *obj);
    bool isStandardFolder(libfwbuilder::FWObject *obj);
    bool isStandardId(libfwbuilder::FWObject *obj);

    bool validateForInsertion(libfwbuilder::FWObject *target,
                              libfwbuilder::FWObject *obj,
                              QString &err);

    /**
     * returns boolean value that defines whether menu item "Copy"
     * should be enabled when object with a path objPath is selected
     * in the tree. By default menu items are enabled, so this method
     * returns True if object path is unknown.
     */
    bool getCopyMenuState(const QString &objPath) {
        if (copyMenuState.count(objPath)!=0)
            return copyMenuState[objPath];
        else
            return true;
    }
    bool getCutMenuState(const QString &objPath) {
        if (cutMenuState.count(objPath)!=0)
            return cutMenuState[objPath];
        else
            return true;
    }
    bool getPasteMenuState(const QString &objPath) {
        if (pasteMenuState.count(objPath)!=0)
            return pasteMenuState[objPath];
        else
            return true;
    }
    bool getDeleteMenuState(const QString &objPath) {
        if (deleteMenuState.count(objPath)!=0)
            return deleteMenuState[objPath];
        else
            return true;
    }

    void getStandardSlotForObject(const QString &objType,
                                         QString &parentType,
                                         QString &parentName);
    libfwbuilder::FWObject* getStandardSlotForObject(libfwbuilder::FWObject* lib,
                                                     const QString &objType);
    libfwbuilder::FWObject* createNewLibrary(libfwbuilder::FWObjectDatabase *db);

    QString getTranslatableObjectTypeName(const QString &type_name);
    QString getTranslatableNewObjectMenuText(const QString &type_name);

    static QList<const char *> getObjectTypes();
    static QList<const char *> getServiceTypes();
};

#endif
