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

#include <assert.h>

#include "Compiler.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/InterfacePolicy.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/MultiAddress.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>

 
using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

FWCompilerException::FWCompilerException(Rule *r,const string &err) : FWException(err)
{
    rule=r;
}


Compiler::~Compiler() {}

int Compiler::cache_objects(FWObject *o)
{
    if ( o->getId() > -1 )  cacheObj(o);

    int n=0;
    for (FWObject::iterator i=o->begin(); i!=o->end(); ++i) {
        n=n+1+cache_objects((*i));
    }
    return n;
}

void Compiler::cacheObj(libfwbuilder::FWObject *o)
{
    objcache[o->getId()]=o;
}


int Compiler::prolog() 
{
    temp=new Group();

    fw->add(temp,false);

/* caching firewall interfaces */
    FWObjectTypedChildIterator j=fw->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {
        Interface *interface_=Interface::cast(*j);
        fw_interfaces[interface_->getId()]=interface_;
    }
    
    fw_id=fw->getId();

    fwopt = fw->getOptionsObject();

/* caching all objects */

    cache_objects( dbcopy );

    return 0;
}

void Compiler::epilog()
{
    cerr << "Compiler::epilog must be overloaded\n";
    exit(1);
}

int    Compiler::getCompiledScriptLength()
{
    return int(output.tellp());
}

string Compiler::getCompiledScript() 
{ 
    string res;
    res=output.str();

/*
 * NB: according to Rogue Wave docs, method  basic_stringbuf::seekpos is public,
 * however implementation that comes with g++ 3.x declares it as protected
 *
 * Method str(const char*) is not described in Rogue Wave docs at
 * all. Stroustrup does not methion it either.
 */
//    output.rdbuf()->seekpos(0);
    output.str("");
    return res;
}

bool Compiler::haveErrorsAndWarnings()
{
    return (int(output.tellp()) > 0);
}

string Compiler::getErrors(const string &comment_sep)
{
    ostringstream ostr;
    istringstream istr(errors_buffer.str());
    while (!istr.eof())
    {
        string tmpstr;
        getline(istr, tmpstr);
        ostr << comment_sep << tmpstr << endl;
    }
    errors_buffer.str("");
    return ostr.str();
}

void Compiler::_init(FWObjectDatabase *_db, const string &fwobjectname)
{ 
    initialized=false;
    _cntr_=1; 

    fw=NULL; 
    temp_ruleset=NULL; 
    combined_ruleset=NULL;

    debug=0;
    debug_rule=-1;
    verbose=true;
    
    dbcopy=new FWObjectDatabase(*_db);  // copies entire tree

    fw=dbcopy->findFirewallByName(fwobjectname);
    if (fw==NULL) {
	cerr << "Firewall object '" << fwobjectname << "' not found \n";
	exit(1);
    }

}

Compiler::Compiler(FWObjectDatabase *_db, 
		   const string &fwobjectname, bool ipv6_policy)
{
    source_ruleset = NULL;
    ruleSetName = "";
    test_mode = false;
    osconfigurator = NULL;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    _init(_db,fwobjectname);
}

Compiler::Compiler(FWObjectDatabase *_db, 
		   const string &fwobjectname, bool ipv6_policy,
		   OSConfigurator *_oscnf)
{
    source_ruleset = NULL;
    ruleSetName = "";
    test_mode = false;
    osconfigurator = _oscnf;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    _init(_db,fwobjectname);
}

// this constructor is used by class Preprocessor, it does not call _init
Compiler::Compiler(FWObjectDatabase*, bool ipv6_policy)
{
    source_ruleset = NULL;
    ruleSetName = "";
    test_mode = false;
    osconfigurator = NULL;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    initialized = false;
    _cntr_ = 1; 
    fw = NULL; 
    temp_ruleset = NULL; 
    combined_ruleset = NULL;
    debug = 0;
    debug_rule = -1;
    verbose = true;
}

string Compiler::createRuleLabel(const std::string &prefix,
                                 const string &txt, int rule_num)
{
    ostringstream  str;

    if (!prefix.empty()) str << prefix << " ";
    str << rule_num << " ";
    str << "(" << txt << ")";
    return str.str();
}

void Compiler::abort(const string &errstr) throw(FWException)
{
    if (test_mode)
        error(errstr);
    else
        throw FWException(errstr);
}

void Compiler::error(const string &errstr)
{
    cout << flush;
    cerr << "Error (" << myPlatformName() << "): ";
    cerr << errstr << endl;
    errors_buffer << "Error (" << myPlatformName() << "): ";
    errors_buffer << errstr << endl;
}

void Compiler::warning(const string &warnstr)
{
    cout << flush;
    cerr << "Warning (" << myPlatformName() << "): ";
    cerr << warnstr << endl;
    errors_buffer << "Warning (" << myPlatformName() << "): ";
    errors_buffer << warnstr << endl;
}

string Compiler::getUniqueRuleLabel()
{
    char str[64];
    sprintf(str,"R_%d",_cntr_);
    _cntr_++;
    return str;
}

void Compiler::compile()
{
    assert(fw);
    assert(combined_ruleset);

}

bool Compiler::MatchesAddressFamily(FWObject *o)
{
    if (Address::cast(o))
    {
        const  InetAddr *inet_addr = Address::cast(o)->getAddressPtr();
        if (inet_addr)
        {
            if (ipv6)
            {
                if (inet_addr->isV6()) return true;
            } else
            {
                if (inet_addr->isV4()) return true;
            }
        } else
        {
            // Address object with no ip address (e.g. dynamic interface
            // or run-time address table)
            return true;
        }
        return false;
    }
    // not an address object at all
    return true;
}


void Compiler::_expand_group_recursive(FWObject *o, list<FWObject*> &ol)
{
/* special case: MultiAddress. This class inherits ObjectGroup, but
 * should not be expanded if it is expanded at run time
 *
 * This is now redundant since we use class MultiAddressRunTime for
 * run-time address tables
 */
    MultiAddress *adt = MultiAddress::cast(o);
    if ((Group::cast(o)!=NULL && adt==NULL) ||
        (adt!=NULL && adt->isCompileTime()))
    {
	for (FWObject::iterator i2=o->begin(); i2!=o->end(); ++i2)
        {
	    FWObject *o1 = *i2;
	    if (FWReference::cast(o1)!=NULL)
                o1=FWReference::cast(o1)->getPointer();
	    assert(o1);

	    _expand_group_recursive(o1, ol);
	}
    } else
    {
        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID)
        {
            o->ref();
            ol.push_back( o );
        } else
        {
            if (Address::cast(o) && Address::cast(o)->hasInetAddress())
            {
                if (MatchesAddressFamily(o))
                {
                    o->ref();
                    ol.push_back( o );
                }
            } else
            {
                // not an address object at all
                o->ref();
                ol.push_back( o );
            }
        }
    }
}

/**
 *   object 's' here is really src or dst or srv. Its children objects
 *   should all be references
 */
void Compiler::expandGroupsInRuleElement(RuleElement *s)
{
    list<FWObject*> cl;
    for (FWObject::iterator i1=s->begin(); i1!=s->end(); ++i1)
    {
        FWObject *o= *i1;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        assert(o);
        _expand_group_recursive(o, cl);
    }

    s->clearChildren();
    //s->setAnyElement();

    for(FWObject::iterator i2=cl.begin(); i2!=cl.end(); ++i2)
    {
        s->addRef( *i2 );
    }
}

void Compiler::_expand_addr_recursive(Rule *rule, FWObject *s,
                                      list<FWObject*> &ol)
{
    Interface *rule_iface = fw_interfaces[rule->getInterfaceId()];
    bool on_loopback= ( rule_iface && rule_iface->isLoopback() );

    list<FWObject*> addrlist;

    for (FWObject::iterator i1=s->begin(); i1!=s->end(); ++i1) 
    {
	FWObject *o= *i1;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	assert(o);

        // this condition includes Host, Firewall and Interface
        if (Address::cast(o) &&
            !Address::cast(o)->hasInetAddress())
        {
            addrlist.push_back(o);
            continue;
        }

        // IPv4, IPv6, Network, NetworkIPv6
        if (Address::cast(o) &&
            Address::cast(o)->hasInetAddress() &&
            MatchesAddressFamily(o))
        {            
            addrlist.push_back(o);
            continue;
        }

        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID ||
            MultiAddress::cast(o)!=NULL ||
            Interface::cast(o) ||
            physAddress::cast(o))
        {
            addrlist.push_back(o);
            continue;
        }
    }

    if (addrlist.empty())
    {
        if (RuleElement::cast(s)==NULL) ol.push_back(s);
    }
    else
    {
        for (list<FWObject*>::iterator i2=addrlist.begin();
             i2!=addrlist.end(); ++i2)
        {
            if (Interface::cast(*i2)!=NULL)
            {
                Interface *interface_=Interface::cast(*i2);
/*
 * Special case is loopback interface - skip it, but only if this rule is
 * not attached to loopback!
 */
                if ( ! on_loopback && interface_->isLoopback() ) continue;

                _expandInterface(interface_,ol);

                continue;
            }
            _expand_addr_recursive(rule, *i2, ol);
        }
    }
}

void Compiler::_expandInterface(Interface *iface,  std::list<FWObject*> &ol)
{
/*
 * if this is unnumbered interface or a bridge port, then do not use it
 */
    if (iface->isUnnumbered() || iface->isBridgePort()) return;

/*
 * if this is an interface with dynamic address, then simply use it
 * (that is, do not use its children elements "Address")
 */
    if (iface->isDyn())
    {
        ol.push_back(iface);
        return;
    }

//    physAddress *pa=iface->getPhysicalAddress();
/*
 * we use physAddress only if Host option "use_mac_addr_filter" of the
 * parent Host object is true
 */
    FWObject  *p;
    FWOptions *hopt;
    p=iface->getParent();
    bool use_mac= (Host::cast(p)!=NULL && 
                   (hopt=Host::cast(p)->getOptionsObject())!=NULL &&
                   hopt->getBool("use_mac_addr_filter") ); 


    for (FWObject::iterator i1=iface->begin(); i1!=iface->end(); ++i1) 
    {
	FWObject *o= *i1;

        if (physAddress::cast(o)!=NULL)
        {
            if (use_mac) ol.push_back(o);
            continue;
        }

        if (Address::cast(o)!=NULL && MatchesAddressFamily(o)) ol.push_back(o);
    }

//    FWObjectTypedChildIterator j=iface->findByType(IPv4::TYPENAME);
//    for ( ; j!=j.end(); ++j ) 
//        ol.push_back(*j);
}

/**
 * internal: scans children of 's' and, if found host or firewall with
 * multiple interfaces, replaces reference to that host or firewall
 * with a set of references to its interfaces. Argument 's' should be 
 * a pointer at either src or dst in the rule
 *
 */
void Compiler::_expandAddr(Rule *rule, FWObject *s) 
{
    list<FWObject*> cl;

    _expand_addr_recursive(rule, s, cl);

    s->clearChildren();

    for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
    {
#if 0
        cerr << "Compiler::_expandAddr: adding object: (*i1)->name=";
        cerr << (*i1)->getName();
        cerr << "  (*i1)->id=" << (*i1)->getId();
        cerr << "  type=" << (*i1)->getTypeName();
        cerr << endl;
#endif        
        s->addRef( *i1 );
    }
}

/**
 * replace address range objects in the rule element 're' with series of
 * regular address obejcts. Drop objects that do not match current
 * address family.
 */
void Compiler::_expandAddressRanges(Rule*, FWObject *re)
{
    list<FWObject*> cl;
    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
	FWObject *o= *i1;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	assert(o!=NULL);

        // if this is address range, check if it matches current address
        // family. If it is not address range, put it back into the rule element
        // If it is address range but it does not match address family,
        // throw it away.
	if (AddressRange::cast(o)!=NULL)
        {
            if (MatchesAddressFamily(o))
            {
                InetAddr a1 = AddressRange::cast(o)->getRangeStart();
                InetAddr a2 = AddressRange::cast(o)->getRangeEnd();
                vector<InetAddrMask> vn = 
                    libfwbuilder::convertAddressRange(a1,a2);

                for (vector<InetAddrMask>::iterator i=vn.begin();
                     i!=vn.end(); i++)
                {
                    Network *h;
                    h= Network::cast(dbcopy->create(Network::TYPENAME) );
                    h->setName(string("%n-")+(*i).toString()+string("%") );
                    h->setNetmask(*(i->getNetmaskPtr()));
                    h->setAddress(*(i->getAddressPtr()));
                    cacheObj(h); // to keep cache consistent
                    dbcopy->add(h,false);
                    cl.push_back(h);
                }
            }
	} else
        {
            cl.push_back(o);
        }
    }

    re->clearChildren();
    for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1)
        re->addRef( *i1 );
}

void Compiler::normalizePortRange(int &rs,int &re)
{
    if (rs<0) rs=0;
    if (re<0) re=0;
    if (rs!=0 && re==0) re=rs;
}


bool Compiler::_complexMatchWithAddress(const InetAddr *obj1_addr,
                                        Interface *iface,
                                        const string &address_type,
                                        bool recognize_broadcasts)
{
    FWObjectTypedChildIterator k = iface->findByType(address_type);
    for ( ; k!=k.end(); ++k ) 
    {
        Address *addr_obj = Address::cast(*k);
                    
        if ( *(addr_obj->getAddressPtr())==*(obj1_addr) ) return true;
        const InetAddr *addr = addr_obj->getAddressPtr();
        const InetAddr *netm = addr_obj->getNetmaskPtr();
        if (addr)
        {
            InetAddrMask n(*addr, *netm);
/*
 * bug #1040773: need to match network address as well as
 * broadcast. Packets sent to the network address (192.168.1.0 for net
 * 192.168.1.0/24) go in the broadcast frame and behave just like IP
 * broadcast packets (sent to 192.168.1.1255 for the same net)
 *
 */
            if (recognize_broadcasts && (
                    *(n.getNetworkAddressPtr())==*(obj1_addr) ||
                    *(n.getBroadcastAddressPtr())==*(obj1_addr)
                )) return true;
        }
    }
    return false;
}

bool Compiler::_complexMatchWithInterface(Address   *obj1,
                                          Interface *iface,
                                          bool recognize_broadcasts)
{
    const InetAddr *obj1_addr = obj1->getAddressPtr();

    if ( physAddress::isA(obj1))
    {
        physAddress *obj1_pa =physAddress::cast(obj1);
        physAddress *iface_pa=iface->getPhysicalAddress();
        if (iface_pa!=NULL && 
            obj1_pa->getPhysAddress()==iface_pa->getPhysAddress()) return true;
    }

    if ( iface->isRegular() )
    {
        string address_type = (ipv6) ? IPv6::TYPENAME : IPv4::TYPENAME;
        return _complexMatchWithAddress(
            obj1_addr, iface, address_type, recognize_broadcasts);
    }
    return false;
}

/**
 * returns true if :
 * 1. obj1 is the same as obj2, or 
 * 2. any child of obj2 or 
 * 3. its address matches that of any obj2's interfaces, or 
 * 4. its address matches broadcast address of one of the interfaces
 * 5. address of obj1 is a broadcast (255.255.255.255)
 * 6. address of obj1 is a multicast ( added 09/15/02, --vk )
 */
bool Compiler::complexMatch(Address *obj1,
                            Address *obj2,
                            bool recognize_broadcasts,
                            bool recognize_multicasts)
{
    if (Network::isA(obj1))
    {
        /*
         * bug #1055937: "Any->all_multicasts not in INPUT Chain"
         * Need to check for multicast networks. We assume they always
         * match if obj2 is firewall
         */
        Network *nobj1 = Network::cast(obj1);
        const InetAddr *inet_addr = nobj1->getAddressPtr();
        if (inet_addr)
        {
            if (inet_addr->isMulticast() && Firewall::isA(obj2))
                return true;

            /*
             * need to check for network object with mask 255.255.255.255
             * Such objects are created by the method that expands address
             * ranges, and some often used ranges trigger that (like
             * "255.255.255.255-255.255.255.255" or "0.0.0.0-0.0.0.0")
             */
            if (!nobj1->getNetmaskPtr()->isHostMask())
                return false;
        } else
            warning(string("Network object with no InetAddr: id=") +
                    FWObjectDatabase::getStringId(obj1->getId()) +
                    "  type=" + obj1->getTypeName() +
                    "  name=" + obj1->getName());
    }
/*
 * if we are matching two firewalls, the positive match is only if
 * their IDs are the same (so it is the same firewall)
 */
    if (Firewall::isA(obj1) && Firewall::isA(obj2))
        return (obj1->getId()==obj2->getId());
/*
 *  match only if object 'obj1' is Network, AddressRange or Host/Firewall
 *  with one interface. Simple and general rule is object 'obj1' should
 *  have zero or one interface child object.
 */
    list<FWObject*> l;
    l=obj1->getByType(Interface::TYPENAME);
    if (l.size()>1) return false;

/*
 *  if obj1 is Interface, match only if it a child of obj2, otherwise
 *  match if it has single address
 */
//    if (obj1->getId()==obj2->getId() ||
//        obj2->getById( obj1->getId() , true )!=NULL ) return true;


    if (obj1->getId()==obj2->getId()) return true;

    FWObject *p=obj1;
    while ( (p=p->getParent())!=NULL )  // it is faster to search this way
        if (p->getId()==obj2->getId()) return true;

    // TODO: this can be expensive since we create and return temporary
    // list object only to get its size. Need method FWObject::countByType
    if ((obj1->getByType(IPv4::TYPENAME)).size()>1) return false;

    const InetAddr *obj1_addr = obj1->getAddressPtr();
    // obj1_addr may be NULL if obj1 does not have any real address,
    // one case when this happens is when obj1 is physAddress
    if (obj1_addr)
    {
        if (!obj1_addr->isAny() && 
            ( (recognize_broadcasts && obj1_addr->isBroadcast()) || 
              (recognize_multicasts && obj1_addr->isMulticast()) )
        ) return true;

        if (Interface::cast(obj1)!=NULL && !Interface::cast(obj1)->isRegular())
            return false;

        if (Interface::cast(obj2)!=NULL)
        {
            return _complexMatchWithInterface(obj1,Interface::cast(obj2));
        }else
        {
            FWObjectTypedChildIterator j=obj2->findByType(Interface::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Interface *iface=Interface::cast(*j);
                if (_complexMatchWithInterface(obj1,iface)) return true;
            }
        }
    }

    return false;
}

/**
 * This method finds interface of obj2 (which is usually
 * firewall object, but not necessarily so) which is connected
 * to the subnet on which obj1 is located.
 */
Interface* Compiler::findInterfaceFor(const Address *obj1, const Address *obj2)
{
    FWObjectTypedChildIterator j=obj2->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j ) 
    {
	Interface *iface=Interface::cast(*j);
	assert(iface);

        if (iface->getId()      == obj1->getId() )      return iface;

        if ( iface->isRegular() && obj1->getAddressPtr() != NULL)
        {
            FWObjectTypedChildIterator k=iface->findByType(IPv4::TYPENAME);
            for ( ; k!=k.end(); ++k ) 
            {
                Address *addr = Address::cast(*k);
                assert(addr);

                if ((*k)->getId() == obj1->getId() ) return iface;

                if (*(addr->getAddressPtr()) == *(obj1->getAddressPtr()) )
                    return iface;

                if (Network::constcast(obj1)!=NULL && 
                    obj1->belongs(*(addr->getAddressPtr()))) return iface; 

                if ( addr->belongs( *(obj1->getAddressPtr()) ) ) return iface;
            }
        }
    }
    return NULL;
}

FWObject* Compiler::findAddressFor(const Address *obj1,const Address *obj2)
{
    FWObjectTypedChildIterator j=obj2->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j ) 
    {
	Interface *iface=Interface::cast(*j);
	assert(iface);

        if (iface->getId()      == obj1->getId() )      return iface;

        if ( iface->isRegular() )
        {
            FWObjectTypedChildIterator k=iface->findByType(IPv4::TYPENAME);
            for ( ; k!=k.end(); ++k ) 
            {
                Address *addr = Address::cast(*k);
                assert(addr);

                if ((*k)->getId() == obj1->getId()) return (*k);

                if (*(addr->getAddressPtr()) == *(obj1->getAddressPtr()) )
                    return (*k);

                if (Network::constcast(obj1)!=NULL &&
                    obj1->belongs(*(addr->getAddressPtr()))) return (*k);

                if (addr->belongs( *(obj1->getAddressPtr()))) return (*k);
            }
        }
    }
    return NULL;
}
        


void Compiler::debugRule()
{
    for(FWObject::iterator i=combined_ruleset->begin(); i!=combined_ruleset->end(); i++) {
	Rule *rule = Rule::cast( *i );

        if ( rule->getPosition()==debug_rule ) {
            cout << debugPrintRule(rule);
            cout << endl;
        }
    }
}

/*
 *  basic rule printing, not very useful. This method is overloaded in
 *  derived classes
 */
string Compiler::debugPrintRule(libfwbuilder::Rule *rule)
{
    return rule->getLabel();
}



/**
 *  adds rule processor to the chain and, if debugging is ON, also
 *  adds rule processor "Debug" after that. Do not add Debug after
 *  certain processors, such as SimplePrintProgress
 */
void Compiler::add(BasicRuleProcessor* rp) 
{ 
    rule_processors.push_back(rp); 
    if (debug_rule>=0  && dynamic_cast<simplePrintProgress*>(rp)==NULL) 
        rule_processors.push_back(new Debug());
}

void Compiler::runRuleProcessors()
{
    list<BasicRuleProcessor*>::iterator i=rule_processors.begin();
    (*i)->setContext(this);
    list<BasicRuleProcessor*>::iterator j=i;
    ++i;
    for ( ; i!=rule_processors.end(); ++i) {
        (*i)->setContext(this);
        (*i)->setDataSource( (*j) );
        j=i;
    }

    while( (*j)->processNext() );
}

void Compiler::deleteRuleProcessors()
{

    rule_processors.clear();
}

Compiler::Begin::Begin() : BasicRuleProcessor("")
{
    init=false;
};

Compiler::Begin::Begin(const std::string &n) : BasicRuleProcessor(n) 
{
    init=false;
};

bool Compiler::Begin::processNext()
{
    assert(compiler!=NULL);
    if (!init)
    {
        for (FWObject::iterator i=compiler->combined_ruleset->begin();
             i!=compiler->combined_ruleset->end(); ++i)
        {
            Rule *rule=Rule::cast(*i);

            Rule  *r= Rule::cast(compiler->dbcopy->create(rule->getTypeName()) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);

            tmp_queue.push_back( r );
        }
        init=true;
        if (!name.empty()) cout << " " << name << endl << flush;

        return true;
    }
    return false;
}

bool Compiler::printTotalNumberOfRules::processNext()
{
    assert(compiler!=NULL);
    assert(prev_processor!=NULL);

    slurp();
    if (tmp_queue.size()==0) return false;
    if (compiler->verbose) cout << " processing " << tmp_queue.size() << " rules" << endl << flush;
    return true;
}

bool Compiler::createNewCompilerPass::processNext()
{
    assert(compiler!=NULL);
    assert(prev_processor!=NULL);

    slurp();
    if (tmp_queue.size()==0) return false;
    cout << pass_name << endl << flush;
    return true;
}

bool Compiler::Debug::processNext()
{
    assert(compiler!=NULL);
    assert(prev_processor!=NULL);

    slurp();
    if (tmp_queue.size()==0) return false;

    if (compiler->debug_rule>=0) {
        string n=prev_processor->getName();
        cout << endl << flush;
        cout << "--- "  << n << " " << setw(74-n.length()) << setfill('-') << "-" << flush;

        for (std::deque<Rule*>::iterator i=tmp_queue.begin(); i!=tmp_queue.end(); ++i) {
            Rule *rule=Rule::cast(*i);
            if ( rule->getPosition()==compiler->debug_rule ) {
                cout << compiler->debugPrintRule(rule) << flush;
                cout << endl << flush;
            }
        }
    }
    return true;
}

bool Compiler::simplePrintProgress::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;

    std::string rl=rule->getLabel();
    if (rl!=current_rule_label) {
            
        if (compiler->verbose) {
            std::string s=" rule "+rl+"\n";
            cout << s << flush;
        }

        current_rule_label=rl;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::convertInterfaceIdToStr::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;

    if (rule->getInterfaceStr().empty())
    {
        Interface *iface = compiler->getCachedFwInterface(rule->getInterfaceId());
        string iface_name= (iface!=NULL) ? iface->getName() : "";
        rule->setInterfaceStr( iface_name );
    } else
    {
        if (rule->getInterfaceStr()=="nil") rule->setInterfaceStr("");
    }

    tmp_queue.push_back(rule);
    return true;
}

/**
 *  re_type can be either RuleElementSrc::TYPENAME or RuleElementDst::TYPENAME
 *  or some other rule element
 */
bool Compiler::splitIfRuleElementMatchesFW::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;

    RuleElement    *re=RuleElement::cast(rule->getFirstByType(re_type));
    int nre=re->size();

    list<FWObject*> cl;

    for (list<FWObject*>::iterator i1=re->begin(); nre>1 && i1!=re->end(); ++i1) {

	FWObject *o   = *i1;
	FWObject *obj = NULL;
	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Address *a=Address::cast(obj);
        assert(a!=NULL);

//        InetAddr obj_addr=a->getAddress();

        if (compiler->complexMatch(a,compiler->fw))
        {
	    cl.push_back(o);   // can not remove right now because remove invalidates iterator
            nre--;

	    Rule  *new_rule= Rule::cast(compiler->dbcopy->create(rule->getTypeName()) );
	    compiler->temp_ruleset->add(new_rule);
	    new_rule->duplicate(rule);
            RuleElement *new_re=RuleElement::cast(new_rule->getFirstByType(re_type));
	    new_re->clearChildren();
	    new_re->setAnyElement();
	    new_re->addRef( a );
	    tmp_queue.push_back(new_rule);
        }
        
    }
    if (!cl.empty()) {
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->remove( (*i1) );
    }

    tmp_queue.push_back(rule);

    return true;
}


bool Compiler::equalObj::operator()(FWObject *o)
{
    return o->getId()==obj->getId();
}

bool Compiler::eliminateDuplicatesInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;
    
    if (comparator==NULL)    comparator=new equalObj();

    RuleElement *re=RuleElement::cast(rule->getFirstByType(re_type));

    vector<FWObject*> cl;

    for(list<FWObject*>::iterator i=re->begin(); i!=re->end(); ++i) {
        FWObject *o= *i;
	FWObject *obj = NULL;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

        comparator->set(obj);

        bool found=false;
        for (vector<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
        {
            if ( (*comparator)( (*i1) ) ) { found=true; break; }
        }
        if (!found) cl.push_back(obj);

//        if (std::find_if(cl.begin(),cl.end(),*comparator)==cl.end())
//        {
//            cl.push_back(obj);
//        }
    }
    if (!cl.empty())
    {
        re->clearChildren();
        for (vector<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->addRef( (*i1) );
    }

    tmp_queue.push_back(rule);

    return true;
}

void  Compiler::recursiveGroupsInRE::isRecursiveGroup(int grid, FWObject *obj)
{
    for (FWObject::iterator i=obj->begin(); i!=obj->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (Group::cast(o)!=NULL)
        {
            if (o->getId()==grid)
            {
                compiler->abort("Group '"+o->getName()+"' references itself recursively");
            }
            isRecursiveGroup(grid,o);
            isRecursiveGroup(o->getId(),o);
        }
    }
}

bool Compiler::recursiveGroupsInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    std::list<FWObject*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (Group::cast(o)!=NULL)  isRecursiveGroup(o->getId(),o);
    }

    tmp_queue.push_back(rule);
    return true;
}


/*
 *  counts children of obj recursively (that is, its direct children objects, plus
 *  their children objects, etc.
 */
int  Compiler::emptyGroupsInRE::countChildren(FWObject *obj)
{
    if (obj->size()==0) return 0;
    int n=0;
    for (FWObject::iterator i=obj->begin(); i!=obj->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        // Check if this is a group, if yes, then count its children
        // recursively. Group itself does not count since it can be
        // empty, too.  However if this is MultiAddress object with
        // run-time processing, it does not count as an empty group
        // since we have no way to know at compile time if it will
        // have some addresses at run time. So we just count it as a
        // regular object.

        if (MultiAddress::cast(o)!=NULL && MultiAddress::cast(o)->isRunTime())
            n++;
        else
        {
            if (Group::cast(o)!=NULL)  n += countChildren(o);
            else n++;   // but if it is not a group, then we count it.
        }
    }
    return n;
}

bool Compiler::emptyGroupsInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    std::list<FWObject*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if ( MultiAddress::cast(o)!=NULL && MultiAddress::cast(o)->isRunTime())
            continue;

        //if (Group::cast(o)!=NULL && MultiAddress::cast(o)==NULL && countChildren(o)==0)
        if (Group::cast(o)!=NULL && countChildren(o)==0)
            cl.push_back(o);
    }

    if (!cl.empty())
    {
        if ( compiler->fw->getOptionsObject()->getBool ("ignore_empty_groups") )
        {
            for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
            {
                FWObject *o= *i;
                ostringstream  str;
                str << "Empty group or address table object '"
                    << o->getName()
                    << "' used in the rule "
                    << rule->getLabel();
                re->removeRef(o);
                compiler->warning( str.str() );
            }
            if (re->isAny())
            {
                ostringstream  str;
                str << "After removal of all empty groups and address table objects rule element "
                    << re->getTypeName()
                    << " becomes 'any' in the rule "
                    << rule->getLabel()
                    << endl
                    << "Dropping rule "
                    <<  rule->getLabel()
                    << " because option 'Ignore rules with empty groups' is in effect";
                compiler->warning( str.str() );

                return true; // dropping this rule
            }
        } else
        {
            std::string gr;
            int cntr = 0;
            for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
            {
                FWObject *o= *i;
                if (cntr>0) gr += ",";
                gr += o->getName();
                cntr++;
            }
            string sfx = "";
            if (cntr>0) sfx = "s";

            ostringstream  str;
            str << "Empty group or address table object"
                << sfx
                << " '"
                << gr
                << "' found in the rule "
                << rule->getLabel()
                << " and option 'Ignore rules with empty groups' is off";
            compiler->abort( str.str() );
        }
    }
    tmp_queue.push_back(rule);
    return true;
}


/**
 * swaps MultiAddress objects that require run-time expansion with
 * MultiAddressRunTime equivalents
 */
bool Compiler::swapMultiAddressObjectsInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    list<MultiAddress*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (MultiAddress::cast(o)!=NULL && MultiAddress::cast(o)->isRunTime())
            cl.push_back(MultiAddress::cast(o));
    }

    if (!cl.empty())
    {
        for (list<MultiAddress*>::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            MultiAddress *ma = *i;

            // Need to make sure the ID of the MultiAddressRunTime
            // object created here is stable and is always the same
            // for the same MultiAddress object. In particular this
            // ensures that we reuse tables between policy and NAT rules
            // in compiler for PF. There could be other similar cases
            // (object-group in compielr for pix may be ?)
            string mart_id_str = FWObjectDatabase::getStringId(ma->getId()) +
                "_runtime";
            int mart_id = FWObjectDatabase::registerStringId(mart_id_str);
            MultiAddressRunTime *mart = MultiAddressRunTime::cast(
                compiler->dbcopy->findInIndex(mart_id));
            if (mart==NULL)
            {
                mart = new MultiAddressRunTime(ma);

                // need to ensure stable ID for the runtime object, so
                // that when the same object is replaced in different
                // rulesets by different compiler passes, chosen
                // runtime object has the same ID and is identified as
                // the same by the compiler.

                mart->setId( mart_id );
                compiler->dbcopy->addToIndex(mart);
                compiler->dbcopy->add(mart);
                compiler->cacheObj(mart);
            }
            re->removeRef(ma);
            re->addRef(mart);
        }
        tmp_queue.push_back(rule);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::FindAddressFamilyInRE(FWObject *parent, bool ipv6)
{
    Address *addr = Address::cast(parent);
    if (addr!=NULL)
    {
        const  InetAddr *inet_addr = addr->getAddressPtr();
        if (ipv6)
            return (inet_addr && inet_addr->isV6());
        else
            return (inet_addr && inet_addr->isV4());
    }

    for (FWObject::iterator i=parent->begin(); i!=parent->end(); i++) 
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (FindAddressFamilyInRE(o, ipv6)) return true;
    }
    return false;
}


void Compiler::DropAddressFamilyInRE(RuleElement *rel, bool drop_ipv6)
{
    list<FWObject*> objects_to_remove;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        // skip "Any"
        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID)
            continue;

        if (drop_ipv6)
        {
            if (Address::cast(o) && Address::cast(o)->hasInetAddress())
            {
                const  InetAddr *inet_addr = Address::cast(o)->getAddressPtr();
                if (inet_addr && inet_addr->isV6())
                    objects_to_remove.push_back(o);
            }
        } else
        {
            if (Address::cast(o) && Address::cast(o)->hasInetAddress())
            {
                const  InetAddr *inet_addr = Address::cast(o)->getAddressPtr();
                if (inet_addr && inet_addr->isV4())
                    objects_to_remove.push_back(o);
            }
        }
    }

    for (list<FWObject*>::iterator i = objects_to_remove.begin();
         i != objects_to_remove.end(); ++i)
        rel->removeRef(*i);
}

void Compiler::DropByServiceTypeInRE(RuleElement *rel, bool drop_ipv6)
{
    list<FWObject*> objects_to_remove;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        // skip "Any"
        if (o->getId() == FWObjectDatabase::ANY_SERVICE_ID)
            continue;

        // Note that all service objects except for ICMPService can be
        // used in both ipv4 and ipv6 contexts.
        if (drop_ipv6)
        {
            if (ICMP6Service::isA(o)) objects_to_remove.push_back(o);
        } else
        {
            if (ICMPService::isA(o)) objects_to_remove.push_back(o);
        }
    }

    for (list<FWObject*>::iterator i = objects_to_remove.begin();
         i != objects_to_remove.end(); ++i)
        rel->removeRef(*i);
}


bool Compiler::catchUnnumberedIfaceInRE(RuleElement *re)
{
    bool err=false;
    Interface *iface;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (o==NULL)
        {
            Rule *rule = Rule::cast(re->getParent());
            FWReference *refo = FWReference::cast(*i);
            string errmsg =
                string("catchUnnumberedIfaceInRE: Can't find object ") +
                string("in cache, ID=") +
                FWObjectDatabase::getStringId(refo->getPointerId()) +
                "  rule " + rule->getLabel();
            abort(errmsg);
        }
        err |= ((iface=Interface::cast(o))!=NULL &&
                (iface->isUnnumbered() || iface->isBridgePort())
        );
    }
    return err;
}


Address* Compiler::getFirstSrc(PolicyRule *rule)
{
    RuleElementSrc *src=rule->getSrc();

    FWObject *o=src->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Address* Compiler::getFirstDst(PolicyRule *rule)
{
    RuleElementDst *dst=rule->getDst();

    FWObject *o=dst->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Service* Compiler::getFirstSrv(PolicyRule *rule)
{
    RuleElementSrv *srv=rule->getSrv();

    FWObject *o=srv->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Service::cast(o);
}

Interval* Compiler::getFirstWhen(PolicyRule *rule)
{
    RuleElementInterval *when=rule->getWhen();
    if (when==NULL) return NULL;  // when is optional element

    FWObject *o=when->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Interval::cast(o);
}

Interface* Compiler::getFirstItf(PolicyRule *rule)
{
    RuleElementItf *itf=rule->getItf();
    if (itf==NULL) return NULL;  // itf is optional element

    FWObject *o=itf->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Interface::cast(o);
}

Address* Compiler::getFirstOSrc(NATRule *rule)
{
    RuleElementOSrc *osrc=rule->getOSrc();

    FWObject *o=osrc->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Address* Compiler::getFirstODst(NATRule *rule)
{
    RuleElementODst *odst=rule->getODst();

    FWObject *o=odst->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Service* Compiler::getFirstOSrv(NATRule *rule)
{
    RuleElementOSrv *osrv=rule->getOSrv();

    FWObject *o=osrv->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Service::cast(o);
}



Address* Compiler::getFirstTSrc(NATRule *rule)
{
    RuleElementTSrc *tsrc=rule->getTSrc();

    FWObject *o=tsrc->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Address* Compiler::getFirstTDst(NATRule *rule)
{
    RuleElementTDst *tdst=rule->getTDst();

    FWObject *o=tdst->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Address::cast(o);
}

Service* Compiler::getFirstTSrv(NATRule *rule)
{
    RuleElementTSrv *tsrv=rule->getTSrv();

    FWObject *o=tsrv->front();
    if (o && FWReference::cast(o)!=NULL)
        o=FWReference::cast(o)->getPointer();

    return Service::cast(o);
}

/*
 * Currently (as of v3.0) we just rely on ruleset name to determine
 * that it is "root" policy or nat. We may add an attribute for that 
 * in the future.
 */
bool Compiler::isRootRuleSet(libfwbuilder::RuleSet* ruleset)
{
    string ruleset_name = ruleset->getName();
    return (ruleset_name.empty() || 
            ruleset_name == "Policy" ||
            ruleset_name == "NAT" ||
            ruleset_name == "__main__");
}

