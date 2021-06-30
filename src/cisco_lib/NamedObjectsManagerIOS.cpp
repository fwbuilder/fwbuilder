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


#include "NamedObjectsManagerIOS.h"
#include "NamedObject.h"
#include "BaseObjectGroup.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Library.h"

#include <sstream>
#include <assert.h>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


NamedObjectsManagerIOS::NamedObjectsManagerIOS(Library *po, Firewall *fw) :
    NamedObjectsManager(po, fw)
{
}

NamedObjectsManagerIOS::~NamedObjectsManagerIOS()
{
}

string NamedObjectsManagerIOS::getClearCommands()
{
    ostringstream output;

    FWObject *object_groups = getObjectGroupsGroup();

    for (FWObject::iterator i=object_groups->begin(); i!=object_groups->end(); ++i)
    {
        BaseObjectGroup *og = dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=nullptr);
        output << "no "  << og->getObjectGroupHeader() << endl;
    }

    return output.str();
}


