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


#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>
#include <string>
#include <cstring>
#include <iomanip>

#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#include "CompilerDriver_ipf.h"

#include <QFileInfo>
#include <QDir>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_ipf::CompilerDriver_ipf(FWObjectDatabase *db) :
    CompilerDriver_pf(db)
{
    have_nat = false;
    have_filter = false;
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_ipf::clone()
{
    CompilerDriver_ipf* new_cd = new CompilerDriver_ipf(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

QString CompilerDriver_ipf::printActivationCommandWithSubstitution(Firewall *fw)
{
    QString script_buffer;
    QTextStream str(&script_buffer, QIODevice::WriteOnly);

    FWObjectTypedChildIterator j=fw->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j ) 
    {
        Interface *iface=Interface::cast(*j);
        if ( iface->isDyn() )
        {
            str << "sed \"s/ (" << iface->getName() << ") "
                << "/ $i_" << iface->getName() << " /\"";
        }
    }
    return script_buffer;
}

