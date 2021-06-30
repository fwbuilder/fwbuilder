/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectManipulator_create_new.cpp 2747 2010-03-20 20:57:09Z a2k $

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



#include "global.h"

#include "ObjectManipulator.h"


using namespace std;
using namespace libfwbuilder;

bool ObjectManipulator::isObjectAllowed(const QString&)
{
    return true;
}


bool ObjectManipulator::isObjectAllowed(FWObject*, FWObject*)
{
    return true;
}

