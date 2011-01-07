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
    ASA8Object *asa8obj = new ASA8Object(obj);
    output << asa8obj->getCommand().toStdString();
    asa8_object_registry[obj->getId()] = asa8obj;
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

    std::map<int,NATCmd*>::iterator it;
    for (it=pix_comp->nat_commands.begin(); it!=pix_comp->nat_commands.end(); ++it)
    {
        NATCmd *natcmd = it->second;

        pix_comp->addASA8Object(natcmd->o_src);
        pix_comp->addASA8Object(natcmd->o_dst);
        pix_comp->addASA8Object(natcmd->t_addr);
        pix_comp->addASA8Object(natcmd->o_srv);


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
    NATCompiler_asa8 *pix_comp = dynamic_cast<NATCompiler_asa8*>(compiler);
    NATCmd *natcmd = pix_comp->nat_commands[ rule->getInt("nat_cmd") ];
    QStringList cmd;

    cmd << "nat ("
        << natcmd->o_iface->getLabel().c_str()
        << ","
        << natcmd->t_iface->getLabel().c_str()
        << ") ";

    cmd << "source" <<  "dynamic";
    cmd << pix_comp->getASA8Object(natcmd->o_src)->getName();
    cmd << pix_comp->getASA8Object(natcmd->t_addr)->getName();
    cmd << "destination" << "static";
    cmd << pix_comp->getASA8Object(natcmd->o_dst)->getName();
    // the same object second time. If this is different object, the rule
    // defines destination translation as well (DNAT or SDNAT in our terms)
    cmd << pix_comp->getASA8Object(natcmd->o_dst)->getName();
    compiler->output << cmd.join(" ").toStdString();
}

void NATCompiler_asa8::PrintRule::printDNAT(libfwbuilder::NATRule *rule)
{
}


