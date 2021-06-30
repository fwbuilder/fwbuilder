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


#include "RoutingCompiler_freebsd.h"
#include "Configlet.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Resources.h"

#include <QStringList>

#include <iostream>
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

QString RoutingCompiler_freebsd::getNextStaticRouteID()
{
    return QString("route_%1").arg(routing_rule_counter++);
}


RoutingCompiler_freebsd::PrintRule::PrintRule(const std::string &name) :
    RoutingCompiler_openbsd::PrintRule(name) 
{ 
}


bool RoutingCompiler_freebsd::PrintRule::processNext()
{
    RoutingCompiler_freebsd* routing_comp =
        dynamic_cast<RoutingCompiler_freebsd*>(compiler);

    FWOptions* options = compiler->fw->getOptionsObject();
    if (options->getBool("generate_rc_conf_file"))
    {
        slurp();
        if (tmp_queue.size()==0) return false;

        QStringList rule_ids;

        for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
        {
            RoutingRule *rule = RoutingRule::cast( *k );
            QString routing_id = routing_comp->getNextStaticRouteID();
            rule_ids << routing_id;
            routing_comp->routing_rules_ids[rule->getId()] = routing_id;
            //rule_ids << FWObjectDatabase::getStringId(rule->getId()).c_str();
        }

        if (rule_ids.size() > 0)
        {
            compiler->output << QString("static_routes=\"%1\"")
                .arg(rule_ids.join(" ")).toStdString()
                             << endl;
        }

        for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
        {
            RoutingRule *rule = RoutingRule::cast( *k );
            compiler->output << RoutingRuleToString(rule) << endl;
        }

        return true;

    } else
        return RoutingCompiler_openbsd::PrintRule::processNext();
}


string RoutingCompiler_freebsd::PrintRule::RoutingRuleToString(RoutingRule *rule,
                                                               bool add_decorations)
{
    RoutingCompiler_freebsd* routing_comp =
        dynamic_cast<RoutingCompiler_freebsd*>(compiler);

    FWOptions* options = compiler->fw->getOptionsObject();
    if (options->getBool("generate_rc_conf_file"))
    {

        RuleElementRDst *dstrel = rule->getRDst();
        Address *dst = Address::cast(FWReference::getObject(dstrel->front()));
        RuleElementRItf *itfrel = rule->getRItf();
        Interface *itf = Interface::cast(FWReference::getObject(itfrel->front()));
        RuleElementRGtw *gtwrel = rule->getRGtw();
        Address *gtw = Address::cast(FWReference::getObject(gtwrel->front()));

        if(dst==nullptr) compiler->abort(rule, "Broken DST");
        
        QStringList command_line;

        if (gtwrel->isAny() && itf != nullptr) command_line << "-interface";

        command_line << _printRDst(rule).c_str();

        if (gtw != nullptr) command_line << _printRGtw(rule).c_str();
        if (itf != nullptr) command_line << _printRItf(rule).c_str();

        QString rule_code = command_line.join(" ");

        if (add_decorations)
        {
            rule_code = QString("route_%1=\"%2\"")
                .arg(routing_comp->routing_rules_ids[rule->getId()])
                .arg(rule_code);
        }

        return rule_code.toUtf8().constData();

    } else
        return RoutingCompiler_openbsd::PrintRule::RoutingRuleToString(
            rule, add_decorations);
}

