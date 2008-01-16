/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Zaliva     lord@crocodile.org

  $Id: Constants.cpp 808 2004-09-08 05:34:53Z vkurland $


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

#include <fwbuilder/Constants.h>
#include <fwbuilder/libfwbuilder-config.h>

using namespace std;
using namespace libfwbuilder;

const string Constants::getLibraryDescription()
{
    return "Firewall Builder API ";
}

const string Constants::getLibraryVersion() 
{ 
    return string(LIBFWBUILDER_VERSION); 
}

const string Constants::getDataFormatVersion()
{
    return string(LIBFWBUILDER_FORMAT_VERSION); 
}

const string Constants::getTemplateDirectory() 
{ 
    return string(LIBFWBUILDER_TEMPLATE_DIR); 
}
