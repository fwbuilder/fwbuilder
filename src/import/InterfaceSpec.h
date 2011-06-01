/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef _INTERFACE_SPEC_H_
#define _INTERFACE_SPEC_H_

#include <map>
#include <list>
#include <string>

#include <QStringList>

#include "AddressSpec.h"

class InterfaceSpec
{
public:

    bool neg;
    std::string name;
    std::string label;
    std::list<AddressSpec> as;
    std::string hwaddr;
    std::list<std::string> groups;
    
    InterfaceSpec()
    {
        neg = false;
        name = "";
        label = "";
    }

    InterfaceSpec(const InterfaceSpec &other)
    {
        neg = other.neg;
        name = other.name;
        label = other.label;
        as = other.as;
        hwaddr = other.hwaddr;
        groups = other.groups;
    }
    
    InterfaceSpec(bool _neg, const std::string _name)
    {
        neg = _neg;
        name = _name;
    }

    // This function is mostly used in unit tests
    QString toString()
    {
        QStringList str;
        str << "InterfaceSpec";
        str << QString("name:%1").arg(name.c_str());
        str << QString("label:%1").arg(label.c_str());
        str << QString((neg)? "neg:true" : "neg:false");
        str << QString("hwaddr:%1").arg(hwaddr.c_str());

        for (std::list<AddressSpec>::iterator i=as.begin(); i!=as.end(); ++i)
            str << i->toString();
        
        for (std::list<std::string>::iterator i=groups.begin(); i!=groups.end(); ++i)
            str << QString("group:%1").arg((*i).c_str());

        return str.join("|");
    }
};

#endif
