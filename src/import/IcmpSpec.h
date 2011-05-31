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


#ifndef _ICMP_SPEC_H_
#define _ICMP_SPEC_H_

#include <map>
#include <list>
#include <string>


class IcmpSpec
{
public:
    std::string icmp_type_name;
    std::string icmp_type_int;
    std::string icmp_code_name;
    std::string icmp_code_int;

    IcmpSpec()
    {
        icmp_type_name = "";
        icmp_type_int = "";
        icmp_code_name = "";
        icmp_code_int = "";
    }

    IcmpSpec(const IcmpSpec &other)
    {
        icmp_type_name = other.icmp_type_name;
        icmp_type_int = other.icmp_type_int;
        icmp_code_name = other.icmp_code_name;
        icmp_code_int = other.icmp_code_int;
    }
    
    IcmpSpec(const std::string s1, const std::string s2,
             const std::string s3, const std::string s4)
    {
        icmp_type_name = s1;
        icmp_type_int = s2;
        icmp_code_name = s3;
        icmp_code_int = s4;
    }
};



#endif
