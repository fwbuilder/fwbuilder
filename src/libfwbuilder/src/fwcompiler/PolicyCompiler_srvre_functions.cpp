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

#include "PolicyCompiler.h"

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



bool PolicyCompiler::splitServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    if (srv->size()==1)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    map<int, list<Service*> > services;

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        Service *s=Service::cast( o );
        assert(s);

        int proto = s->getProtocolNumber();
        services[proto].push_back(s);
    }

    for (map<int, list<Service*> >::iterator i1=services.begin();
         i1!=services.end(); i1++)
    {
        list<Service*> &sl=(*i1).second;

        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        RuleElementSrv *nsrv=r->getSrv();
        nsrv->clearChildren();

        for (list<Service*>::iterator j=sl.begin(); j!=sl.end(); j++)
        {
           nsrv->addRef( (*j) );
        }

        tmp_queue.push_back(r);
    }
    return true;
}



PolicyCompiler::separateServiceObject::separateServiceObject(
    const string &name) : PolicyRuleProcessor(name)
{
}

bool PolicyCompiler::separateServiceObject::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if (condition(s))
        {
            PolicyRule *r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementSrv *nsrv=r->getSrv();
            nsrv->clearChildren();
            nsrv->addRef( s );
            tmp_queue.push_back(r);
            services.push_back(s);
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}


/**
 * separate TCP/UDP services that specify source port (can
 * not be used in combination with destination port with
 * multiport)
 */
bool PolicyCompiler::separateSrcPort::condition(const Service *srv)
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

bool PolicyCompiler::separateTagged::condition(const Service *srv)
{
    return ( TagService::isA(srv));
}

bool PolicyCompiler::separateUserServices::condition(const Service *srv)
{
    return ( UserService::isA(srv));
}

bool PolicyCompiler::separateTOS::condition(const Service *srv)
{
    const IPService *ip = IPService::constcast(srv);
    return (ip && !ip->getTOSCode().empty());
}

bool PolicyCompiler::splitIpOptions::condition(const Service *srv)
{
    const IPService *ip = IPService::constcast(srv);
    return (ip && ip->hasIpOptions());
}

bool PolicyCompiler::separateTCPWithFlags::condition(const Service *srv)
{
    const TCPService *s = TCPService::constcast(srv);
    return (s && s->inspectFlags() );
}

bool PolicyCompiler::separatePortRanges::condition(const Service *srv)
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







bool PolicyCompiler::verifyCustomServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=NULL);
	if (CustomService::isA(o) && 
	    CustomService::cast(o)->getCodeForPlatform(compiler->myPlatformName()).empty())
	    throw FWException("Custom service is not configured for the platform '"+compiler->myPlatformName()+"'. Rule "+rule->getLabel());
    }

    return true;
}




bool PolicyCompiler::CheckForTCPEstablished::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        TCPService *s = TCPService::cast( o );
        if (s==NULL) continue;

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

bool PolicyCompiler::CheckForUnsupportedUserService::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
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


