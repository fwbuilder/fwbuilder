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

#ifndef __HOSTS_FILE_HH_FLAG__
#define __HOSTS_FILE_HH_FLAG__

#include <fwbuilder/FWException.h>
#include <fwbuilder/InetAddr.h>

#include <QStringList>


/**
 * This class is parser for file in hosts(5) format
 * (e.g. /etc/hosts)
 */
class HostsFile
{
    QString file_name;
    
public:

    HostsFile(const QString &file_name) { this->file_name = file_name; }
    
    void parse();
    
    // Returns all hosts found
    std::map<libfwbuilder::InetAddr, QStringList> getAll() { return data; }

    private:
    
    std::map<libfwbuilder::InetAddr, QStringList> data;
};


#endif // _HOSTS_FILE_HH_

