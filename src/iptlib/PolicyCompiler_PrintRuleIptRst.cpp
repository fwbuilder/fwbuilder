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

#include "PolicyCompiler_ipt.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Resources.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <assert.h>

#include <QStringList>

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
string PolicyCompiler_ipt::PrintRuleIptRst::_createChain(const string &chain)
{
    string res;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    if (!minus_n_tracker_initialized) initializeMinusNTracker();

    if ( ipt_comp->minus_n_commands->count(chain)==0 )
    {
        if ( ! compiler->inSingleRuleCompileMode())
            res = ":" + chain + " - [0:0]\n";
	(*(ipt_comp->minus_n_commands))[chain] = true;
    }
    return res;
}

string PolicyCompiler_ipt::PrintRuleIptRst::_startRuleLine()
{            
    return string("-A ");
}

string PolicyCompiler_ipt::PrintRuleIptRst::_endRuleLine()
{            
    return string("\n");
}

string PolicyCompiler_ipt::PrintRuleIptRst::_printRuleLabel(PolicyRule *rule)
{
    bool nocomm =
        Resources::os_res[compiler->fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_comments");

    return compiler->printComment(rule, current_rule_label, "#", nocomm);

#if 0
    ostringstream res;

    string rl=rule->getLabel();
    if (rl!=current_rule_label)
    {
        if (!compiler->inSingleRuleCompileMode() && !nocomm)
        {
            res << "# " << endl;
            res << "# Rule " << rl << endl;
            res << "# " << endl;
        }

/* do not put comment in the script if it is intended for linksys */
        if (!nocomm || compiler->inSingleRuleCompileMode())
        {
            QStringList comm = QString(rule->getComment().c_str()).split("\n");
            foreach(QString line, comm)
            {
                res << "# " << line.toStdString() << endl;
            }
            //res << "# " << endl;
        }
        current_rule_label=rl;
    }
    return res.str();
#endif
}

bool  PolicyCompiler_ipt::PrintRuleIptRst::processNext()
{
    if (print_once_on_top)
    {

        print_once_on_top=false;
    }

    return PolicyCompiler_ipt::PrintRule::processNext();
}

string PolicyCompiler_ipt::PrintRuleIptRst::_declareTable()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    ostringstream res;

    res << "*" << ipt_comp->my_table << endl;

    return res.str();
}

string PolicyCompiler_ipt::PrintRuleIptRst::_commit()
{
    return "COMMIT\n";
}


string PolicyCompiler_ipt::PrintRuleIptRst::_quote(const string &s)
{
    return "\"" + s + "\"";
}

