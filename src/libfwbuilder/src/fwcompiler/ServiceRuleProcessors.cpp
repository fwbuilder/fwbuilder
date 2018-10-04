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

#include <assert.h>

#include "Compiler.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;


/*
 *  These rule processors should work for both Policy and NAT rules
 *  without having to build specialized classes inheriting from these.
 */

bool Compiler::groupServices::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string re_type = PolicyRule::isA(rule) ?
        RuleElementSrv::TYPENAME : RuleElementOSrv::TYPENAME;
    RuleElement *re_srv = RuleElement::cast(rule->getFirstByType(re_type));

    if (re_srv->size()==1)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    map<int, list<Service*> > services;

    for (FWObject::iterator i=re_srv->begin(); i!=re_srv->end(); i++)
    {
        Service *s = Service::cast(FWReference::getObject(*i));
        assert(s);

        int proto = groupingCode(s);
        services[proto].push_back(s);
    }

    for (map<int, list<Service*> >::iterator i1=services.begin();
         i1!=services.end(); i1++)
    {
        list<Service*> &sl=(*i1).second;

        Rule *r = Rule::cast(compiler->dbcopy->create(rule->getTypeName()));
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        RuleElement *nsrv = RuleElement::cast(r->getFirstByType(re_type));
        nsrv->clearChildren();

        for (list<Service*>::iterator j=sl.begin(); j!=sl.end(); j++)
        {
           nsrv->addRef( (*j) );
        }

        tmp_queue.push_back(r);
    }
    return true;
}

int Compiler::groupServicesByProtocol::groupingCode(const Service *srv)
{
    return srv->getProtocolNumber();
}

int Compiler::groupTCPUDPServices::groupingCode(const Service *srv)
{
    return ( TCPService::isA(srv) || UDPService::isA(srv));
}

Compiler::separateServiceObject::separateServiceObject(
    const string &name) : BasicRuleProcessor(name)
{
}

bool Compiler::separateServiceObject::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string re_type = PolicyRule::isA(rule) ?
        RuleElementSrv::TYPENAME : RuleElementOSrv::TYPENAME;
    RuleElement *re_srv = RuleElement::cast(rule->getFirstByType(re_type));

    if (re_srv->size()==1)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=re_srv->begin(); i!=re_srv->end(); i++)
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=nullptr);

	if (condition(s))
        {
            Rule *r = Rule::cast(compiler->dbcopy->create(rule->getTypeName()));
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElement *nsrv = RuleElement::cast(r->getFirstByType(re_type));
            nsrv->clearChildren();
            nsrv->addRef( s );
            tmp_queue.push_back(r);
            services.push_back(s);
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	re_srv->removeRef( (*i) );

    if (!re_srv->isAny())
	tmp_queue.push_back(rule);

    return true;
}


/**
 * separate TCP/UDP services that specify source port (can
 * not be used in combination with destination port with
 * multiport)
 */
bool Compiler::separateSrcPort::condition(const Service *srv)
{
    if ( TCPService::isA(srv) || UDPService::isA(srv))
    {
        int srs = TCPUDPService::constcast(srv)->getSrcRangeStart();
        int sre = TCPUDPService::constcast(srv)->getSrcRangeEnd();

        compiler->normalizePortRange(srs,sre);

        return (srs!=0 || sre!=0);
    }
    return false;
}

bool Compiler::separateSrcAndDstPort::condition(const Service *srv)
{
    if ( TCPService::isA(srv) || UDPService::isA(srv))
    {
        int srs = TCPUDPService::constcast(srv)->getSrcRangeStart();
        int sre = TCPUDPService::constcast(srv)->getSrcRangeEnd();
        int drs = TCPUDPService::constcast(srv)->getDstRangeStart();
        int dre = TCPUDPService::constcast(srv)->getDstRangeEnd();

        compiler->normalizePortRange(srs,sre);
        compiler->normalizePortRange(drs,dre);

        return ( (srs!=0 || sre!=0) && (drs!=0 || dre!=0) );
    }
    return false;
}

bool Compiler::separateTCPUDP::condition(const Service *srv)
{
    return ( TCPService::isA(srv) || UDPService::isA(srv));
}

bool Compiler::separateTagged::condition(const Service *srv)
{
    return ( TagService::isA(srv));
}

bool Compiler::separateCustom::condition(const Service *srv)
{
    return ( CustomService::isA(srv));
}

bool Compiler::separateUserServices::condition(const Service *srv)
{
    return ( UserService::isA(srv));
}

bool Compiler::separateTOS::condition(const Service *srv)
{
    const IPService *ip = IPService::constcast(srv);
    return (ip && !ip->getTOSCode().empty());
}

bool Compiler::splitIpOptions::condition(const Service *srv)
{
    const IPService *ip = IPService::constcast(srv);
    return (ip && ip->hasIpOptions());
}

bool Compiler::separateTCPWithFlags::condition(const Service *srv)
{
    const TCPService *s = TCPService::constcast(srv);
    return (s && s->inspectFlags() );
}

bool Compiler::separatePortRanges::condition(const Service *srv)
{
    if ( TCPService::isA(srv) || UDPService::isA(srv) ) 
    {
        unsigned srs = TCPUDPService::constcast(srv)->getSrcRangeStart();
        unsigned sre = TCPUDPService::constcast(srv)->getSrcRangeEnd();
        unsigned drs = TCPUDPService::constcast(srv)->getDstRangeStart();
        unsigned dre = TCPUDPService::constcast(srv)->getDstRangeEnd();

        if (srs!=0 && sre==0) sre = srs;
        if (drs!=0 && dre==0) dre = drs;

/*
 * I also need to separate rules that use "Any UDP" and "Any TCP"
 * objects. These objects have all ports set to zero and iptables code
 * for them should just have "-p udp" or "-p tcp" without any
 * "--source-port" or "--destination-port" specification. Commands
 * like this do not combine with commands that do specify port because
 * they lose their "any udp"/"any tcp" meaning as soon as
 * "--source-port"/"--destination-port" is added.
 */
        if (srs==0 && sre==0 && drs==0 && dre==0) { sre = 65535; dre = 65535; }

        return (srs!=sre || drs!=dre);
    }
    return false;
}







bool Compiler::verifyCustomServices::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string re_type = PolicyRule::isA(rule) ?
        RuleElementSrv::TYPENAME : RuleElementOSrv::TYPENAME;
    RuleElement *re_srv = RuleElement::cast(rule->getFirstByType(re_type));

    tmp_queue.push_back(rule);

    for (FWObject::iterator i=re_srv->begin(); i!=re_srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=nullptr);
	if (CustomService::isA(o) && 
	    CustomService::cast(o)->getCodeForPlatform(compiler->myPlatformName()).empty())
	    throw FWException("Custom service is not configured for the platform '"+compiler->myPlatformName()+"'. Rule "+rule->getLabel());
    }

    return true;
}

bool Compiler::CheckForTCPEstablished::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string re_type = PolicyRule::isA(rule) ?
        RuleElementSrv::TYPENAME : RuleElementOSrv::TYPENAME;
    RuleElement *re_srv = RuleElement::cast(rule->getFirstByType(re_type));

    for (FWObject::iterator i=re_srv->begin(); i!=re_srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        TCPService *s = TCPService::cast( o );
        if (s==nullptr) continue;

        if (s->getEstablished())
            compiler->abort(
                rule, 
                string("TCPService object with option \"established\" "
                       "is not supported by firewall platform \"") +
                compiler->myPlatformName() +
                string("\". Use stateful rule instead."));
    }

    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::CheckForUnsupportedUserService::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string re_type = PolicyRule::isA(rule) ?
        RuleElementSrv::TYPENAME : RuleElementOSrv::TYPENAME;
    RuleElement *re_srv = RuleElement::cast(rule->getFirstByType(re_type));

    for (FWObject::iterator i=re_srv->begin(); i!=re_srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        if (UserService::isA(o))
            compiler->abort(
                rule, 
                string("UserService object is not supported by ") +
                compiler->myPlatformName());
    }

    tmp_queue.push_back(rule);
    return true;
}


