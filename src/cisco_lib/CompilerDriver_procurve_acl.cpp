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


#include <assert.h>
#include <string>

#include "CompilerDriver_procurve_acl.h"
#include "PolicyCompiler_procurve_acl.h"

#include "fwbuilder/Resources.h"


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_procurve_acl::CompilerDriver_procurve_acl(FWObjectDatabase *db) :
    CompilerDriver_iosacl(db)
{
    safety_net_install_option_name = "procurve_acl_acl_substitution";
    safety_net_install_acl_addr_option_name = "procurve_acl_acl_temp_addr";
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_procurve_acl::clone()
{
    CompilerDriver_procurve_acl* new_cd = new CompilerDriver_procurve_acl(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

void CompilerDriver_procurve_acl::printProlog(QTextStream &file,
                                              const string &prolog_code)
{
    file << endl;
    file << ";" << endl;
    file << "; Prolog script" << endl;
    file << ";" << endl;
    file << prolog_code << endl;
    file << ";" << endl;
    file << "; End of prolog script" << endl;
    file << ";" << endl;
}

