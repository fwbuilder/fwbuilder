/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: ACL.h,v 1.1 2008/03/06 06:48:55 vkurland Exp $

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

#ifndef __ACL_H
#define __ACL_H

#include <string>
#include <list>

#include "fwbuilder/Interface.h"

class ciscoACL {

    bool                     _ip_acl;
    std::string              _name;
    std::string              _workName;
    libfwbuilder::Interface *_interface;
    std::string              _dir;
    std::string              _last_rule_label;
    int                      nlines;
    std::list<std::string>   acl;

    std::string printLine(const std::string &s);

 public:
    ciscoACL() {
        _ip_acl = false;
        _name = "";
        _workName = "";
        _interface = NULL;
        _dir = "in";
        nlines = 0;
        _last_rule_label = "";
    }

    ciscoACL(const std::string &n,
             libfwbuilder::Interface *intf,
             const std::string &d="in",
             bool _ip_list=false)
    {
        _ip_acl = _ip_list;
        _name = n;
        _workName = "";
        _interface = intf;
        _dir = d;
        nlines = 0;
        _last_rule_label = "";
    }

    std::string addLine(const std::string &s);

    /*
     * Adds remark to access list. Checks and adds each remark only
     * once. We use rule labels for remarks
     */
    std::string addRemark(const std::string &rl);

    void setName(const std::string &s) { _name=s; }
    std::string name() { return _name; }

    void setWorkName(const std::string &s) { _workName=s; }
    std::string workName() { return _workName; }

    void setInterface(libfwbuilder::Interface *intf) { _interface=intf; }
    libfwbuilder::Interface* getInterface() { return _interface; }
    
    void setDirection(const std::string &d) { _dir=d; }
    std::string direction() { return _dir; }
    
    
    std::string print();
    std::string printLastLine();
    
    int  size() { return nlines; }
};

#endif
