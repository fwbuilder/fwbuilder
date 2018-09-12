/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>

 
using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


int Compiler::prolog() 
{
    temp = new Group();
    temp->setName("Temp Group");
    fw->add(temp, false);
    return 0;
}

void Compiler::epilog()
{
}

void Compiler::abort(const string &errstr)
{
    BaseCompiler::abort(fw, source_ruleset, nullptr, errstr);
}

void Compiler::abort(FWObject *rule, const string &errstr)
{
    BaseCompiler::abort(fw, source_ruleset, rule, errstr);
}

void Compiler::error(const string &errstr)
{
    BaseCompiler::error(fw, source_ruleset, nullptr, errstr);
}

void Compiler::error(FWObject *rule, const string &errstr)
{
    BaseCompiler::error(fw, source_ruleset, rule, errstr);
}

void Compiler::warning(const string &errstr)
{
    BaseCompiler::warning(fw, source_ruleset, nullptr, errstr);
}

void Compiler::warning(FWObject *rule, const string &errstr)
{
    BaseCompiler::warning(fw, source_ruleset, rule, errstr);
}

int Compiler::getCompiledScriptLength()
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

void Compiler::_init(FWObjectDatabase *_db, Firewall *_fw)
{ 
    initialized = false;
    _cntr_ = 1; 
    group_registry = nullptr;

    temp_ruleset = nullptr; 

    debug = 0;
    debug_rule = -1;
    rule_debug_on = false;
    verbose = true;
    single_rule_mode = false;
    single_rule_ruleset_name = "";
    single_rule_position = -1;

    dbcopy = nullptr;
    persistent_objects = nullptr;
    fw = nullptr;
    fwopt = nullptr;
    fw_id = -1;

    if (_db != nullptr && _fw != nullptr)
    {
        assert(_fw->getRoot() == _db);

        dbcopy = _db;
        fw = _fw;
        fwopt = fw->getOptionsObject();
        fw_id = fw->getId();

        // string fw_str_id = FWObjectDatabase::getStringId(_fw->getId());
        // dbcopy = new FWObjectDatabase(*_db);  // copies entire tree
        // fw = Firewall::cast(
        //     dbcopy->findInIndex(FWObjectDatabase::getIntId(fw_str_id)));
        // fwopt = fw->getOptionsObject();
        // fw_id = fw->getId();
    }
}

Compiler::Compiler(FWObjectDatabase *_db, Firewall *fw, bool ipv6_policy)
{
    source_ruleset = nullptr;
    ruleSetName = "";
    osconfigurator = nullptr;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    persistent_objects = nullptr;
    _init(_db, fw);
}

Compiler::Compiler(FWObjectDatabase *_db, Firewall *fw, bool ipv6_policy,
		   OSConfigurator *_oscnf)
{
    source_ruleset = nullptr;
    ruleSetName = "";
    osconfigurator = _oscnf;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    persistent_objects = nullptr;
    _init(_db, fw);
}

// this constructor is used by class Preprocessor, it does not call _init
Compiler::Compiler(FWObjectDatabase*, bool ipv6_policy)
{
    source_ruleset = nullptr;
    ruleSetName = "";
    osconfigurator = nullptr;
    countIPv6Rules = 0;
    ipv6 = ipv6_policy;
    initialized = false;
    _cntr_ = 1; 
    persistent_objects = nullptr;
    fw = nullptr; 
    temp_ruleset = nullptr; 
    debug = 0;
    debug_rule = -1;
    rule_debug_on = false;
    verbose = true;
    single_rule_mode = false;
}

Compiler::~Compiler()
{
    deleteRuleProcessors();
    dbcopy = nullptr;
}

void Compiler::setPersistentObjects(Library* po)
{
    persistent_objects = po;
    dbcopy->reparent(persistent_objects);
    persistent_objects->fixTree();
}

void Compiler::setSourceRuleSet(RuleSet *rs)
{
    FWObject *copy_rs = dbcopy->findInIndex(rs->getId());
    source_ruleset = RuleSet::cast(copy_rs);
}

void Compiler::setSingleRuleCompileMode(const string &rule_id)
{
    if (!rule_id.empty())
    {
        Rule *rule = Rule::cast(
            dbcopy->findInIndex(FWObjectDatabase::getIntId(rule_id)));
        if (rule)
        {
            single_rule_mode = true;
            single_rule_compile_rule = rule;
            single_rule_position = rule->getPosition();
            single_rule_ruleset_name = rule->getParent()->getName();
        }
    }
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

string Compiler::getUniqueRuleLabel()
{
    ostringstream str;
    str << "R_" << _cntr_;
    _cntr_++;
    return str.str();
}

void Compiler::compile()
{
    assert(fw);
}

void Compiler::_expand_group_recursive(FWObject *o, list<FWObject*> &ol)
{
/*
 * ref #50: ignore various Options child objects. In particular this
 * skips ClusterGroupOptions object which is a child of
 * FailoverClusterGroup and StateSyncClusterGroup objects.
 */
    if (FWOptions::cast(o)) return;

/* special case: MultiAddress. This class inherits ObjectGroup, but
 * should not be expanded if it is expanded at run time
 *
 * This is now redundant since we use class MultiAddressRunTime for
 * run-time address tables
 */
    MultiAddress *adt = MultiAddress::cast(o);

    if ((Group::cast(o)!=nullptr && adt==nullptr) ||
        (adt!=nullptr && adt->isCompileTime()))
    {
	for (FWObject::iterator i2=o->begin(); i2!=o->end(); ++i2)
        {
            FWObject *o1 = FWReference::getObject(*i2);
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
            Address *oaddr = Address::cast(o);
            if (oaddr && oaddr->hasInetAddress())
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

/*
 * Common interface to the operation of expanding of a group
 * recursively. This just calls internal function
 * _expand_group_recursive()
 */
void Compiler::expandGroup(FWObject *grp, list<FWObject*> &ol)
{
    for (FWObject::iterator i1=grp->begin(); i1!=grp->end(); ++i1)
    {
        FWObject *o = FWReference::getObject(*i1);
        assert(o);
        _expand_group_recursive(o, ol);
    }
}

/**
 *   object 's' here is really src or dst or srv. Its children objects
 *   should all be references
 */
void Compiler::expandGroupsInRuleElement(RuleElement *s)
{
    list<FWObject*> cl;
    expandGroup(s, cl);

    s->clearChildren();
    //s->setAnyElement();

    cl.sort(FWObjectNameCmpPredicate());

    for(FWObject::iterator i2=cl.begin(); i2!=cl.end(); ++i2)
    {
        if (!s->validateChild(*i2))
            abort(s->getParent(),
                  "Object '" + (*i2)->getName() +
                  "' can not be used in rule element " + s->getTypeName());

        s->addRef( *i2 );
    }
}

void Compiler::_expand_addr_recursive(Rule *rule, FWObject *s,
                                      list<FWObject*> &ol,
                                      bool expand_cluster_interfaces_fully)
{
//    Interface *rule_iface = Interface::cast(dbcopy->findInIndex(rule->getInterfaceId()));
    bool on_loopback = false;

    if (PolicyRule::isA(rule))
    {
        RuleElement *intf_re = PolicyRule::cast(rule)->getItf();
        Interface *rule_iface = 
            Interface::cast(FWObjectReference::getObject(intf_re->front()));
        on_loopback = ( rule_iface && rule_iface->isLoopback() );
    }

    list<FWObject*> addrlist;

    for (FWObject::iterator i1=s->begin(); i1!=s->end(); ++i1) 
    {
        FWObject *o = FWReference::getObject(*i1);
	assert(o);

        Address *addr = Address::cast(o);

        // this condition includes Host, Firewall and Interface
        if (addr && !addr->hasInetAddress())
        {
            addrlist.push_back(o);
            continue;
        }

        // IPv4, IPv6, Network, NetworkIPv6
        if (addr && addr->hasInetAddress() && MatchesAddressFamily(o))
        {            
            addrlist.push_back(o);
            continue;
        }

        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID ||
            MultiAddress::cast(o)!=nullptr ||
            Interface::cast(o) ||
            physAddress::cast(o))
        {
            addrlist.push_back(o);
            continue;
        }
    }
 
    if (addrlist.empty())
    {
        if (RuleElement::cast(s)==nullptr) ol.push_back(s);
    }
    else
    {
        for (list<FWObject*>::iterator i2=addrlist.begin();
             i2!=addrlist.end(); ++i2)
        {
            Interface *i2itf = Interface::cast(*i2);
            if (i2itf)
            {
/*
 * skip copy of the member interface added in CompilerDriver::copyFailoverInterface
 */
                if (i2itf->getBool("member_interface_copy")) continue;
                    
/*
 * Special case is loopback interface - skip it, but only if this rule is
 * not attached to loopback!
 *
 * Correction 10/20/2008: if user put loopback interface object into
 * rule element, keep it. However if we expanded it from a host or
 * firewall object, then skip it unless the rule is attached to
 * loopback interface.
 */
                if (i2itf->isLoopback())
                {
                    if (RuleElement::cast(s) || on_loopback)
                        _expand_interface(
                            rule, i2itf, ol, expand_cluster_interfaces_fully);
                } else
// this is not a loopback interface                
                    _expand_interface(
                        rule, i2itf, ol, expand_cluster_interfaces_fully);

                continue;
            }
            _expand_addr_recursive(rule, *i2, ol, expand_cluster_interfaces_fully);
        }
    }
}

void Compiler::_expand_interface(Rule *rule,
                                 Interface *iface,
                                 std::list<FWObject*> &ol,
                                 bool expand_cluster_interfaces_fully)
{
/*
 * if this is an interface with dynamic address, then simply use it
 * (that is, do not use its children elements "Address")
 */
    if (iface->isDyn())
    {
        ol.push_back(iface);
        return;
    }

/*
 * we use physAddress only if Host option "use_mac_addr_filter" of the
 * parent Host object is true
 */
    FWObject *p = Host::getParentHost(iface);
    //FWObject *p = iface->getParentHost();
    Host *hp = Host::cast(p);
    if (hp==nullptr) return;  // something is very broken
    FWOptions *hopt = hp->getOptionsObject();
    bool use_mac = (hopt!=nullptr && hopt->getBool("use_mac_addr_filter")); 

    for (FWObject::iterator i1=iface->begin(); i1!=iface->end(); ++i1) 
    {
	FWObject *o= *i1;

        if (physAddress::cast(o)!=nullptr)
        {
            if (use_mac) ol.push_back(o);
            continue;
        }

        // Skip bridge ports
        Interface *subint = Interface::cast(o);
        if (subint)
        {
            if (subint->isBridgePort()) continue;
            _expand_interface(rule, subint, ol, expand_cluster_interfaces_fully);
            continue;
        }

        if ( ! iface->isUnnumbered() &&
             Address::cast(o)!=nullptr &&
             MatchesAddressFamily(o)) ol.push_back(o);
    }

    if (expand_cluster_interfaces_fully && iface->isFailoverInterface())
    {
        // See #1234  Cluster failover interface expands to its own addresses,
        // plus addresses of the corresponding member interface

        FailoverClusterGroup *fg = FailoverClusterGroup::cast(
            iface->getFirstByType(FailoverClusterGroup::TYPENAME));

        Interface* member_intf = fg->getInterfaceForMemberFirewall(fw);
        if (member_intf) 
            _expand_interface(rule, member_intf, ol, expand_cluster_interfaces_fully);
        else
        {
            // per #1394, if the cluster interface used in the rule does not
            // belong to the cluster being compiled, expand it to its own
            // address and addresses of all corresponding member interfaces
            for (FWObjectTypedChildIterator it =
                     fg->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
                Interface *other_intf = Interface::cast(FWObjectReference::getObject(*it));
                assert(other_intf);
                _expand_interface(rule, other_intf, ol, expand_cluster_interfaces_fully);
            }
        }
    }
}

bool compare_addresses(FWObject *o1, FWObject *o2)
{
    Address *a1 = Address::cast(o1);
    Address *a2 = Address::cast(o2);
    if (a1 == nullptr || a2 == nullptr)
    {
        // one or both could be MultiAddress objects (e.g. DNSName)
        return o1->getName() < o2->getName();
    }

    const InetAddr *addr1 = a1->getAddressPtr();
    const InetAddr *addr2 = a2->getAddressPtr();
    if (addr1 == nullptr) return true;
    if (addr2 == nullptr) return false;
    return *addr1 < *addr2;
}

/**
 * internal: scans children of 's' and, if found host or firewall with
 * multiple interfaces, replaces reference to that host or firewall
 * with a set of references to its interfaces. Argument 's' should be 
 * a pointer at either src or dst in the rule
 *
 */
void Compiler::_expand_addr(Rule *rule, FWObject *s,
                            bool expand_cluster_interfaces_fully) 
{
    list<FWObject*> cl;
    _expand_addr_recursive(rule, s, cl, expand_cluster_interfaces_fully);
    
    list<FWObject*> expanded_addresses;
    for (FWObject::iterator i=cl.begin(); i!=cl.end(); ++i) 
    {
        expanded_addresses.push_back(*i);
    }

    expanded_addresses.sort(compare_addresses);

    s->clearChildren();

    for (list<FWObject*>::iterator i1=expanded_addresses.begin();
         i1!=expanded_addresses.end(); ++i1)
    {
        s->addRef( *i1 );
    }
}

/**
 * replace address range objects in the rule element 're' with series of
 * regular address obejcts. Drop objects that do not match current
 * address family.
 */
void Compiler::_expandAddressRanges(Rule *rule, FWObject *re)
{
    list<FWObject*> cl;
    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o = FWReference::getObject(*i1);
	assert(o!=nullptr);

        // if this is address range, check if it matches current address
        // family. If it is not address range, put it back into the rule element
        // If it is address range but it does not match address family,
        // throw it away.
        AddressRange *aro = AddressRange::cast(o);
	if (aro)
        {
            if (MatchesAddressFamily(o))
            {
                InetAddr a1 = aro->getRangeStart();
                InetAddr a2 = aro->getRangeEnd();
                vector<InetAddrMask> vn = 
                    libfwbuilder::convertAddressRange(a1,a2);

                if (vn.size() == 0)
                {
                    abort(rule,
                        "Address Range object '" + aro->getName() +
                        "' can not be converted to set of addresses");
                }

                for (vector<InetAddrMask>::iterator i=vn.begin();
                     i!=vn.end(); i++)
                {
                    Network *h = dbcopy->createNetwork();
                    h->setName(string("%n-")+(*i).toString()+string("%") );
                    h->setNetmask(*(i->getNetmaskPtr()));
                    h->setAddress(*(i->getAddressPtr()));
                    persistent_objects->add(h, false);
                    cl.push_back(h);

                    // see GroupRegistry::registerGroupObject()
                    if (group_registry != nullptr)
                    {
                        group_registry->setGroupRegistryKey(
                            h, group_registry->getGroupRegistryKey(aro));
                    }
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

void Compiler::debugRule()
{
    for (FWObject::iterator i=source_ruleset->begin();
         i!=source_ruleset->end(); i++)
    {
	Rule *rule = Rule::cast( *i );
        if (rule == nullptr) continue;
        if (rule_debug_on && rule->getPosition()==debug_rule )
        {
            info(debugPrintRule(rule));
            info("\n");
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
    if (rule_debug_on && dynamic_cast<simplePrintProgress*>(rp)==nullptr) 
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

    while ((*j)->processNext()) ;
}

void Compiler::deleteRuleProcessors()
{
    while (rule_processors.size() > 0)
    {
        BasicRuleProcessor *rp = rule_processors.front();
        rule_processors.pop_front();
        delete rp;
    }
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
    assert(compiler!=nullptr);
    if (!init)
    {
        for (FWObject::iterator i=compiler->source_ruleset->begin();
             i!=compiler->source_ruleset->end(); ++i)
        {
            Rule *rule = Rule::cast(*i);
            if (rule == nullptr) continue;
            if (rule->isDisabled()) continue;
            if (rule->isDummyRule()) {
                compiler->warning(rule, "Rule contains dummy object and is not parsed.");
                continue;
            }


            Rule  *r = Rule::cast(compiler->dbcopy->create(rule->getTypeName()));
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            tmp_queue.push_back( r );
        }
        init = true;

        if (!name.empty()) compiler->info(string(" ") + name);

        return true;
    }
    return false;
}

bool Compiler::printTotalNumberOfRules::processNext()
{
    assert(compiler!=nullptr);
    assert(prev_processor!=nullptr);

    slurp();
    if (tmp_queue.size()==0) return false;
    if (compiler->verbose)
    {
        ostringstream str;
        str << " processing " << tmp_queue.size() << " rules";
        compiler->info(str.str());
    }
    return true;
}

bool Compiler::createNewCompilerPass::processNext()
{
    assert(compiler!=nullptr);
    assert(prev_processor!=nullptr);

    slurp();
    if (tmp_queue.size()==0) return false;
    compiler->info(pass_name);
    return true;
}

bool Compiler::Debug::processNext()
{
    assert(compiler!=nullptr);
    assert(prev_processor!=nullptr);

    slurp();
    if (tmp_queue.size()==0) return false;

    if (compiler->rule_debug_on)
    {
        string n = prev_processor->getName();
        ostringstream str;
        str << endl << "--- "  << n << " " << setw(74-n.length()) << setfill('-') << "-";
        compiler->info(str.str());
        for (std::deque<Rule*>::iterator i=tmp_queue.begin(); i!=tmp_queue.end(); ++i)
        {
            Rule *rule = Rule::cast(*i);
            if (compiler->rule_debug_on && rule->getPosition()==compiler->debug_rule )
            {
                compiler->info(compiler->debugPrintRule(rule));
                compiler->info("\n");
            }
        }
    }
    return true;
}

bool Compiler::singleRuleFilter::processNext()
{
    assert(compiler!=nullptr);
    assert(prev_processor!=nullptr);

    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;

    if (!compiler->single_rule_mode)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (compiler->single_rule_ruleset_name == compiler->ruleSetName &&
        rule->getPosition() == compiler->single_rule_position)
        tmp_queue.push_back(rule);

    return true;
}

bool Compiler::simplePrintProgress::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;

    std::string rl=rule->getLabel();
    if (rl!=current_rule_label) {
            
        if (compiler->verbose) {
            std::string s=" rule "+rl;
            compiler->info(s);
        }

        current_rule_label=rl;
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
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;

    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));
    int nre = re->size();

    list<FWObject*> cl;

    for (list<FWObject*>::iterator i1=re->begin(); nre>1 && i1!=re->end(); ++i1)
    {
        FWObject *obj = FWReference::getObject(*i1);
        Address *a = Address::cast(obj);
        assert(a!=nullptr);

        if (a->getId() == compiler->fw->getId() ||
            a->getInt("parent_cluster_id") == compiler->fw->getId() ||
            compiler->complexMatch(a, compiler->fw))
        {
	    cl.push_back(*i1);
            nre--;
	    Rule  *new_rule = Rule::cast(
                compiler->dbcopy->create(rule->getTypeName()) );
	    compiler->temp_ruleset->add(new_rule);
	    new_rule->duplicate(rule);
            RuleElement *new_re = RuleElement::cast(
                new_rule->getFirstByType(re_type));
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

/*
 * This rule processor replaces firewall object in given rule element
 * with run-time DNSName object with name "self" and source name (A
 * record) set to "self". This is a trick in that when compliers see
 * objects like that in a rule, they just put source name in the
 * generated code verbatim. This is useful for firewall platforms that
 * support keyword "self" (e.g. PF).
 *
 * Always call this RE after splitIfFirewallInSrc or splitIfFirewallInDst
 */
bool Compiler::ReplaceFirewallObjectWithSelfInRE::processNext()
{
    Rule *rule = prev_processor->getNextRule();
    if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1)
    {
        FWObject *obj = FWReference::getObject(*i1);
        if (obj == compiler->fw)
        {
            DNSName *self = DNSName::cast(
                compiler->persistent_objects->findObjectByName(
                    DNSName::TYPENAME, "self"));
            if (self == nullptr)
            {
                self = compiler->dbcopy->createDNSName();
                self->setName("self");
                self->setRunTime(true);
                self->setSourceName("self");
                compiler->persistent_objects->add(self, false);
            }

            re->addRef(self);
            re->removeRef(compiler->fw);
            break;
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::RegisterGroupsAndTablesInRE::processNext()
{
    Rule *rule = prev_processor->getNextRule();
    if (rule==nullptr) return false;

    if (compiler->group_registry != nullptr)
    {
        RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *obj = FWReference::getObject(*i);
            if (ObjectGroup::cast(obj)!=nullptr && obj->size() > 0)
            {
                compiler->registerGroupObject(re, ObjectGroup::cast(obj));
            }
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

void Compiler::registerGroupObject(RuleElement *re, ObjectGroup *grp)
{
    assert(group_registry!=nullptr);
    list<FWObject*> objects;
    expandGroup(grp, objects);
    group_registry->registerGroup(grp, objects);
    group_registry->registerGroupInRE(re, grp);
}

bool Compiler::equalObj::operator()(FWObject *o)
{
    return o->getId()==obj->getId();
}

bool Compiler::singleObjectNegation::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;

    RuleElement *rel = RuleElement::cast(rule->getFirstByType(re_type));
    assert(rel);

    if (rel->getNeg() && rel->size()==1)
    {
        if (rel->getTypeName() == RuleElementItfInb::TYPENAME ||
            rel->getTypeName() == RuleElementItfOutb::TYPENAME ||
            rel->getTypeName() == RuleElementItf::TYPENAME )
        {
            rel->setNeg(false);
            rel->setBool("single_object_negation", true);
        } else
        {
            FWObject *o = rel->front();
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
            Address *reladdr = Address::cast(o);
            if ( reladdr && reladdr->countInetAddresses(true)==1 &&
                 !compiler->complexMatch(reladdr, compiler->fw)) 
            {
                rel->setNeg(false);
                rel->setBool("single_object_negation", true);
            }
        }
    }

    tmp_queue.push_back(rule);
    return true;
}


/*
 * Process negation in the "Interface" rule element. Scan objects in
 * this RE, replace cluster interfaces with interfaces of the member,
 * then replace them with a list of all other interfaces of the member.
 *
 * Note that normally compiler should call
 * replaceClusterInterfaceInItf before calling this processor. This
 * means that this processor should never see cluster interfaces in
 * the RE. However I keep the code that deals with them in place to be
 * able to use this processor without prior call to
 * replaceClusterInterfaceInItf if necessary.
 *
 * TODO: make this code assert() if cluster interface appears in RE/
 *
 * Note that rule processor singleObjectNegationItf deals with single
 * object negation in Interface rule elements.
 */
bool Compiler::fullInterfaceNegationInRE::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *itfre = RuleElement::cast(rule->getFirstByType(re_type));

    if (itfre==nullptr)
        compiler->abort(rule, "Missing interface rule element");

    FWOptions *fwopt = compiler->getCachedFwOpt();

    if (itfre->getNeg())
    {
        // Use getByTypeDeep() to pick subinterfaces (vlans and such)
        list<FWObject*> all_interfaces =
            compiler->fw->getByTypeDeep(Interface::TYPENAME);
        list<FWObject*> work_interfaces;

        // skip unprotected interfaces bug #2710034 "PF Compiler in 3.0.3
        // Unprotected Interface Bug"
        for (FWObject::iterator i=all_interfaces.begin(); i!=all_interfaces.end(); ++i)
        {
            Interface *intf = Interface::cast(*i);
            if (intf == nullptr) continue;
            if (intf->isUnprotected()) continue;
            if (intf->isLoopback()) continue;

            // skip bridge ports, but use them if this is bridging firewall
            if ( ! fwopt->getBool("bridging_fw") && intf->isBridgePort()) continue;

            if (intf->getOptionsObject()->getBool("cluster_interface")) continue;
            work_interfaces.push_back(intf);
        }

        for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
        {
            // Only interface objects are allowed in the "Interface" rule element
            Interface *rule_iface = Interface::cast(FWReference::getObject(*i));
            if (rule_iface == nullptr) continue;
            // If this interface belongs to a cluster (which can only happen
            // if the rule set belongs to  a cluster), then replace it with
            // corresponding interface of the member
            if (rule_iface->isFailoverInterface())
            {
                FailoverClusterGroup *fg = FailoverClusterGroup::cast(
                    rule_iface->getFirstByType(FailoverClusterGroup::TYPENAME));
                if (fg)
                    rule_iface = fg->getInterfaceForMemberFirewall(compiler->fw);
            }
            if (rule_iface) work_interfaces.remove(rule_iface);
        }

        itfre->reset();
        itfre->setNeg(false);
        FWObject::iterator i;
        for (i=work_interfaces.begin(); i!=work_interfaces.end(); ++i)
            itfre->addRef(*i);
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * Call this processor before ItfNegation (and in some compilers,
 * singleItfNegation). 
 */
bool Compiler::replaceClusterInterfaceInItfRE::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *itfre = RuleElement::cast(rule->getFirstByType(re_type));
    if (itfre==nullptr)
        compiler->abort(rule, "Missing interface rule element");

    map<FWObject*, FWObject*> interface_replacement;

    for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
    {
        Interface *member_iface = nullptr;

        // Only interface objects are allowed in the "Interface" rule element
        FWObject *o = FWReference::getObject(*i);
        Interface *rule_iface = Interface::cast(o);
        if (rule_iface == nullptr) continue;
        // If this interface belongs to a cluster (which can only happen
        // if the rule set belongs to  a cluster), then replace it with
        // corresponding interface of the member
        if (rule_iface->isFailoverInterface())
        {
            FailoverClusterGroup *fg = FailoverClusterGroup::cast(
                rule_iface->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (fg)
                member_iface = fg->getInterfaceForMemberFirewall(compiler->fw);
        }
        if (member_iface)
        {
            interface_replacement[rule_iface] = member_iface;
        }
    }

    map<FWObject*, FWObject*>::iterator r;
    for (r = interface_replacement.begin(); r != interface_replacement.end(); ++r)
    {
        itfre->removeRef(r->first);
        itfre->addRef(r->second);
    }

    itfre->sort(FWObjectNameCmpPredicate(true));

    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::eliminateDuplicatesInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;
    
    if (comparator==nullptr) comparator = new equalObj();

    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    list<FWObject*> cl;

    for(list<FWObject*>::iterator i=re->begin(); i!=re->end(); ++i)
    {
        FWObject *obj = FWReference::getObject(*i);
        if (obj == nullptr) continue;

        comparator->set(obj);

        bool found = false;
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
        {
            FWObject *o2 = *i1;
            if ( (*comparator)(o2) ) { found=true; break; }
        }
        if (!found) cl.push_back(obj);
    }
    if (!cl.empty())
    {
        re->clearChildren();
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->addRef( (*i1) );
    }

    tmp_queue.push_back(rule);

    return true;
}

void  Compiler::recursiveGroupsInRE::isRecursiveGroup(int grid, FWObject *obj)
{
    for (FWObject::iterator i=obj->begin(); i!=obj->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        if (Group::cast(o)!=nullptr)
        {
            if (o->getId()==grid || obj->getId()==o->getId())
            {
                compiler->abort(
                    
                        "Group '" + o->getName() + "' references itself recursively");
            }
            isRecursiveGroup(grid,o);
            isRecursiveGroup(o->getId(),o);
        }
    }
}

bool Compiler::recursiveGroupsInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re == nullptr || re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    std::list<FWObject*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        if (Group::cast(o)!=nullptr)  isRecursiveGroup(o->getId(),o);
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
        FWObject *o = FWReference::getObject(*i);

        // Check if this is a group, if yes, then count its children
        // recursively. Group itself does not count since it can be
        // empty, too.  However if this is MultiAddress object with
        // run-time processing, it does not count as an empty group
        // since we have no way to know at compile time if it will
        // have some addresses at run time. So we just count it as a
        // regular object.

        if (MultiAddress::cast(o)!=nullptr && MultiAddress::cast(o)->isRunTime())
            n++;
        else
        {
            if (Group::cast(o)!=nullptr)  n += countChildren(o);
            else n++;   // but if it is not a group, then we count it.
        }
    }
    return n;
}

bool Compiler::emptyGroupsInRE::processNext()
{
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re == nullptr || re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    std::list<FWObject*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
 
        if ( MultiAddress::cast(o)!=nullptr && MultiAddress::cast(o)->isRunTime())
            continue;

        if (Group::cast(o)!=nullptr && countChildren(o)==0)
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
                    << "'";
                re->removeRef(o);
                compiler->warning(rule, str.str());
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
                compiler->warning(rule,  str.str());

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

            ostringstream  str;
            str << "Empty group or address table object"
                << " '"
                << gr
                << "' is used in the rule"
                << " but option 'Ignore rules with empty groups' is off";
            compiler->abort(rule, str.str());
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
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    if (re == nullptr || re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }
    
    list<MultiAddress*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
        if (MultiAddress::cast(o)!=nullptr && MultiAddress::cast(o)->isRunTime())
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
            if (mart==nullptr)
            {
                mart = new MultiAddressRunTime(ma);

                // need to ensure stable ID for the runtime object, so
                // that when the same object is replaced in different
                // rulesets by different compiler passes, chosen
                // runtime object has the same ID and is identified as
                // the same by the compiler.

                mart->setId( mart_id );
                compiler->dbcopy->addToIndex(mart);
                compiler->persistent_objects->add(mart);
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

bool Compiler::expandMultipleAddressesInRE::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast( rule->getFirstByType(re_type) );
    if (re) compiler->_expand_addr(rule, re, true);
    tmp_queue.push_back(rule);
    return true;
}


bool Compiler::checkForObjectsWithErrors::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    
    for (FWObject::iterator it1=rule->begin(); it1!=rule->end(); it1++)
    {
        RuleElement *re = RuleElement::cast(*it1);
        if (re == nullptr || re->isAny()) continue;
        for (FWObject::iterator it2=re->begin(); it2!=re->end(); it2++)
        {
            FWObject *obj = FWReference::getObject(*it2);
            if (obj->getBool(".rule_error"))
            {
                // it is ok to call abort this late in rule
                // processing.  If the error was fatal, the code that
                // encounter it should have called abort() then. If it
                // continued, then we are in test mode and this call to
                // abort will continue too. In the worst case, we end up
                // with duplicate error messages in the test mode.
                compiler->abort(rule, obj->getStr(".error_msg"));
            }
        }
    }

    tmp_queue.push_back(rule);
    return true;
}


bool Compiler::replaceFailoverInterfaceInRE::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast( rule->getFirstByType(re_type) );

    if (re == nullptr || re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    // list of pointers to cluster interfaces used in the RE
    list<Interface*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        Interface *intf = Interface::cast(FWReference::getObject(*i));
        if (intf==nullptr) continue;

        if (intf->isFailoverInterface()) cl.push_back(intf);
        else
        {
            // this could be a copy of cluster interface which belongs
            // to the firewall. This is done in
            // Compiler::processFailoverGroup. Dont use interface name
            // to distinguish cluster interface. Better method is to
            // check for the variable "cluster_interface".
            if (intf->getOptionsObject()->getBool("cluster_interface"))
                cl.push_back(intf);
        }
    }

    if (!cl.empty())
    {
        for (list<Interface*>::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            Interface *intf = *i;

            FailoverClusterGroup *fg = FailoverClusterGroup::cast(
                intf->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (fg)
            {
                Interface *other_interface = fg->getInterfaceForMemberFirewall(compiler->fw);
                re->removeRef(intf);
                re->addRef(other_interface);
            } else 
            {
                string base_interface_id = intf->getOptionsObject()->getStr("base_interface_id");
                if (!base_interface_id.empty())
                {
                    FWObject *base_interface = compiler->dbcopy->findInIndex(
                        FWObjectDatabase::getIntId(base_interface_id));
                    if (base_interface)
                    {
                        re->removeRef(intf);
                        re->addRef(base_interface);
                    }
                }
            }
        }
    }
    tmp_queue.push_back(rule);
    return true;
}

bool Compiler::FindAddressFamilyInRE(FWObject *parent, bool ipv6)
{
    Address *addr = Address::cast(parent);
    if (addr!=nullptr)
    {
        const  InetAddr *inet_addr = addr->getAddressPtr();
        if (ipv6)
            return (inet_addr && inet_addr->isV6());
        else
            return (inet_addr && inet_addr->isV4());
    }

    for (FWObject::iterator i=parent->begin(); i!=parent->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        if (FindAddressFamilyInRE(o, ipv6)) return true;
    }
    return false;
}


void Compiler::DropAddressFamilyInRE(RuleElement *rel, bool drop_ipv6)
{
    list<FWObject*> objects_to_remove;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        // skip "Any"
        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID) continue;

        if (Address::cast(o) && Address::cast(o)->hasInetAddress())
        {
            const  InetAddr *inet_addr = Address::cast(o)->getAddressPtr();
            if (inet_addr)
            {
                if (drop_ipv6 && inet_addr->isV6())
                    objects_to_remove.push_back(o);

                if (!drop_ipv6 && inet_addr->isV4())
                    objects_to_remove.push_back(o);
            }
        }
    }

    for (list<FWObject*>::iterator i = objects_to_remove.begin();
         i != objects_to_remove.end(); ++i)
        rel->removeRef(*i);
}

bool Compiler::dropRuleWithEmptyRE::isREEmpty(Rule *rule,
                                              const std::string &re_type)
{
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));
    return re->size()==0;
}

/*
 * TODO: why rule elements Service, Interface and Time are not checked
 * for policy rules?
 */
bool Compiler::dropRuleWithEmptyRE::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;

    if (PolicyRule::cast(rule) &&
        (isREEmpty(rule, RuleElementSrc::TYPENAME) ||
         isREEmpty(rule, RuleElementDst::TYPENAME)))
    {
        if (!warning_str.empty()) compiler->warning(rule, warning_str);
        return true;
    }

    if (NATRule::cast(rule) && 
        (isREEmpty(rule, RuleElementOSrc::TYPENAME) ||
         isREEmpty(rule, RuleElementODst::TYPENAME) ||
         isREEmpty(rule, RuleElementOSrv::TYPENAME) ||
         isREEmpty(rule, RuleElementTSrc::TYPENAME) ||
         isREEmpty(rule, RuleElementTDst::TYPENAME) ||
         isREEmpty(rule, RuleElementTSrv::TYPENAME)))
    {
        if (!warning_str.empty())compiler->warning(rule, warning_str);
        return true;
    }

    if (RoutingRule::cast(rule) &&
        (isREEmpty(rule, RuleElementRDst::TYPENAME) ||
         isREEmpty(rule, RuleElementRGtw::TYPENAME) ||
         isREEmpty(rule, RuleElementRItf::TYPENAME)))
    {
        if (!warning_str.empty()) compiler->warning(rule, warning_str);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}



void Compiler::DropByServiceTypeInRE(RuleElement *rel, bool drop_ipv6)
{
    list<FWObject*> objects_to_remove;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        // skip "Any"
        if (o->getId() == FWObjectDatabase::ANY_SERVICE_ID) continue;

        Service *svc = Service::cast(o);

        if (svc == nullptr)
        {
            cerr << endl;
            cerr << "Rule " << Rule::cast(rel->getParent())->getLabel()
                 << "  Rule element " << rel->getTypeName()
                 << endl;
            o->dump(true, false);
        }

        assert(svc);

        // Note that all service objects except for ICMPService and
        // CustomService can be used in both ipv4 and ipv6 contexts.
        if (drop_ipv6)
        {
            if (svc->isV6Only()) objects_to_remove.push_back(o);
        } else
        {
            if (svc->isV4Only()) objects_to_remove.push_back(o);
        }
    }

    for (list<FWObject*>::iterator i = objects_to_remove.begin();
         i != objects_to_remove.end(); ++i)
        rel->removeRef(*i);
}

bool Compiler::catchUnnumberedIfaceInRE(RuleElement *re)
{
    bool err = false;
    Interface *iface;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        if (o==nullptr)
        {
            //Rule *rule = Rule::cast(re->getParent());
            FWReference *refo = FWReference::cast(*i);
            string errmsg =
                string("catchUnnumberedIfaceInRE: Can't find object ") +
                string("in cache, ID=") +
                FWObjectDatabase::getStringId(refo->getPointerId());
            abort(re->getParent(), errmsg);
        }
        err |= ((iface=Interface::cast(o))!=nullptr &&
                (iface->isUnnumbered() || iface->isBridgePort())
        );
    }
    return err;
}

Address* Compiler::getFirstSrc(PolicyRule *rule)
{
    RuleElementSrc *src = rule->getSrc();
    FWObject *o = FWReference::getObject(src->front());
    return Address::cast(o);
}

Address* Compiler::getFirstDst(PolicyRule *rule)
{
    RuleElementDst *dst = rule->getDst();
    FWObject *o = FWReference::getObject(dst->front());
    return Address::cast(o);
}

Service* Compiler::getFirstSrv(PolicyRule *rule)
{
    RuleElementSrv *srv = rule->getSrv();
    FWObject *o = FWReference::getObject(srv->front());
    return Service::cast(o);
}

Interval* Compiler::getFirstWhen(PolicyRule *rule)
{
    RuleElementInterval *when = rule->getWhen();
    if (when==nullptr) return nullptr;  // when is optional element
    FWObject *o = FWReference::getObject(when->front());
    return Interval::cast(o);
}

Interface* Compiler::getFirstItf(PolicyRule *rule)
{
    RuleElementItf *itf = rule->getItf();
    if (itf==nullptr || itf->size()==0) return nullptr;  // itf is optional element
    FWObject *o = FWReference::getObject(itf->front());
    return Interface::cast(o);
}

Address* Compiler::getFirstOSrc(NATRule *rule)
{
    RuleElementOSrc *osrc = rule->getOSrc();
    assert(osrc!=nullptr);
    FWObject *o = FWReference::getObject(osrc->front());
    return Address::cast(o);
}

Address* Compiler::getFirstODst(NATRule *rule)
{
    RuleElementODst *odst = rule->getODst();
    assert(odst!=nullptr);
    FWObject *o = FWReference::getObject(odst->front());
    return Address::cast(o);
}

Service* Compiler::getFirstOSrv(NATRule *rule)
{
    RuleElementOSrv *osrv = rule->getOSrv();
    assert(osrv!=nullptr);
    FWObject *o = FWReference::getObject(osrv->front());
    return Service::cast(o);
}

Address* Compiler::getFirstTSrc(NATRule *rule)
{
    RuleElementTSrc *tsrc = rule->getTSrc();
    assert(tsrc!=nullptr);
    FWObject *o = FWReference::getObject(tsrc->front());
    return Address::cast(o);
}

Address* Compiler::getFirstTDst(NATRule *rule)
{
    RuleElementTDst *tdst = rule->getTDst();
    assert(tdst!=nullptr);
    FWObject *o = FWReference::getObject(tdst->front());
    return Address::cast(o);
}

Service* Compiler::getFirstTSrv(NATRule *rule)
{
    RuleElementTSrv *tsrv = rule->getTSrv();
    assert(tsrv!=nullptr);
    FWObject *o = FWReference::getObject(tsrv->front());
    return Service::cast(o);
}

/*
 * Compares given object with firewall or its parent cluster (if any).
 * Compares only IDs of these objects. Relies on class CompilerDriver
 * to set integer variable "parent_cluster_id" in the firewall object
 * if it is a member of a cluster.
 */
bool Compiler::isFirewallOrCluster(FWObject *obj)
{
    int fw_id = fw->getId();
    int cluster_id = fw->getInt("parent_cluster_id");
    return obj->getId() == fw_id || obj->getId() == cluster_id;
}

string Compiler::printComment(Rule *rule, string &prev_rule_label,
                              const std::string &prefix, bool suppress_comment)
{
    ostringstream res;
    string rl = rule->getLabel();
    if (rl != prev_rule_label)
    {
        if ( ! inSingleRuleCompileMode())
        {
            res << prefix << " " << endl;
            res << prefix << " Rule  " << rl << endl;
        }
        string comm = rule->getComment();
        if ( ! suppress_comment && ! comm.empty())
        {
            string::size_type c1, c2;
            c1 = 0;
            while ( (c2 = comm.find('\n', c1)) != string::npos )
            {
                res << prefix << " " << comm.substr(c1, c2 - c1) << endl;
                c1 = c2 + 1;
            }
            string remainder =  comm.substr(c1);
            if (!remainder.empty())
                res << prefix << " " << remainder << endl;
        }

        string err = getErrorsForRule(rule, prefix + " ");
        if (!err.empty()) res << err << endl;

        prev_rule_label = rl;
    }
//    string err = rule->getCompilerMessage();
//    if (!err.empty()) res << prefix << " " << err << endl;
    return res.str();
}

Address* Compiler::correctForCluster(Address *addr)
{
#ifdef DEBUG_FOR_DMZ
    cerr << "Compiler::correctForCluster " << addr << endl;
    if (addr)
    {
        cerr << "  addr: " << addr->getName() << endl;
    }
#endif

    Interface *intf = Interface::cast(addr);

    if (intf)
    {
#ifdef DEBUG_FOR_DMZ
        cerr << "    intf: " << intf->getName()
             << "    isFailoverInterface: " << intf->isFailoverInterface()
             << endl;
#endif
    }

    if (intf && intf->isFailoverInterface())
    {
        FailoverClusterGroup *fg = FailoverClusterGroup::cast(
            intf->getFirstByType(FailoverClusterGroup::TYPENAME));
        if (fg)
        {
            Address *other_intf = fg->getInterfaceForMemberFirewall(fw);
#ifdef DEBUG_FOR_DMZ
            cerr << "    fg: " << fg->getName() << endl;
            cerr << "    other_intf: " << other_intf << endl;
#endif
            if (other_intf) return other_intf;
        }
    }
    return addr;
}

/* keep only rules that have ipv4 addresses in src and dst
 * 
 * This rule processor assumes all groups and multi-address objects
 * have already been expanded.
 *
 * TODO: figure out what to do with rules that have mix of ipv4 and ipv6
 * addresses in different rule elements (such as ipv4 address in odst
 * and ipv6 address in tdst or similar)
 */
bool Compiler::DropRulesByAddressFamilyAndServiceType::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;

    for(FWObject::iterator it=rule->begin(); it!=rule->end(); ++it)
    {
        RuleElement *re = RuleElement::cast(*it);
        if (re == nullptr) continue;  // probably RuleOptions object

        bool orig_any = re->isAny();
        if (orig_any) continue;

        FWObject *first_object = FWReference::getObject(re->front());
        if (Address::cast(first_object) != nullptr)
            compiler->DropAddressFamilyInRE(re, drop_ipv6);

        if (Service::cast(first_object) != nullptr)
            compiler->DropByServiceTypeInRE(re, drop_ipv6);

        if (!orig_any && re->isAny())
        {
            // removing all ipv6 addresses from rule element makes it 'any', drop
            // this rule
            if (!warning_str.empty()) compiler->warning(rule, warning_str);
            return true;
        }
    }

    tmp_queue.push_back(rule);

    return true;
}


