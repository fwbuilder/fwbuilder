/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "NATCompiler_ipt.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
    _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <fstream>
#include <sstream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */
/*
 *  check and create new chain if needed
 */
string NATCompiler_ipt::PrintRuleIptRst::_createChain(const string &chain)
{
    string res;
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);

    if (!minus_n_tracker_initialized) InitializeMinusNTracker();

    if ( ipt_comp->minus_n_commands->count(chain)==0 )
    {
	res = ":" + chain + " - [0:0]\n";
	(*(ipt_comp->minus_n_commands))[chain] = true;
    }
    return res;
}

string NATCompiler_ipt::PrintRuleIptRst::_startRuleLine()
{            
    return string("-A ");
}

string NATCompiler_ipt::PrintRuleIptRst::_endRuleLine()
{            
    return string("\n");
}

string NATCompiler_ipt::PrintRuleIptRst::_printRuleLabel(NATRule *rule)
{
    ostringstream res;

    bool nocomm=Resources::os_res[compiler->fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_comments");

    string rl=rule->getLabel();
    if (rl!=current_rule_label)
    {
        if (!nocomm)
        {
            res << "# " << endl;
            res << "# Rule " << rl << endl;
            res << "# " << endl;
        }

/* do not put comment in the script if it is intended for linksys */
        if (!nocomm)
        {
            string    comm=rule->getComment();
            string::size_type c1,c2;
            c1=0;
            while ( (c2=comm.find('\n',c1))!=string::npos ) {
                res << "# " << comm.substr(c1,c2-c1) << endl;
                c1=c2+1;
            }
            res << "# " << comm.substr(c1) << endl;
        }
        current_rule_label=rl;
    }
    return res.str();
}

bool  NATCompiler_ipt::PrintRuleIptRst::processNext()
{
    if (print_once_on_top)
    {

        print_once_on_top=false;
    }

    return NATCompiler_ipt::PrintRule::processNext();
}

string NATCompiler_ipt::PrintRuleIptRst::_declareTable()
{
    ostringstream res;
    res << "*nat" << endl;
    return res.str();
}

string NATCompiler_ipt::PrintRuleIptRst::_commit()
{
    return "COMMIT\n";
}


string NATCompiler_ipt::PrintRuleIptRst::_quote(const string &s)
{
    return "\"" + s + "\"";
}

