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


#ifndef _PORT_SPEC_H_
#define _PORT_SPEC_H_

#include <map>
#include <list>
#include <string>


class PortSpec
{
public:
    std::string port1;
    std::string port2;
    std::string port_op;

    PortSpec()
    { port1 = ""; port2 = ""; port_op = ""; }

    PortSpec(const PortSpec &other)
    {
        port1 = other.port1;
        port2 = other.port2;
        port_op = other.port_op;
    }
    
    PortSpec(const std::string s1, const std::string s2, const std::string s3)
    { port1 = s1; port2 = s2; port_op = s3; }

    void setFromPortRange(const std::string &port_range)
    {
        std::size_t n = port_range.find(':');
        if ( n != std::string::npos )
        {
            port1 = port_range.substr(0, n);
            port2 = port_range.substr(n+1);
            port_op = ":";
        }
    }
    
    std::string toString()
    { return std::string("PortSpec: ") + port_op + " " + port1 + " " + port2; }
};

#endif
