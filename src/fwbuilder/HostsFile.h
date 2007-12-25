/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

  $Id: HostsFile.h 808 2004-09-08 05:34:53Z vkurland $


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

#ifndef __HOSTS_FILE_HH_FLAG__
#define __HOSTS_FILE_HH_FLAG__

#include <fwbuilder/FWException.h>
#include <fwbuilder/IPAddress.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace libfwbuilder
{

/**
 * This class is parser for file in hosts(5) format
 * (e.g. /etc/hosts)
 */
class HostsFile
{
    public:

    void parse(const std::string &filename) throw(FWException);
    void parse(std::istream &from) throw(FWException);
    
    // Returns all hosts found
    std::map<IPAddress, std::vector<std::string> > getAll() { return data; }

    private:
    
    std::map<IPAddress, std::vector<std::string> > data;
};

}

#endif // _HOSTS_FILE_HH_

