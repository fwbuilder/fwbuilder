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


#include <QMap>
#include <QString>

namespace libfwbuilder
{
    class FWObject;
    class FWObjectDatabase;
    class Firewall;
    class Interface;
    class Library;
    class Rule;
};

namespace fwcompiler
{
    class Compiler;
};


extern void build_interface_groups(
    libfwbuilder::FWObjectDatabase *dbcopy,
    libfwbuilder::Library *persistent_objects,
    libfwbuilder::Firewall *fw, bool ipv6,
    QMap<QString, libfwbuilder::FWObject*> &regular_interfaces);


extern void expand_interface_with_phys_address(
    fwcompiler::Compiler *compiler,
    libfwbuilder::Rule *rule,
    libfwbuilder::Interface *iface,
    std::list<libfwbuilder::FWObject*> &list_temp,
    std::list<libfwbuilder::FWObject*> &list_result);
