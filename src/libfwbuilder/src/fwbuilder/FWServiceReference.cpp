/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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



#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Service.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;

const char *FWServiceReference::TYPENAME={"ServiceRef"};

FWServiceReference::FWServiceReference():FWReference(){}

//FWServiceReference::FWServiceReference(Service *p):FWReference(p){}
//FWServiceReference::FWServiceReference(ServiceGroup *p):FWReference(p){}

void FWServiceReference::setPointer(Service *p)
{
    FWReference::setPointer(p);
}


void FWServiceReference::setPointer(ServiceGroup *p)
{
    FWReference::setPointer(p);
}







