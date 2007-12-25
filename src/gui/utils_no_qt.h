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


#ifndef  __UTILS_NO_QT_H_
#define  __UTILS_NO_QT_H_


#include <string>
#include <list>
#include <functional>

#include <utility>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWReference.h"

/* Utility functions that do not depend on QT */

struct FWObjectNameCmpPredicate :
    public std::binary_function<libfwbuilder::FWObject*, libfwbuilder::FWObject*, bool>
{
    bool operator()(libfwbuilder::FWObject *a,libfwbuilder::FWObject *b)
    {
        return a->getName() < b->getName();
    }
};

class findFWObjectIDPredicate : public std::unary_function<libfwbuilder::FWObject*, bool>
{
    std::string _id;
    public:
    findFWObjectIDPredicate(const std::string &id):_id(id) {}
    bool operator()(const libfwbuilder::FWObject *o) const
    {return o->getId()==_id;}
};

extern void findFirewalls(libfwbuilder::FWObject *o,
                          std::list<libfwbuilder::FWObject*> &fwlist,
                          bool skip_system_libs=true);

extern void findHosts(libfwbuilder::FWObject *o,
                      std::list<libfwbuilder::FWObject*> &fwlist,
                      bool skip_system_libs=true);

extern void findByObjectType(libfwbuilder::FWObject *o,
                             const std::string &otype,
                             std::list<libfwbuilder::FWObject*> &fwlist,
                             bool skip_system_libs=true);

extern libfwbuilder::FWReference* findRef(libfwbuilder::FWObject *o,
                                          libfwbuilder::FWObject *p);

// helper: strip whitespaces from the beginning and end of a string
extern std::string strip(const std::string &s);
  
// a functor to join list<string> into a string with separator sep
class join : public std::unary_function<std::string, void>
{
    std::string *result;
    std::string  separator;
public:
    join(std::string *res, const std::string &s)
    { result = res; separator = s; }
    void operator()(std::string &s);
};


#endif
