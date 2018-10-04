/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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

#include <stdlib.h>

#include <assert.h>



#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

// --- Management ---

const char *Management::TYPENAME={"Management"};

Management::Management()
{
    // This object does not have standard attributes
    setId(-1);
}

void Management::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("address")));
    if (!n)
    {
        throw FWException(
            "Missing required address attribute in Management element");
    }
    try
    {
        addr = InetAddr(n);
        XMLTools::FreeXmlBuff(n);
    } catch(FWException &ex)
    {
        // try ipv6
        try
        {
            addr = InetAddr(AF_INET6, n);
            XMLTools::FreeXmlBuff(n);
        } catch(FWException &ex)
        {
            XMLTools::FreeXmlBuff(n);
            throw FWException(
                string("Invalid address attribute in Management element: ") +
                ex.toString());
        }
    }
}

xmlNodePtr Management::toXML(xmlNodePtr parent)
{
    setId(-1);
    setStr("address", addr.toString());
    xmlNodePtr me = FWObject::toXML(parent, false);
    
    // to ensure right order we convert these nodes manually
    // also we do not save them if they are empty
    SNMPManagement *sm=getSNMPManagement();
    sm->toXML(me);
//    if(!sm->isEmpty()) sm->toXML(me);

    FWBDManagement *fm=getFWBDManagement();
    fm->toXML(me);
//    if(!fm->isEmpty()) fm->toXML(me);

    PolicyInstallScript *pi=getPolicyInstallScript();
    pi->toXML(me);
//    if(!pi->isEmpty()) pi->toXML(me);
    
    return me;
}

bool Management::cmp(const FWObject *obj, bool recursive)
{
    if (Management::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    const Management *o2=Management::constcast(obj);

    return (addr==o2->addr);
}

bool Management::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();
    return (otype==SNMPManagement::TYPENAME || 
            otype==FWBDManagement::TYPENAME ||
            otype==PolicyInstallScript::TYPENAME 
    );
}


FWObject& Management::shallowDuplicate(const FWObject *o,
                                       bool preserve_id)
{
    const Management *other = dynamic_cast<const Management*>(o);
    addr = other->getAddress();
    FWObject::shallowDuplicate(o, preserve_id);
    return *this;
}

bool Management::isEmpty() const
{
    const SNMPManagement *sm =
        dynamic_cast<SNMPManagement*>(getFirstByType(SNMPManagement::TYPENAME));
    const FWBDManagement *fm =
        dynamic_cast<FWBDManagement*>(getFirstByType(FWBDManagement::TYPENAME));
    const PolicyInstallScript *pi =
        dynamic_cast<PolicyInstallScript*>(
            getFirstByType(PolicyInstallScript::TYPENAME));
    
    return 
	(!pi || pi->isEmpty()) && 
	(!sm || sm->isEmpty()) && 
	(!fm || fm->isEmpty()) &&  
	addr.isAny();
}

PolicyInstallScript *Management::getPolicyInstallScript()
{
    PolicyInstallScript *res = dynamic_cast<PolicyInstallScript*>(
        getFirstByType(PolicyInstallScript::TYPENAME));
    if (!res) add( res= getRoot()->createPolicyInstallScript() );
    //add(res = new PolicyInstallScript());
    return res;
}

SNMPManagement *Management::getSNMPManagement()
{
    SNMPManagement *res = dynamic_cast<SNMPManagement*>(
        getFirstByType(SNMPManagement::TYPENAME));
    if (!res) add( res = getRoot()->createSNMPManagement() );
    //add(res = new SNMPManagement());
    return res;
}

FWBDManagement *Management::getFWBDManagement()
{
    FWBDManagement *res = dynamic_cast<FWBDManagement*>(
        getFirstByType(FWBDManagement::TYPENAME));
    if (!res) add( res = getRoot()->createFWBDManagement() );
    //add(res = new FWBDManagement());
    return res;
}

// --- PolicyInstallScript ---

const char *PolicyInstallScript::TYPENAME={"PolicyInstallScript"};

PolicyInstallScript::PolicyInstallScript()
{
    // This object does not have standard attributes
    setId(-1);
    enabled = false ;
}

bool PolicyInstallScript::isEmpty() const
{
    return command.empty();
}

bool PolicyInstallScript::isEnabled() const
{
    return enabled;
}

void PolicyInstallScript::setEnabled(bool v)
{
    enabled = v;
}

void PolicyInstallScript::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    
    const char *n;
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("command")));
    if(n)
    {
        command=n;
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("arguments")));
    if(n)
    {
        arguments=n;
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("enabled")));
    if(n)
    {
        enabled=(cxx_strcasecmp(n,"True")==0);
        XMLTools::FreeXmlBuff(n);
    } else enabled = false;
    
}

xmlNodePtr PolicyInstallScript::toXML(xmlNodePtr parent)
{
    setId(-1);
    setStr("command", command );
    setStr("arguments", arguments);
    setBool("enabled", enabled);
    return FWObject::toXML(parent);
}

bool PolicyInstallScript::cmp(const FWObject *obj, bool recursive)
{
    if (PolicyInstallScript::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    const PolicyInstallScript *o2=PolicyInstallScript::constcast(obj);

    return (command==o2->command &&
            arguments==o2->arguments &&
            enabled==o2->enabled);
}

FWObject& PolicyInstallScript::shallowDuplicate(const FWObject *o, bool preserve_id)
{
    const PolicyInstallScript *n=dynamic_cast<const PolicyInstallScript *>(o);
    command   =  n->getCommand();
    arguments =  n->getArguments();
    enabled   =  n->isEnabled();
    FWObject::shallowDuplicate(o, preserve_id);
    return *this;
}

const string& PolicyInstallScript::getCommand() const
{
    return command;
}

void PolicyInstallScript::setCommand(const string& s)
{
    command = s;
}

const string& PolicyInstallScript::getArguments() const
{
    return arguments;
}

void PolicyInstallScript::setArguments(const string& s)
{
    arguments = s;
}

// --- SNMPManagement ---

const char *SNMPManagement::TYPENAME={"SNMPManagement"};

SNMPManagement::SNMPManagement()
{
    // This object does not have standard attributes
    enabled = false ;
    setId(-1);
}

bool SNMPManagement::isEmpty() const
{
    return read_community.empty() && write_community.empty();
}

bool SNMPManagement::isEnabled() const
{
    return enabled;
}

void SNMPManagement::setEnabled(bool v)
{
    enabled = v;
}

void SNMPManagement::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    
    const char *n;
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("snmp_read_community")));
    if(n)
    {
        read_community=n;
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("snmp_write_community")));
    if(n)
    {
        write_community=n;
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("enabled")));
    if(n)
    {
        enabled=(cxx_strcasecmp(n,"True")==0);
        XMLTools::FreeXmlBuff(n);
    } else enabled = false;
    
}

xmlNodePtr SNMPManagement::toXML(xmlNodePtr parent)
{
    setId(-1);
    setStr("snmp_read_community", read_community );
    setStr("snmp_write_community", write_community);
    setBool("enabled", enabled);
    return FWObject::toXML(parent);
}

bool SNMPManagement::cmp(const FWObject *obj, bool recursive)
{
    if (SNMPManagement::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    const SNMPManagement *o2=SNMPManagement::constcast(obj);

    return (read_community==o2->read_community &&
            write_community==o2->write_community &&
            enabled==o2->enabled);
}

FWObject& SNMPManagement::shallowDuplicate(const FWObject *o, bool preserve_id)
{
    const SNMPManagement *n=dynamic_cast<const SNMPManagement *>(o);
    read_community  =  n->getReadCommunity();
    write_community =  n->getWriteCommunity();
    enabled         =  n->isEnabled();
    FWObject::shallowDuplicate(o, preserve_id);
    return *this;
}

const string& SNMPManagement::getReadCommunity () const
{
    return read_community;
}

void SNMPManagement::setReadCommunity (const string& s)
{
    read_community = s;
}

const string& SNMPManagement::getWriteCommunity() const
{
    return write_community;
}

void SNMPManagement::setWriteCommunity(const string& s)
{
    write_community = s;
}


// --- FWBDManagement ---

const char *FWBDManagement::TYPENAME={"FWBDManagement"};

FWBDManagement::FWBDManagement()
{
    port = -1 ;
    enabled = false ;

    // This object does not have standard attributes
    setId(-1);
}

FWBDManagement::~FWBDManagement()
{
}

bool FWBDManagement::isEmpty() const
{
    return port==-1;
}

bool FWBDManagement::isEnabled() const
{
    return enabled;
}

void FWBDManagement::setEnabled(bool v)
{
    enabled = v;
}

void FWBDManagement::fromXML(xmlNodePtr parent)
{
    const char *n=XMLTools::FromXmlCast(xmlGetProp(parent,XMLTools::ToXmlCast("identity")));
    assert(n!=nullptr);
    identity_id = n;
    XMLTools::FreeXmlBuff(n);
    
    n=XMLTools::FromXmlCast(xmlGetProp(parent,XMLTools::ToXmlCast("port")));
    assert(n!=nullptr);
    port = atoi(n);
    XMLTools::FreeXmlBuff(n);

    enabled=false;
    n=XMLTools::FromXmlCast(xmlGetProp(parent,XMLTools::ToXmlCast("enabled")));
    if(n)
    {
        enabled=(cxx_strcasecmp(n,"True")==0);
        XMLTools::FreeXmlBuff(n);
    }
}

xmlNodePtr FWBDManagement::toXML(xmlNodePtr parent)
{
    setId(-1);
    setInt("port", port);
    setStr("identity", identity_id);
    setBool("enabled", enabled);
    
    xmlNodePtr me = FWObject::toXML(parent, false);
 
    return me;
}

bool FWBDManagement::cmp(const FWObject *obj, bool recursive)
{
    if (FWBDManagement::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    const FWBDManagement *o2=FWBDManagement::constcast(obj);

    return (port==o2->port &&
            identity_id==o2->identity_id &&
            enabled==o2->enabled);
}

FWObject& FWBDManagement::shallowDuplicate(const FWObject *o, bool preserve_id)
{
    const FWBDManagement *n=dynamic_cast<const FWBDManagement *>(o);
#if 0
    cerr << "FWBDManagement::shallowDuplicate" << endl;
    cerr << "this:" << endl;
    dump(false,false);
    cerr << "Port: " << port << endl;
    cerr << "identity_id: " << identity_id << endl;
    cerr << "enabled: " << enabled << endl;
    cerr << endl;

    cerr << "n:" << endl;
    ((FWObject*)n)->dump(false,false);
    cerr << "Port: " << n->port << endl;
    cerr << "identity_id: " << n->identity_id << endl;
    cerr << "enabled: " << n->enabled << endl;
    cerr << endl;
#endif

    port        = n->getPort();
    identity_id = n->getIdentityId();
    enabled     = n->isEnabled();
    FWObject::shallowDuplicate(o, preserve_id);
    return *this;
}

int FWBDManagement::getPort() const
{
    return port;
}

void FWBDManagement::setPort(int x)
{
    port = x;
}

const string &FWBDManagement::getIdentityId() const
{
    return identity_id;
}

void FWBDManagement::setIdentityId(const string &s)
{
    identity_id = s;
}

