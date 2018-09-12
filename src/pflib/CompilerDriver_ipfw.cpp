/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include "CompilerDriver_ipfw.h"

#include <QFileInfo>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_ipfw::CompilerDriver_ipfw(FWObjectDatabase *db) :
    CompilerDriver_pf(db)
{
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_ipfw::clone()
{
    CompilerDriver_ipfw* new_cd = new CompilerDriver_ipfw(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

