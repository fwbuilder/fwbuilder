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

#include <map>

#include <qstring.h>

namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
};

class FWBTree {

    std::map<QString,QString>  systemGroupTypes;
    std::map<QString,QString>  systemGroupNames;
    std::map<QString,QString>  systemGroupPaths;
    std::map<QString,bool>     copyMenuState;
    std::map<QString,bool>     cutMenuState;
    std::map<QString,bool>     pasteMenuState;
    std::map<QString,bool>     deleteMenuState;
    
 public:
    
    FWBTree();

    static bool isSystem(libfwbuilder::FWObject *obj);
    static bool isStandardId(libfwbuilder::FWObject *obj);
    
    static bool validateForInsertion(libfwbuilder::FWObject *target,libfwbuilder::FWObject *obj);

    /**
     * returns boolean value that defines whether menu item "Copy"
     * should be enabled when object with a path objPath is selected
     * in the tree. By default menu items are enabled, so this method
     * returns True if object path is unknown.
     */
    static bool getCopyMenuState(const QString &objPath) {
        if (standardObjectTreeFormat->copyMenuState.count(objPath)!=0)
            return standardObjectTreeFormat->copyMenuState[objPath];
        else
            return true;
    }
    static bool getCutMenuState(const QString &objPath) {
        if (standardObjectTreeFormat->cutMenuState.count(objPath)!=0)
            return standardObjectTreeFormat->cutMenuState[objPath];
        else
            return true;
    }
    static bool getPasteMenuState(const QString &objPath) {
        if (standardObjectTreeFormat->pasteMenuState.count(objPath)!=0)
            return standardObjectTreeFormat->pasteMenuState[objPath];
        else
            return true;
    }
    static bool getDeleteMenuState(const QString &objPath) {
        if (standardObjectTreeFormat->deleteMenuState.count(objPath)!=0)
            return standardObjectTreeFormat->deleteMenuState[objPath];
        else
            return true;
    }
    
    static void getStandardSlotForObject(const QString &objType,
                                         QString &parentType,
                                         QString &parentName);
    static libfwbuilder::FWObject* getStandardSlotForObject(libfwbuilder::FWObject* lib,
                                                            const QString &objType);
    static libfwbuilder::FWObject* createNewLibrary(libfwbuilder::FWObjectDatabase *db);

    static FWBTree *standardObjectTreeFormat;

};

