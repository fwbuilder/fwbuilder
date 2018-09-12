/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

#include <qsettings.h>
#include <qdatetime.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <deque>
#include <vector>
#include <string>


#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>

#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include <assert.h>

#include "fwbedit.h"


using namespace libfwbuilder;
using namespace std;


string getAttributeValue(FWObject *obj, const string &attr_name)
{
    if (attr_name=="ID" || attr_name=="id")
        return FWObjectDatabase::getStringId(obj->getId());
    if (attr_name=="type")
        return obj->getTypeName();
    if (attr_name=="name")
        return obj->getName();
    if (attr_name=="path")
        return obj->getPath();
    if (attr_name=="comment")
        return obj->getComment();

    string objtype = obj->getTypeName();

    if (attr_name=="address")
    {
        if (objtype==IPv4::TYPENAME ||
            objtype==IPv6::TYPENAME ||
            objtype==Network::TYPENAME ||
            objtype==NetworkIPv6::TYPENAME)
        {
            return Address::cast(obj)->getAddressPtr()->toString();
        }
    }

    if (attr_name=="netmask")
    {
        if (objtype==IPv4::TYPENAME || objtype==Network::TYPENAME)
        {
            return Address::cast(obj)->getNetmaskPtr()->toString();
        }

        if (objtype==IPv6::TYPENAME || objtype==NetworkIPv6::TYPENAME)
        {
            ostringstream str;
            str << Address::cast(obj)->getNetmaskPtr()->getLength();
            return str.str();
        }
    }

    if (attr_name=="start_address" && objtype==AddressRange::TYPENAME)
    {
        return AddressRange::cast(obj)->getRangeStart().toString();
    }
    
    if (attr_name=="end_address" && objtype==AddressRange::TYPENAME)
    {
        return AddressRange::cast(obj)->getRangeEnd().toString();
    }

    if (attr_name=="dnsname" && objtype==DNSName::TYPENAME)
    {
        return DNSName::cast(obj)->getSourceName();
    }

    if (TCPUDPService::cast(obj)!=nullptr)
    {
        ostringstream str;
        if (attr_name=="src_range_start") 
            str << TCPUDPService::cast(obj)->getSrcRangeStart();
        if (attr_name=="src_range_end") 
            str << TCPUDPService::cast(obj)->getSrcRangeEnd();
        if (attr_name=="dst_range_start") 
            str << TCPUDPService::cast(obj)->getDstRangeStart();
        if (attr_name=="dst_range_end") 
            str << TCPUDPService::cast(obj)->getDstRangeEnd();
        if (str.tellp()>0) return str.str();
    }

    if (ICMPService::cast(obj)!=nullptr)
    {
        if (attr_name=="icmp_type") return obj->getStr("type");
        if (attr_name=="icmp_code") return obj->getStr("code");
    }

    string av = obj->getStr(attr_name);
    return av;
}

/*
 * find first occurrence of the %attr% macro and replace it with the
 * value of corresponding attribute of the obj. Replacement is done in
 * the same string in place, function returns true if it found and
 * replaced at least one macro, false otherwise
 */
bool replaceFirstMacroInString(string &str, FWObject *obj)
{
    string::size_type n = 0;
    for (n=0; n<str.length(); ++n)
    {
        if (str[n]=='%')
        {
            string::size_type n0 = n;
            string::size_type n1 = n;
            n++;
            while (n<str.length() && str[n]!='%')
                ++n;

            if (n>=str.length()) return false;
            n1 = n;
            string attr_name = str.substr(n0+1, n1-n0-1);
            string attr_value = getAttributeValue(obj, attr_name);
            str.replace(n0, n1-n0+1, attr_value);
            return true;
        }
    }
    return false;
}

void listObject(FWObject *obj, bool list_children, bool recursive,
                const string &list_format,
                bool full_dump, int offset)
{
    int off = offset;
    /*
     * print according to the list_format
     * format macros are attribute names surrounded by %%, like
     * %name% or %address%
     */
    if (!list_children)
    {
        if (full_dump) obj->dump(recursive, false);
        else
        {
            string format = list_format;
            while (replaceFirstMacroInString(format, obj)) ;

            string::size_type n;
            while ( (n=format.find("\\t"))!=string::npos )
                format.replace(n, 2, "\t");
            while ( (n=format.find("\\n"))!=string::npos )
                format.replace(n, 2, "\n");

            cout << string(offset,' ')  << format << endl;
            off += 4;
        }
    }
    if (recursive || list_children)
    {
        for (FWObject::iterator it=obj->begin(); it!=obj->end(); ++it)
            listObject(*it, false, recursive,
                       list_format, full_dump, off);
    }
}


void listObject(FWObjectDatabase *objdb,
                const string &path,
                bool list_children,
                bool recursive,
                const string &list_format,
                bool full_dump)
{
    list<FWObject*> objects;
    findObjects(path, objdb, objects);
    if (objects.size()==0)
    {
        cout << "Object " << path << " not found" << endl;
        exit(-1);
    }

    for (list<FWObject*>::iterator it=objects.begin(); it!=objects.end(); ++it)
        listObject(*it, list_children, recursive, list_format, full_dump, 0);

}

