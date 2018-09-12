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

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#include "CompilerDriver_pix.h"
#include "PolicyCompiler_pix.h"
#include "OSConfigurator_pix_os.h"

#include <QFileInfo>
#include <QDir>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_pix::CompilerDriver_pix(FWObjectDatabase *db) :
    CompilerDriver(db)
{
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_pix::clone()
{
    CompilerDriver_pix* new_cd = new CompilerDriver_pix(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

string CompilerDriver_pix::protocolInspectorCommands()
{
    OSConfigurator_pix_os oscnf(objdb , locateObject(), false);
    oscnf.prolog();
    return oscnf.getProtocolInspectionCommands();
}

void CompilerDriver_pix::printProlog(QTextStream &file, const string &prolog_code)
{
    file << endl;
    file << "#" << endl;
    file << "# Prolog script" << endl;
    file << "#" << endl;
    file << prolog_code << endl;
    file << "#" << endl;
    file << "# End of prolog script" << endl;
    file << "#" << endl;
}

