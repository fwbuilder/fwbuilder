/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "config.h"

#include "NATCompiler_asa8.h"
#include "ASA8Object.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Resources.h"

#include <assert.h>
#include <sstream>

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

void NATCompiler_asa8::addASA8Object(const FWObject *obj)
{
    if (asa8_object_registry[obj->getId()] == NULL)
    {
        ASA8Object *asa8obj = new ASA8Object(obj);
        output << asa8obj->getCommand().toStdString();
        asa8_object_registry[obj->getId()] = asa8obj;
    }
}
        
ASA8Object* NATCompiler_asa8::getASA8Object(const FWObject *obj)
{
    return asa8_object_registry[obj->getId()];
}

bool NATCompiler_asa8::PrintObjectsForNat::processNext()
{
    NATCompiler_asa8 *pix_comp = dynamic_cast<NATCompiler_asa8*>(compiler);
    string version = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");

    slurp();
    if (tmp_queue.size()==0) return false;

    compiler->output << endl;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *rule = NATRule::cast( *k );
        // NATCmd *natcmd = pix_comp->nat_commands[ rule->getInt("nat_cmd") ];

        Address  *osrc = compiler->getFirstOSrc(rule);  assert(osrc);
        Address  *odst = compiler->getFirstODst(rule);  assert(odst);
        Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);
                                      
        Address  *tsrc = compiler->getFirstTSrc(rule);  assert(tsrc);
        Address  *tdst = compiler->getFirstTDst(rule);  assert(tdst);
        Service  *tsrv = compiler->getFirstTSrv(rule);  assert(tsrv);

        pix_comp->addASA8Object(osrc);
        pix_comp->addASA8Object(odst);
        pix_comp->addASA8Object(osrv);
        pix_comp->addASA8Object(tsrc);
        pix_comp->addASA8Object(tdst);
        pix_comp->addASA8Object(tsrv);
    }

    return true;
}

bool NATCompiler_asa8::PrintClearCommands::processNext()
{
    string version = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");

    slurp();
    if (tmp_queue.size()==0) return false;

    compiler->output << endl;

    if ( !compiler->fw->getOptionsObject()->getBool("pix_acl_no_clear") &&
         !compiler->inSingleRuleCompileMode())
    {
        compiler->output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") +
            "version_" + version + "/pix_commands/clear_xlate") << endl;
        compiler->output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") +
            "version_" + version + "/pix_commands/clear_nat") << endl;
        compiler->output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") +
            "version_" + version+"/pix_commands/clear_object") << endl;
    }

    return true;
}


NATCompiler_asa8::PrintRule::PrintRule(const std::string &name) :
    NATCompiler_pix::PrintRule(name) 
{ }


void NATCompiler_asa8::PrintRule::printNONAT(libfwbuilder::NATRule *rule)
{
}

void NATCompiler_asa8::PrintRule::printSNAT(libfwbuilder::NATRule *rule)
{
    printSDNAT(rule);
}

void NATCompiler_asa8::PrintRule::printDNAT(libfwbuilder::NATRule *rule)
{
    printSDNAT(rule);
}

void NATCompiler_asa8::PrintRule::printSDNAT(NATRule *rule)
{
    NATCompiler_asa8 *pix_comp = dynamic_cast<NATCompiler_asa8*>(compiler);
    // NATCmd *natcmd = pix_comp->nat_commands[ rule->getInt("nat_cmd") ];

    QStringList cmd;

    Address  *osrc = compiler->getFirstOSrc(rule);  assert(osrc);
    Address  *odst = compiler->getFirstODst(rule);  assert(odst);
    Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);
                                      
    Address  *tsrc = compiler->getFirstTSrc(rule);  assert(tsrc);
    Address  *tdst = compiler->getFirstTDst(rule);  assert(tdst);
    Service  *tsrv = compiler->getFirstTSrv(rule);  assert(tsrv);

    Interface *o_iface = Interface::cast(compiler->dbcopy->findInIndex(
                                             rule->getInt("nat_iface_orig")));
    Interface *t_iface = Interface::cast(compiler->dbcopy->findInIndex(
                                             rule->getInt("nat_iface_trn")));

    cmd << QString("nat (%1,%2)")
        .arg(o_iface->getLabel().c_str())
        .arg(t_iface->getLabel().c_str());

    cmd << "source";

    if (!tsrc->isAny()) cmd << "dynamic";
    else cmd << "static";

    cmd << pix_comp->getASA8Object(osrc)->getCommandWord();
    cmd << pix_comp->getASA8Object(tsrc)->getCommandWord();

    // only need "destination" part if ODst is not any
    if (!odst->isAny())
    {
        // ASA documentation says destination translation is always "static"
        cmd << "destination" << "static";
        cmd << pix_comp->getASA8Object(odst)->getCommandWord();

        if (tdst->isAny())
            cmd << pix_comp->getASA8Object(odst)->getCommandWord();
        else
            cmd << pix_comp->getASA8Object(tdst)->getCommandWord();
    }

    if (!osrv->isAny())
    {
        cmd << "service";
        cmd << pix_comp->getASA8Object(osrv)->getCommandWord();
        if (tsrv->isAny())
            cmd << pix_comp->getASA8Object(osrv)->getCommandWord();
        else
            cmd << pix_comp->getASA8Object(tsrv)->getCommandWord();
    }

    compiler->output << cmd.join(" ").toStdString() << endl;
}




