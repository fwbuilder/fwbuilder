/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Zaliva <lord@crocodile.org>

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



#include "fwbuilder/Service.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWObjectDatabase.h"

using namespace libfwbuilder;
using namespace std;

const char *Service::TYPENAME={"Service"};

string Service::getProtocolName() const
{
    throw FWException("base class Service does not return protocol name");
}

int Service::getProtocolNumber() const
{
    throw FWException("base class Service does not return protocol number");
}

FWReference* Service::createRef()
{
//    FWServiceReference *ref=new FWServiceReference();
    FWServiceReference *ref = getRoot()->createFWServiceReference();
    ref->setPointer(this);
    return ref;
}

bool Service::isAny() const
{ 
    return getId()==FWObjectDatabase::ANY_SERVICE_ID;
}
