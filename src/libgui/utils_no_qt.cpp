/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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
#include "utils_no_qt.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"

#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
using namespace libfwbuilder;


/**
 * this method finds all firewalls in the tree and makes no
 * assumtions about tree structure
 */
void findFirewalls(FWObject *o, std::list<FWObject*> &fwlist,
                   bool skip_system_libs)
{
    findByObjectType(o,Firewall::TYPENAME,fwlist,skip_system_libs);
}

void findClusters(FWObject *o, std::list<FWObject*> &fwlist,
                  bool skip_system_libs)
{
    findByObjectType(o, Cluster::TYPENAME, fwlist, skip_system_libs);
}

void findHosts(FWObject *o, std::list<FWObject*> &fwlist,
               bool skip_system_libs)
{
    findByObjectType(o,Host::TYPENAME,fwlist,skip_system_libs);
}

void findByObjectType(FWObject *o,
                      const string &otype,
                      list<FWObject*> &fwlist,
                      bool skip_system_libs)
{
    if (skip_system_libs)
    {
        if (o->getId()==FWObjectDatabase::DELETED_OBJECTS_ID) return;
        if (o->getId()==FWObjectDatabase::TEMPLATE_LIB_ID) return;
    }

    for (list<FWObject*>::iterator m=o->begin(); m!=o->end(); m++)
    {
        FWObject *o1=*m;
        if (FWReference::cast(o1)!=nullptr) continue;
        if (o1->getTypeName()==otype) fwlist.push_back(o1);
        else                   findByObjectType(o1,otype,fwlist);
    }
}

/** 
 * Find reference to object <o> in the group <container> and return
 * pointer to that reference object. If no such reference exists,
 * return nullptr.
 */
FWReference* findRef(FWObject *o, FWObject *container)
{
    FWReference* ref=nullptr;
    FWObject::iterator i = container->begin();
    for(; i!=container->end(); ++i)
    {
        ref = FWReference::cast(*i);
        if (ref!=nullptr && ref->getPointer() == o)
        {
            break;
        }
        ref=nullptr;
    }
    return ref;
}

string strip(const string &s)
{
    if (s.empty()) return s;

    string tmps = s;
    string::size_type n1,n2;

    n1 = s.find_first_not_of(" ", 0);
    n2 = s.size()-1;
    while (n2>n1 && isspace(s[n2])) n2--;
    n2++;

    return s.substr(n1, n2-n1);
}

string getPathToBinary(const string &pgm_name)
{
    return appRootDir + FS_SEPARATOR + pgm_name;
}

