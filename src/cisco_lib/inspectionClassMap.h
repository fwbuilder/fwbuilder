/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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

#ifndef INSPECTION_CLASS_MAP_HH
#define INSPECTION_CLASS_MAP_HH


#include <string>

/*
 *  status:
 *    0:  enable
 *    1:  disable
 *    2:  skip
 */
class InspectionClassMap {

public:
    
    std::string class_map_name;
    std::string fixup_name;
    std::string inspect_name;
    int    status;
    int    port1,port2;
    std::string arg_name;
    int    arg_val;

    InspectionClassMap(const std::string &fn,int s,int p1,int p2,
                       const std::string &a,int v)
    {
        status=s; port1=p1; port2=p2; arg_name=a; arg_val=v;
        std::string ss = fn;
        std::string::size_type k;
        while ( (k=ss.find(" ")) != std::string::npos ) 
            ss.replace(k,1,1,'_');
        inspect_name = ss;
        fixup_name = fn;
        class_map_name = std::string("custom_") + ss + std::string("_inspection");
    }

    bool isDefault();
    std::string getIPProtocol();
    std::string getPrintableName();
    std::string getMatchCommand();
};

#endif
