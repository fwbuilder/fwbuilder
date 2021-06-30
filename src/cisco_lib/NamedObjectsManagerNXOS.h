/* 

                          Firewall Builder

                 Copyright (C) 2010-2011 NetCitadel, LLC

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

#ifndef _NAMED_OBJECTS_MANAGER_NXOS_HH
#define _NAMED_OBJECTS_MANAGER_NXOS_HH


#include "NamedObjectsManager.h"


namespace libfwbuilder
{
    class Group;
    class Firewall;
    class Library;
};

namespace fwcompiler
{

    class NamedObjectsManagerNXOS : public NamedObjectsManager
    {

public:
        NamedObjectsManagerNXOS(libfwbuilder::Library *persistent_objects,
                               libfwbuilder::Firewall *_fw);
        virtual ~NamedObjectsManagerNXOS();

        virtual std::string getClearCommands();
    };
}

#endif
