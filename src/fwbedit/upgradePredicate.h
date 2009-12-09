/* 

                          Firewall Builder

                 Copyright (C) 2003-2009 NetCitadel, LLC

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

#include "fwbuilder/XMLTools.h"

class UpgradePredicate: public libfwbuilder::XMLTools::UpgradePredicate
{
    bool autoupgrade_flag;

public:
    UpgradePredicate(bool autoupgrade) { autoupgrade_flag = autoupgrade; }
    virtual bool operator()(const std::string&) const 
    {
        bool res=false;
        std::cout << "Data file has been created in the old version of Firewall Builder."
                  << std::endl << std::flush;
        if (autoupgrade_flag)
        {
            std::cout << "Do you want to convert it? (Y/n)" << std::endl;
            int a = getchar();
            if (a=='y' || a=='Y' || a=='\n' ) res= true;
        }
        else
        {
            std::cout << "Use option '-u' to upgrade the file in fwbedit."
                " Alternatively, fwbuilder GUI can convert it." << std::endl;
        }
        if (res) std::cout << "Upgrading the file now ..." << std::endl;
        return res;
    }
};

