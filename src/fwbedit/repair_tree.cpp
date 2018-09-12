/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: fwbedit.cpp 429 2008-07-31 07:03:39Z vadim $

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

#include "fwbuilder/Constants.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/IntervalGroup.h"

#include "fwbedit.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;


void testAndFix(FWObjectDatabase *objdb,
                const string &path, const string &type, FWObject *root)
{
    list<FWObject*> objects;
    findObjects(path, root, objects);
    if (objects.size()==0)
    {
        string::size_type n = path.rfind('/');
        string obj_name = path.substr(n+1);
        string parent_path = path.substr(0, n);
        findObjects(parent_path, root, objects);
        assert(objects.size()==1);
        FWObject *parent = objects.front();

        cout << path <<  " is missing ";
       
        FWObject *new_obj = objdb->create(type);
        new_obj->setName(obj_name);
        parent->add(new_obj);
        cout << "( Fixed )" << endl;
    } else
        cout << path << " ok" << endl;
}


void checkAndRepairTree(FWObjectDatabase *objdb)
{
    cout << "Check and repair tree structure:" << endl;
    FWObject *root=objdb;
    FWObject *nlib;
    string lib_name;
    bool ro_flag;
            
    for (FWObject::iterator i=root->begin(); i!=root->end(); ++i)
    {
        nlib = *i;
        lib_name = nlib->getName();
        if (nlib->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
        {
            ro_flag = nlib->isReadOnly();
            cout << "Library: " << lib_name
                 << ((ro_flag)?"(Read only)":" ") << endl;

            nlib->setReadOnly(false);
                    
            testAndFix(objdb, lib_name + "/Objects",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Address Ranges",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Address Tables",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Addresses",
                       ObjectGroup::TYPENAME,  root);
            testAndFix(objdb, lib_name + "/Objects/DNS Names",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Groups",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Hosts",
                       ObjectGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Objects/Networks",
                       ObjectGroup::TYPENAME, root);
                        
            testAndFix(objdb, lib_name + "/Services",
                       ServiceGroup::TYPENAME,nlib);
            testAndFix(objdb, lib_name + "/Services/Custom",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/Groups",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/ICMP",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/IP",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/TCP",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/UDP",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/Custom",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/TagServices",
                       ServiceGroup::TYPENAME, root);
            testAndFix(objdb, lib_name + "/Services/Users",
                       ServiceGroup::TYPENAME, root);
                        
            testAndFix(objdb, lib_name + "/Clusters",
                       ObjectGroup::TYPENAME,nlib);
            testAndFix(objdb, lib_name + "/Firewalls",
                       ObjectGroup::TYPENAME,nlib);
            testAndFix(objdb, lib_name + "/Time",
                       IntervalGroup::TYPENAME,nlib);

            nlib->setReadOnly(ro_flag);    
        }
    }
}

