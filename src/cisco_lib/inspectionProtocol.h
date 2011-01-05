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

#ifndef INSPECTION_PROTOCOL_HH
#define INSPECTION_PROTOCOL_HH

#include <string>
#include <map>

typedef enum { FIXUP_ENABLE=0, FIXUP_DISABLE=1, FIXUP_SKIP=2, FIXUP_CLEAR=3 } FixupTypes;

/*
 * par1 and par2 are parameters for the inspection protocol. These are
 * port numbers most of the time, but for some protocols the meaning
 * may be different. For example for dns it is "maximum-length".
 */
class InspectionProtocol {
public:

    std::string name;
    std::string printable_name;
    std::string ip_proto;
    int  par1,par2;

    static std::map<std::string,InspectionProtocol*> protocols;

    InspectionProtocol(const std::string &fn,
                       const std::string &prn,
                       const std::string &pn,
                       int p1,
                       int p2)
    {
        name = fn;
        printable_name = prn;
        ip_proto = pn;
        par1 = p1;
        par2 = p2;
        if (protocols.count(fn)==0) protocols[fn] = this;
    }
};

extern InspectionProtocol protocolDefinitions[];

#endif
