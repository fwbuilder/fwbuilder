/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "specialServices.h"
#include "PolicyCompiler_pix.h"

#include "fwbuilder/IPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/CustomService.h"
#include "fwcompiler/Compiler.h"


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


bool SpecialServices::processNext()
{
    PolicyCompiler_pix *pix_comp = dynamic_cast<PolicyCompiler_pix*>(compiler);
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re->size() == 0)
    {
        cerr << "Rule " << rule->getLabel()
             << "rule element " << re_type <<  " is empty" << endl;
        assert(re->size() != 0);
    }

    FWObject *obj = FWReference::getObject(re->front());
    Service *s = Service::cast(obj);

    string version = compiler->fw->getStr("version");

    if (IPService::cast(s)!=nullptr)
    {
	if (s->getBool("short_fragm") ||
	    s->getBool("fragm") )
        {
            if (pix_comp) pix_comp->fragguard = true;
	    return true;   // do not copy the rule
	}
	if (s->getBool("rr")        ||
	    s->getBool("ssrr")      ||
	    s->getBool("ts") )
        {
	    compiler->abort(
                    rule, 
                    "PIX does not support checking for IP options in ACLs.");
            return true;
        }
    }

    if (TCPService::cast(s)!=nullptr)
    {
	if (s->getBool("ack_flag")  ||
	    s->getBool("fin_flag")  ||
	    s->getBool("rst_flag")  ||
	    s->getBool("syn_flag") )
        {
	    compiler->abort(
                    rule, 
                    "PIX does not support checking for TCP options in ACLs.");
            return true;
        }
    }

    if (CustomService::cast(s)!=nullptr && pix_comp==nullptr)
    {
        compiler->abort(
            rule, 
            "CustomService objects are not supported in NAT rules");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

