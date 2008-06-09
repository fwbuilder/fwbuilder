/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: NATCompiler_ipt.cpp 1451 2007-12-09 23:53:22Z vk $

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

#include "NATCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "combinedAddress.h"

#include "fwcompiler/OSConfigurator.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"

#include "config.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

#include <assert.h>
#include <ctype.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

struct subnetInfo {
    Interface *iface;
    IPv4      *ipv4;
    int        nmlength;
    subnetInfo()                           { iface=NULL; ipv4=NULL; nmlength=0; }
    subnetInfo(Interface *i,IPv4 *a,int n) { iface=i;    ipv4=a;    nmlength=n; }
};


#if 0
static int chain_no=0;
#endif

static std::map<std::string,int> tmp_chain_no;
static std::list<std::string> standard_chains;

const std::list<std::string>& NATCompiler_ipt::getStandardChains()
{
    if (standard_chains.size()==0)
    {
        standard_chains.push_back("POSTROUTING");
        standard_chains.push_back("PREROUTING");
        standard_chains.push_back("SNAT");
        standard_chains.push_back("DNAT");
        standard_chains.push_back("MASQUERADE");
        standard_chains.push_back("REDIRECT");
        standard_chains.push_back("NETMAP");
        standard_chains.push_back("LOG");
        standard_chains.push_back("MARK");
        standard_chains.push_back("ACCEPT");
        standard_chains.push_back("REJECT");
        standard_chains.push_back("DROP");
        standard_chains.push_back("RETURN");
        standard_chains.push_back("OUTPUT");
    }
    return standard_chains;
}


string NATCompiler_ipt::myPlatformName() { return "iptables"; }

string NATCompiler_ipt::getInterfaceVarName(FWObject *iface)
{
    ostringstream  ostr;
    string iname=iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos)
        iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    return ostr.str();
}

string NATCompiler_ipt::getAddressTableVarName(FWObject *at)
{
    ostringstream  ostr;
    string name=at->getName();
    string::size_type p1;
    char *bad_shell_chars = " !#$&*()-+=\\|{}[]?<>,.";
    for (char *cptr=bad_shell_chars; *cptr; cptr++)
    {
        while ( (p1=name.find(*cptr))!=string::npos)
            name=name.replace(p1,1,"_");
    }
    ostr << "at_" << name;
    return ostr.str();
}

string NATCompiler_ipt::getNewTmpChainName(NATRule *rule)
{
    std::ostringstream str;
    string chain_id=rule->getUniqueId();
    int    n=tmp_chain_no[chain_id];

    str << "C" << chain_id;
    str << "." <<  setw(1) << setfill('0') << n;

    n++;
    tmp_chain_no[chain_id]=n;
    return str.str();

#if 0
    std::ostringstream str;
    str << "ntmp" << setw(3) << setfill('0') << chain_no;
    chain_no++;
    return str.str();
#endif
}

string NATCompiler_ipt::debugPrintRule(Rule *r)
{
    NATRule *rule=NATRule::cast(r);

    return NATCompiler::debugPrintRule(rule)+
        " " + FWObjectDatabase::getStringId(rule->getInterfaceId()) +
        " " + rule->getStr("ipt_chain") +
        " " + rule->getStr("ipt_target") +
        " (type="+rule->getRuleTypeAsString()+")";
}


int NATCompiler_ipt::prolog()
{
    // initialize counters for the standard chains
    for (list<string>::const_iterator i =
             NATCompiler_ipt::getStandardChains().begin();
         i != NATCompiler_ipt::getStandardChains().end(); ++i)
    {
        chain_usage_counter[*i] = 1;
    }

    int n=NATCompiler::prolog();

    if ( n>0 ) 
    {
	bool found_ext=false;
	list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
	    Interface *iface=dynamic_cast<Interface*>(*i);
	    assert(iface);

	    if ( iface->isExt() ) found_ext=true;
            if ( iface->isDyn())  iface->setBool("use_var_address",true);
	}

	if (!found_ext)
	    throw FWException(_("At least one interface should be marked as external, can not configure NAT"));
    }

    return n;
}


void NATCompiler_ipt::_expandInterface(Interface *iface,
                                       std::list<FWObject*> &ol)
{
    std::list<FWObject*> nol;

    Compiler::_expandInterface(iface,ol);

    physAddress *pa=iface->getPhysicalAddress();
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

/*
 * Compiler::_expandInterface picks all IPv4 objects under Interface;
 * it can also put interface itself into the list ol.
 */
    for (std::list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); j++)
    {
        if (physAddress::cast(*j)!=NULL) continue;

        //const InetAddrMask *ipv4 = Address::cast(*j)->getAddressObjectInetAddrMask();
        const InetAddr *ip_addr = Address::cast(*j)->getAddressPtr();
        const InetAddr *ip_netm = Address::cast(*j)->getNetmaskPtr();


        if (ip_addr!=NULL && use_mac && pa!=NULL)
        {
            combinedAddress *ca = new combinedAddress(dbcopy,true);
            dbcopy->add(ca);
            dbcopy->addToIndex(ca);
            cacheObj(ca);
            ca->setName( "CA("+iface->getName()+")" );
            ca->setAddress( *ip_addr );
            ca->setNetmask( *ip_netm );
            ca->setPhysAddress( pa->getPhysAddress() );
            nol.push_back(ca);
        } else
            nol.push_back(*j); // if this is not IPv4, or we do not need to deal with MAC address
    }
    ol.clear();
    ol=nol;
}

bool compare_addresses_ptr(const InetAddr* a1, const InetAddr* a2)
{
    return (*a1 < *a2);
}


/*
 * call this processor after classifyNATRules
 */
bool NATCompiler_ipt::ConvertLoadBalancingRules::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::LB)
    {
        RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);

        list<const InetAddr*> al;
        for(list<FWObject*>::iterator i=tdst->begin(); i!=tdst->end(); i++) 
        {
            FWObject *o= *i;
            FWObject *obj = NULL;
            if (FWReference::cast(o)!=NULL)
                obj=FWReference::cast(o)->getPointer();
            //const InetAddrMask *a = Address::cast(obj)->getAddressObjectInetAddrMask();
            const InetAddr *ip_addr = Address::cast(obj)->getAddressPtr();
            al.push_back( ip_addr );
        }

        al.sort(compare_addresses_ptr);

        const InetAddr* a1 = al.front();
        list<const InetAddr*>::iterator j=al.begin();
        j++;

        for ( ; j!=al.end(); j++)
        {
/* I use temporary AddressRange object here because it takes care of
 * big endian/little endian conversion for me 
 */
            AddressRange tar;
            tar.setRangeStart( *a1 );
            tar.setRangeEnd( *(*j) );
            if ( tar.dimension() != 2 )
            {
                compiler->abort(
                    string( _("Non-contiguous address range in Translated Destination in load balancing NAT rule ") )+
                    rule->getLabel());
            }
            a1 = *j;
        }

        AddressRange *ar = AddressRange::cast(
            compiler->dbcopy->create(AddressRange::TYPENAME) );
        ar->setRangeStart( *(al.front()) );
        ar->setRangeEnd( *(al.back()) );
        ar->setName(string("%")+al.front()->toString()
                    +"-"+al.back()->toString()+"%" );
        compiler->cacheObj(ar); // to keep cache consistent
        compiler->dbcopy->add(ar,false);
        tdst->clearChildren();
        tdst->addRef(ar);

        rule->setRuleType(NATRule::DNAT);
    }

    return true;
}

/*
 * This processor should be called after classifyNATRule. Should call
 * classifyNATRule after this processor again.
 *
 * This algorithm is very much specific to iptables. Platforms where
 * this simple algorithm for SDNAT rules is not appropriate, should
 * either implement equivalent of this processor using different
 * algorithm, or should catch SDNAT rules and abort in their own
 * verifyNATRule processor.
 */
bool NATCompiler_ipt::splitSDNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if ( rule->getRuleType()==NATRule::SDNAT)
    {
//        RuleElementOSrc *osrc;
        RuleElementODst *odst;
        RuleElementOSrv *osrv;
        RuleElementTSrc *tsrc;
        RuleElementTDst *tdst;

/* first rule translates destination and may translate service (depends
 * on the original rule) */
        NATRule *r = NATRule::cast( compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
        tsrc=r->getTSrc();
        tsrc->clearChildren();
        tsrc->setAnyElement();

        tmp_queue.push_back(r);

/* the second rule translates source and uses translated object in
 * ODst. Since the service could have been translated by the first
 * rule, we use TSrv in OSrv */
        r = NATRule::cast( compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
/* if original rule involved negation in ODst, it should be processed
 * in the first of the two rules we create for SDNAT. Negation in OSrc
 * must be processed in both rules since the first rule does not
 * change OSrc
 */

        odst=r->getODst();
        odst->setNeg(false);

        odst->clearChildren();
        for (FWObject::iterator i=rule->getTDst()->begin(); i!=rule->getTDst()->end(); i++)
            odst->add( *i );

        if ( ! rule->getTSrv()->isAny())
        {
            osrv=r->getOSrv();
            osrv->clearChildren();
            for (FWObject::iterator i=rule->getTSrv()->begin(); i!=rule->getTSrv()->end(); i++)
                osrv->add( *i );
        }

        tdst=r->getTDst();
        tdst->clearChildren();
        tdst->setAnyElement();

        tmp_queue.push_back(r);
    }
    else 
        tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_ipt::VerifyRules::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

    if (tsrc->getNeg())
	throw FWException(_("Can not use negation in translated source. Rule ")+rule->getLabel());

    if (tdst->getNeg())
	throw FWException(_("Can not use negation in translated destination. Rule ")+rule->getLabel());

    if (tsrv->getNeg())
	throw FWException(_("Can not use negation in translated service. Rule ")+rule->getLabel());

    if (tsrv->size()!=1) 
	throw FWException(_("Translated service should be 'Original' or should contain single object. Rule: ")+rule->getLabel());

    if ( Group::cast( compiler->getFirstTSrv(rule) )!=NULL)
	throw FWException(_("Can not use group in translated service. Rule ")+rule->getLabel());


    if (rule->getRuleType()==NATRule::LB)
        throw FWException(_("Load balancing rules are not supported. Rule ")+rule->getLabel());


    if (rule->getRuleType()==NATRule::DNAT) 
    {
/*
 * check removed per bug report #780708  07/31/03  --vk
 *
 * multiple objects in ODST are allowed only in port translation rules
 * that do not change destination address. Such rules must have TDST==any
 *
        if ( odst->size()!=1 && ! tdst->isAny() )
            throw FWException(_("There should be no more than one object in original destination in the rule ")+rule->getLabel());
 */

/*
 * check removed per bug report #566172  06/07/02 --vk
 *
 *
        if ( odst->isAny() )
            throw FWException("Original destination can not be 'any' in DNAT rule. Rule "+rule->getLabel());

        Address* o2=odst->getFirst(true);
        if ( ! Host::isA(o2)  && ! Firewall::isA(o2) )
            throw FWException("Original destination must be host or firewall for this type of NAT rule. Rule "+rule->getLabel());
*/

/*
 *  check if we have specific IP address to use in odst 
 *
 *  disabled to fix a bug #562173 (bug created per user's request on
 *  Open Forum) 05/29/02 --vk
 *
        bool find_ext_static_address=false;
        FWObjectTypedChildIterator j=o2->findByType(Interface::TYPENAME);
        for ( ; j!=j.end(); ++j ) {
            Interface *iface=Interface::cast(*j);
            if ( ! iface->isLoopback() && iface->isExt() && ! iface->isDyn() ) 
                find_ext_static_address=true;
        }
        if ( ! find_ext_static_address && Host::isA(o2) ) 
            find_ext_static_address=true;

        if ( ! find_ext_static_address ) {
            if ( Firewall::isA(o2) )
                throw FWException("Could not find appropriate address for original destination: external interface of the firewall has dynamic address. Rule "+rule->getLabel());
            else
                throw FWException("Could not find appropriate address for original destination. Rule "+rule->getLabel());
        }
*/
    }

    if (rule->getRuleType()==NATRule::SNAT ) 
    {
//        if ( tsrc->size()!=1)
//            throw FWException(_("There should be no more than one object in translated source in the rule ")+rule->getLabel());

        Address* o1=compiler->getFirstTSrc(rule);
        if ( ! tsrc->isAny() && Network::cast(o1)!=NULL)
            throw FWException(_("Can not use network object in translated source. Rule ")+rule->getLabel());
    }


    if (rule->getRuleType()==NATRule::SNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstOSrc(rule));
        Network *a2=Network::cast(compiler->getFirstTSrc(rule));
        if ( a1==NULL || a2==NULL ||
             a1->getNetmaskPtr()->getLength() != a2->getNetmaskPtr()->getLength() )
            throw FWException(_("Original and translated source should both be networks of the same size . Rule ")+rule->getLabel());
    }

    if (rule->getRuleType()==NATRule::DNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstODst(rule));
        Network *a2=Network::cast(compiler->getFirstTDst(rule));
        if ( a1==NULL || a2==NULL ||
             a1->getNetmaskPtr()->getLength() != a2->getNetmaskPtr()->getLength() )
            throw FWException(_("Original and translated destination should both be networks of the same size . Rule ")+rule->getLabel());
    }

    return true;
}

/*
 * this should be called only after splitServices, so that we have
 * objects of the same type in OSrv and either "any" or a single
 * object in TSrv
 */
bool NATCompiler_ipt::VerifyRules2::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()!= NATRule::Return)
    {
        RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
        RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

        Service *s1=compiler->getFirstOSrv(rule);
        Service *s2=compiler->getFirstTSrv(rule);

        if (osrv->isAny() && ! tsrv->isAny())
            throw FWException(_("Can not use service object in Translated Service if Original Service is 'Any'. Rule ")+rule->getLabel());
        
        if (!tsrv->isAny() && s1->getProtocolNumber()!=s2->getProtocolNumber())
            throw FWException(_("Translated Service should be either 'Original' or should contain object of the same type as Original Service. Rule ")+rule->getLabel());
    }
    return true;
}

bool NATCompiler_ipt::convertToAtomicportForOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getOSrv()->size()>1 && ! rule->getTSrv()->isAny())
    {
        RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

        for (FWObject::iterator i1=osrv->begin(); i1!=osrv->end(); ++i1) 
        {
            NATRule *r = NATRule::cast(
                compiler->dbcopy->create(NATRule::TYPENAME) );
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);

            FWObject *s;
        
            s=r->getOSrv();	assert(s);
            s->clearChildren();
            s->add( *i1 );
            
            tmp_queue.push_back(r);
        }
    }
    else
        tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::portTranslationRules::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    Address  *odst=compiler->getFirstODst(rule);
//    Service  *osrv=compiler->getFirstOSrv(rule);

    Address  *tsrc=compiler->getFirstTSrc(rule);
    Address  *tdst=compiler->getFirstTDst(rule);
    Service  *tsrv=compiler->getFirstTSrv(rule);

    if (rule->getRuleType() == NATRule::DNAT &&
        tsrc->isAny() &&   tdst->isAny() && ! tsrv->isAny() && odst->getId()==compiler->fw->getId() )
    {
        rule->getTDst()->addRef( odst );
    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::specialCaseWithRedirect::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    Address  *tdst=compiler->getFirstTDst(rule);

/* we consider rule redirect only if TDst is a firewall object */
    if (rule->getRuleType() == NATRule::DNAT && tdst->getId()==compiler->fw->getId()) 
        rule->setRuleType(NATRule::Redirect);

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitOnODst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementODst  *odst=rule->getODst();  assert(odst);
    if (rule->getRuleType()==NATRule::DNAT && odst->size()!=1)
    {
	for(list<FWObject*>::iterator i=odst->begin(); i!=odst->end(); ++i) 
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    Address *a=Address::cast( o );
	    assert(a);

	    NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    RuleElementODst *nodst=r->getODst();
	    nodst->clearChildren();

	    nodst->addRef( a );

	    tmp_queue.push_back( r );
        }
    } else
	tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_ipt::splitOnOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
    if (osrv->size()!=1) 
    {
	for(list<FWObject*>::iterator i=osrv->begin(); i!=osrv->end(); ++i) 
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    Service *s=Service::cast( o );
	    assert(s);

	    NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    RuleElementOSrv *nosrv=r->getOSrv();
	    nosrv->clearChildren();

	    nosrv->addRef( s );

	    tmp_queue.push_back( r );
	}
    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::fillTranslatedSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    Service  *osrv_o=compiler->getFirstOSrv(rule);
    Service  *tsrv_o=compiler->getFirstTSrv(rule);

    if ( ! osrv_o->isAny()  && tsrv_o->isAny() ) {
	RuleElementTSrv  *tsrv=rule->getTSrv();
	tsrv->addRef(osrv_o);
    }
    return true;
}

bool NATCompiler_ipt::addVirtualAddress::processNext()
{
    FWOptions* options=compiler->fw->getOptionsObject();
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    Address *a=NULL;

    if (rule->getRuleType()==NATRule::SNAT || rule->getRuleType()==NATRule::DNAT) 
    {
        if (rule->getRuleType()==NATRule::SNAT)   
            a=compiler->getFirstTSrc(rule);
        else
            a=compiler->getFirstODst(rule);

        if ( ! a->isAny() && ! compiler->complexMatch(a,compiler->fw) && 
             options->getBool("manage_virtual_addr") )
        {
            if (AddressRange::cast(a)!=NULL)
            {
                compiler->warning(string(_("Adding of virtual address for address range is not implemented (object ")) +
                                  a->getName() + ")" );
            } else
                compiler->osconfigurator->addVirtualAddressForNAT( a );
        }
        return true;
    }


    if (rule->getRuleType()==NATRule::SNetnat || rule->getRuleType()==NATRule::DNetnat)
    {
        if (rule->getRuleType()==NATRule::SNetnat)   
            a=compiler->getFirstTSrc(rule);
        else
            a=compiler->getFirstODst(rule);

        if ( ! a->isAny() && Network::cast(a) )
            compiler->osconfigurator->addVirtualAddressForNAT( Network::constcast(a) );

        return true;
    }

    return true;
}

bool NATCompiler_ipt::splitRuleIfRuleElementIsDynamicInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElement    *re=RuleElement::cast(rule->getFirstByType(re_type));
    int nre=re->size();

    vector<FWObject*> cl;

    for(list<FWObject*>::iterator i=re->begin(); nre>1 && i!=re->end(); ++i) 
    {
        FWObject *o= *i;
	FWObject *obj = NULL;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=NULL && !iface->isRegular())
        {
	    cl.push_back(o);   // can not remove right now because remove invalidates iterator
            nre--;

	    NATRule  *new_rule= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	    compiler->temp_ruleset->add(new_rule);
	    new_rule->duplicate(rule);
            RuleElement *new_re=RuleElement::cast(new_rule->getFirstByType(re_type));
	    new_re->clearChildren();
	    new_re->setAnyElement();
	    new_re->addRef( iface );
	    tmp_queue.push_back(new_rule);
        }
    }
    if (!cl.empty()) {
        for (vector<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->remove( (*i1) );
    }

    tmp_queue.push_back(rule);

    return true;
}



bool NATCompiler_ipt::specialCaseWithUnnumberedInterface::dropUnnumberedInterface(RuleElement *re)
{
    if (re->isAny()) return true;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL &&
            (ifs->isUnnumbered() || ifs->isBridgePort())
           ) cl.push_back(obj);
    }
    if (!cl.empty()) 
    {
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->removeRef( (*i1) );
    }
    return (!re->isAny());
}

bool NATCompiler_ipt::specialCaseWithUnnumberedInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    bool keep_rule=true;
    switch (rule->getRuleType()) {
    case NATRule::Masq:
    case NATRule::SNAT:
        keep_rule=dropUnnumberedInterface( rule->getOSrc() );
        break;
    case NATRule::DNAT:
        keep_rule=dropUnnumberedInterface( rule->getODst() );
        break;
    default: ;
    }
    if (keep_rule) tmp_queue.push_back(rule);
    return true;
}


/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_ipt::ReplaceFirewallObjectsODst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementODst *rel;
    Address     *obj=NULL;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
//    case NATRule::Redirect: 
	return true;
    default:
	rel=rule->getODst();               assert(rel);
	obj=compiler->getFirstODst(rule);  assert(obj!=NULL);

	if (obj->getId()==compiler->getFwId() ) 
	{

	    list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
	    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) {
		Interface *interface_=Interface::cast(*i);
/*
 * Right now I build DNAT and Redirect rules only for external interface.
 * This actually implies some sort of assymmetric firewall; it does
 * not have to be like this.
 * 
 * TODO: add platform-specific fw option "Generate NAT rules for all
 * interfaces" and use here all interfaces , not only external ones.
 * Do this for v1.0.4.  Modify verification code in verifyRules
 * accordingly. The same applies to ReplaceFirewallObjectsTSrc
 *
 * Note 05/29/02: I do not check if external interface is dynamic anymore
 *                see bug #562173    --vk
 *
 *
 *		if (! interface_->isLoopback() && 
 *                    interface_->isExt() &&
 *                  ! interface_->isDyn() ) cl.push_back(interface_);
 *
 * update 03/20/03:
 *
 * generally we assume that if firewall object is used in the rule,
 * then any or all its interface will be used. This means that if
 * firewall is in ODst we should really use all of its interfaces, not
 * only external ones.
 */
		if (! interface_->isLoopback() ) cl.push_back(interface_);

//		if (! interface_->isLoopback() && 
//                      interface_->isExt() ) cl.push_back(interface_);
	    }
	    if ( ! cl.empty() ) {
		while (rel->size()) 
		    rel->remove( rel->front()  );
		
		for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
		{
		    rel->addRef( *i1 );
		}
	    }
	}
    }
    return true;
}

/*
 * This processor works together with ConvertToAtomicRules and
 * AssignInterfaces. If firewall object is used in TSrc of SNAT rule,
 * it gets replaced with its interfaces. ConvertToAtomicRules slits
 * this rule onto atomic rules, each of which has one interface object
 * in TSrc. AssigInterfaces then assigns each atomic rule to
 * corresponding interface.
 *
 * it seems the simplest way is just to assign SNAT rule to all
 * interfaces if firewall is used in TSrc.  This automatically takes
 * care of weird cases where people use address that belongs to subnet
 * of one interface to do translation of packets going out through
 * another interface. Basically, compiler does not have information
 * about routing, so we have no choice but assume the routing can be
 * anything and assign the rule to all interfaces.
 */
bool NATCompiler_ipt::ReplaceFirewallObjectsTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementTSrc *rel;
    Address     *obj=NULL;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
    case NATRule::Redirect: return true;

    default:
	rel=rule->getTSrc();               assert(rel);
	obj=compiler->getFirstTSrc(rule);  assert(obj!=NULL);

	if (obj->getId()==compiler->getFwId() ) 
	{
            Address *odst=compiler->getFirstODst(rule);

            rel->clearChildren();

            Interface *iface=compiler->findInterfaceFor(odst,compiler->fw);

            if (!odst->isAny() && iface!=NULL)  rel->addRef(iface);
            else  // else use all interfaces except loopback and unnumbered ones
            {
                list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
                for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
                {
                    Interface *iface=Interface::cast(*i);
                    if (! iface->isLoopback() &&
                        ! iface->isUnnumbered() &&
                        ! iface->isBridgePort()
                    )
                        rel->addRef( *i );
                }
                for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1)
                    rel->addRef( *i1 );

/* it is an error if rule element is empty at this point. this could have
 * happened if all external interfaces are unnumbered */
                if (rel->size()==0)
                {
                    char errmsg[1024];
                    sprintf(errmsg,
_("Could not find suitable interface for the NAT rule %s. Perhaps all interfaces are unnumbered?"), 
                            rule->getLabel().c_str() );
                            compiler->abort(errmsg);
                }
            }
	}
    }
    return true;
}

bool NATCompiler_ipt::dynamicInterfaceInODst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementODst  *odstrel=rule->getODst();  assert(odstrel);
    Address          *odst   =compiler->getFirstODst(rule);
    if ( ! odstrel->isAny() )
    {
        Interface        *iface  =Interface::cast(odst);
        if (iface!=NULL && iface->isDyn())
        {
            ;
//            iface->setBool("use_var_address",true);
//            odstrel->clearChildren();
//            odstrel->setAnyElement();
        }
    }
    return true;
}

bool NATCompiler_ipt::splitMultiSrcAndDst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *osrv=rule->getOSrv();
    RuleElementOSrc *osrc=rule->getOSrc();
    RuleElementODst *odst=rule->getODst();
    RuleElementOSrc *rosrc;
    RuleElementODst *rodst;

    int nosrv=osrv->size();
    int nosrc=osrc->size();
    int nodst=odst->size();

/*
 * Return if service is set - svcs my introduce complications and I'm
 * treading carefully here.
 * We don't handle anything thats redirect, MASQ yet - just NONAT,SNAT & DNAT 
 * We also check we've got multiple rules to deal with - we can't optimize
 * 1 src with 1 dst ...
 */
 
    if ((nosrv>1  || !(osrv->isAny())) || 
        (nosrc<1 || osrc->isAny())  ||
        (nodst<1 || odst->isAny())  ||
        (nosrc==1 && nodst==1) )
    {
        tmp_queue.push_back(rule);
        return true;
    }
      
    switch (rule->getRuleType()) {
    case NATRule::NONAT:     
    case NATRule::SNAT:     
    case NATRule::DNAT:
    { 
// get old chain name create new chain name
        string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
// create new rule
        NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
        compiler->temp_ruleset->add(r);
        r->duplicate(rule); 
// move existing rule onto new chain
        rule->setStr("ipt_chain",new_chain);
// we've already tested for interface ....
        rule->setInterfaceStr("nil");
// new rule points to new chain, continues if no match
        r->setStr("ipt_target",new_chain);

// Now decide which way round would be best ...
        if (nosrc < nodst) 
        {
            rodst=r->getODst(); rodst->clearChildren(); rodst->setAnyElement();
            osrc->clearChildren();   osrc->setAnyElement();
        } else {
            rosrc=r->getOSrc(); rosrc->clearChildren(); rosrc->setAnyElement();
            odst->clearChildren();   odst->setAnyElement();
        }
               
        tmp_queue.push_back(r); 
        
    }
    break;
    default: ;
    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::dynamicInterfaceInTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    Address  *tsrc=compiler->getFirstTSrc(rule);

    if (rule->getRuleType()==NATRule::SNAT && 
        Interface::cast(tsrc)!=NULL && !Interface::cast(tsrc)->isRegular())
    {

            rule->setRuleType(NATRule::Masq);

            if ( rule->getStr("ipt_target")=="" || rule->getStr("ipt_target")=="SNAT" )
                rule->setStr("ipt_target","MASQUERADE");
    }
    return true;
}

/**
 * unlike standard inspector addressRanges in the base class NATCompiler,
 * this one does not expand address ranges in TSrc and TDst because
 * iptables supports ranges in those rule elements
 */
bool NATCompiler_ipt::ExpandAddressRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    rel=rule->getOSrc();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getODst();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    return true;
}



void NATCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(RuleElement *re,
                                                                                     Rule        *rule)
{
    if (re->isAny()) return;

    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL && ifs->isDyn() && ! ifs->isChildOf(compiler->fw))        
        {
#if 0
            cerr << "NATCompiler_ipt::checkForDynamicInterfacesOfOtherObjects" << endl;
            cerr << "ifs: " << endl;
            ifs->dump(true,true);
            cerr << endl;
            cerr << "fw: " << endl;
            compiler->fw->dump(true,true);
            cerr << endl;
#endif
            char errstr[2048];
            sprintf(errstr,_("Can not build rule using dynamic interface '%s' of the object '%s' because its address in unknown. Rule %s"),
                    ifs->getName().c_str(), 
                    ifs->getParent()->getName().c_str(),
                    rule->getLabel().c_str() );

            throw FWException(errstr);
        }
    }
}


bool NATCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    findDynamicInterfaces( rule->getOSrc() , rule );
    findDynamicInterfaces( rule->getODst() , rule );
    findDynamicInterfaces( rule->getTSrc() , rule );
    findDynamicInterfaces( rule->getTDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}




bool NATCompiler_ipt::splitServices::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *srv=rule->getOSrv();

    if (srv->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    map<int, list<Service*> > services;

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

	Service *s=Service::cast( o );
	assert(s);

	int proto=s->getProtocolNumber();
	services[proto].push_back(s);
    }

    for (map<int, list<Service*> >::iterator i=services.begin(); i!=services.end(); i++) {
	list<Service*> &sl=(*i).second;

	NATRule *r= NATRule::cast(
	    compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementOSrv *nsrv=r->getOSrv();
	nsrv->clearChildren();

	for (list<Service*>::iterator j=sl.begin(); j!=sl.end(); j++) {
	    nsrv->addRef( (*j) );
	}

	tmp_queue.push_back(r);

    }
    return true;
}

bool NATCompiler_ipt::separatePortRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *rel= rule->getOSrv();

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) {
	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if ( TCPService::isA(s) || UDPService::isA(s) ) {
            int srs=TCPUDPService::cast(s)->getSrcRangeStart();
            int sre=TCPUDPService::cast(s)->getSrcRangeEnd();
            int drs=TCPUDPService::cast(s)->getDstRangeStart();
            int dre=TCPUDPService::cast(s)->getDstRangeEnd();

            compiler->normalizePortRange(srs,sre);
            compiler->normalizePortRange(drs,dre);

            if (srs!=sre || drs!=dre) {
                NATRule *r= NATRule::cast(
                    compiler->dbcopy->create(NATRule::TYPENAME) );
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementOSrv *nsrv=r->getOSrv();
                nsrv->clearChildren();
                nsrv->addRef( s );
                tmp_queue.push_back(r);
                services.push_back(s);
            } 
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::separateSourcePorts::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *rel= rule->getOSrv();

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    NATRule         *rule_4_src_ports=NULL;
    RuleElementOSrv *nsrv = NULL;

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if ( TCPService::isA(s) || UDPService::isA(s) ) {
            int srs=TCPUDPService::cast(s)->getSrcRangeStart();
            int sre=TCPUDPService::cast(s)->getSrcRangeEnd();

            compiler->normalizePortRange(srs,sre);

            if (srs!=0 || sre!=0)
            {
                if (rule_4_src_ports==NULL)
                {
                    rule_4_src_ports= NATRule::cast(
                        compiler->dbcopy->create(NATRule::TYPENAME) );
                    compiler->temp_ruleset->add(rule_4_src_ports);
                    rule_4_src_ports->duplicate(rule);
                    nsrv=rule_4_src_ports->getOSrv();
                    nsrv->clearChildren();
                    tmp_queue.push_back(rule_4_src_ports);
                }
                assert(nsrv!=NULL);
                nsrv->addRef( s );
                services.push_back(s);
            }
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::separateSourceAndDestinationPorts::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *rel= rule->getOSrv();

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    NATRule         *nrule=NULL;
    RuleElementOSrv *nsrv = NULL;

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if ( TCPService::isA(s) || UDPService::isA(s) ) {
            int srs=TCPUDPService::cast(s)->getSrcRangeStart();
            int sre=TCPUDPService::cast(s)->getSrcRangeEnd();
            int drs=TCPUDPService::cast(s)->getDstRangeStart();
            int dre=TCPUDPService::cast(s)->getDstRangeEnd();

            compiler->normalizePortRange(srs,sre);
            compiler->normalizePortRange(drs,dre);

            if ( (srs!=0 || sre!=0) && (drs!=0 || dre!=0) )
            {
                if (nrule==NULL)
                {
                    nrule= NATRule::cast(
                        compiler->dbcopy->create(NATRule::TYPENAME) );
                    compiler->temp_ruleset->add(nrule);
                    nrule->duplicate(rule);
                    nsrv=nrule->getOSrv();
                    nsrv->clearChildren();
                    tmp_queue.push_back(nrule);
                }
                assert(nsrv!=NULL);
                nsrv->addRef( s );
                services.push_back(s);
            } 
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_ipt::prepareForMultiport::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *rel= rule->getOSrv();
    Service         *srv= compiler->getFirstOSrv(rule);

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

/*
 * processor splitServices should have been called eariler, so now all
 * services in Srv are of the same type
 */
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	rule->setBool("ipt_multiport",true);
/* make sure we have no more than 15 ports */
	if (rel->size()>15) 
        {
	    int n=0;
	    NATRule         *r;
	    RuleElementOSrv *nsrv=NULL;
	    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
            {
		FWObject *o= *i;
		if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    
		Service *s=Service::cast( o );
		assert(s);

		if (n==0) 
                {
		    r= NATRule::cast(
			compiler->dbcopy->create(NATRule::TYPENAME) );
		    compiler->temp_ruleset->add(r);
		    r->duplicate(rule);
		    nsrv=r->getOSrv();
		    nsrv->clearChildren();
		    tmp_queue.push_back(r);
		}
                assert(nsrv!=NULL);
		nsrv->addRef( s );
		if (++n>=15) n=0;
	    }
	} else {
	    tmp_queue.push_back(rule);
	}

//	tmp_queue.push_back(rule);
    } else
	tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::splitMultipleICMP::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *rel= rule->getOSrv();
    Service         *srv= compiler->getFirstOSrv(rule);

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    if (ICMPService::isA(srv))
    {
        NATRule         *r;
        RuleElementOSrv *nsrv;
        for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    
            Service *s=Service::cast( o );
            assert(s);

            r= NATRule::cast( compiler->dbcopy->create(NATRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nsrv=r->getOSrv();
            nsrv->clearChildren();
            nsrv->addRef( s );
            tmp_queue.push_back(r);
        }
    } else
	tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::doOSrcNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrc *osrcrel=rule->getOSrc();

/*   ! A  B  C    */

    if (osrcrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;
	RuleElementTSrv *ntsrv;

	string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
	osrcrel->setNeg(false);
/*
 *   negation in OSrc :
 *
 *  CHAIN     !A    B    C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----      any   B    C    SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN   A   any  any   RETURN     RETURN
 * TMP_CHAIN  any  any   C   SNAT/DNAT  ---------
 */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
//	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   A  any  any  RETURN  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	ndst=r->getODst();   ndst->clearChildren();   ndst->setAnyElement();
	nsrv=r->getOSrv();   nsrv->clearChildren();   nsrv->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	ntsrv=r->getTSrv();  ntsrv->clearChildren();  ntsrv->setAnyElement();
	ndst->setNeg(false);
	nsrv->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any any C  ACTION  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();
	ndst->setNeg(false);
	nsrv->setNeg(false);
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::doODstNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementODst *odstrel=rule->getODst();

/*   ! A  B  C    */

    if (odstrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;
	RuleElementTSrv *ntsrv;

	string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
	odstrel->setNeg(false);
/*
 *   negation in Odst :
 *
 *  CHAIN      A    !B   C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----       A   any   C    SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN  any   B   any   RETURN     RETURN
 * TMP_CHAIN  any  any   C    SNAT/DNAT  ---------
 */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	ndst=r->getODst();   ndst->clearChildren();   ndst->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  B  any  RETURN  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();   nsrc->clearChildren();   nsrc->setAnyElement();
	nsrv=r->getOSrv();   nsrv->clearChildren();   nsrv->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	ntsrv=r->getTSrv();  ntsrv->clearChildren();  ntsrv->setAnyElement();
	nsrc->setNeg(false);
	nsrv->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
//	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  C  ACTION  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();
	nsrc->setNeg(false);
	nsrv->setNeg(false);
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::doOSrvNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv *osrvrel=rule->getOSrv();

/*   A  B ! C    */

    if (osrvrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;

	string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
	osrvrel->setNeg(false);
/*
 *   negation in OSrv :
 *
 *  CHAIN      A    B   !C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----       A    B   any   SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN  any  any   C    RETURN     RETURN
 * TMP_CHAIN  any  any  any   SNAT/DNAT  ---------
 */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getOSrv();  nsrv->clearChildren();  nsrv->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  C  RETURN  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	nsrc->setNeg(false);
	ndst->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  any  ACTION  */
	r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();  nsrv->clearChildren();  nsrv->setAnyElement();
	nsrc->setNeg(false);
	ndst->setNeg(false);
	r->setStr("ipt_chain",new_chain);
	r->setInterfaceStr("nil");
//	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitNONATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if ( rule->getStr("ipt_chain").empty() && rule->getRuleType()==NATRule::NONAT) {

        Address *osrc=compiler->getFirstOSrc(rule);
        bool osrcfw= compiler->complexMatch(osrc,compiler->fw);
/*
 * NONAT is special if OSrc matches firewall. It is not sufficient to
 * only put this rule in the OUTPUT chain because packets originating
 * on the firewall actually cross both OUTPUT and POSTROUTING chains
 * (I tested this). So, we need to make sure we _do not_ translate in
 * both these chains because there may be other rules in POSTROUTING
 * chain that may accidentally match the packet and translate it.
 */
        NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        r->setStr("ipt_chain","POSTROUTING");  
        tmp_queue.push_back(r);

        if (osrcfw)
        {
            rule->setStr("ipt_chain","OUTPUT");
            if (osrc->getId()==compiler->fw->getId())
            {
                RuleElementOSrc *src;
                src=rule->getOSrc();
                src->clearChildren();
                src->setAnyElement();
            }
        } else          rule->setStr("ipt_chain","PREROUTING");  

        tmp_queue.push_back(rule);

    } else
        tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::localNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

//    if ( rule->getStr("ipt_chain").empty())
//    {

    Address          *osrc=compiler->getFirstOSrc(rule);
    bool osrcfw= compiler->complexMatch(osrc,compiler->fw);

    switch( rule->getRuleType())
    {
    case NATRule::DNAT:
    case NATRule::DNetnat:

/* it should not be necessary to do anything if rule type is NONAT
 * since splitNONATRule takes care of NONAT rules
 *
 * is there any need to split the rule if it is SNAT or DNAT type ?  I
 * can't see any reason to do it.
 *
 * Can use OUTPUT chain only for DNAT rules and a like
 */
        if (osrcfw) rule->setStr("ipt_chain","OUTPUT");
        if (osrcfw && osrc->getId()==compiler->fw->getId())
        {
            RuleElementOSrc *src;
            src=rule->getOSrc();
            src->clearChildren();
            src->setAnyElement();
        }
        break;
    default:
        break;
    }
//    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitIfOSrcAny::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

/* do not split rules added to handle negation, these rules have "any"
 * in OSrc but get control only after OSrc is tested by another
 * rule */
    if (rule->getBool("rule_added_for_osrc_neg")) return true;
    if (rule->getBool("rule_added_for_odst_neg")) return true;
    if (rule->getBool("rule_added_for_osrv_neg")) return true;

    if (rule->getRuleType()==NATRule::DNAT)
    {
//        RuleElementOSrc *osrcrel=rule->getOSrc();
        Address         *osrc=compiler->getFirstOSrc(rule); 

        if (osrc->isAny())
        {
            NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementOSrc *nosrcrel=r->getOSrc();
            nosrcrel->addRef(compiler->fw);
            tmp_queue.push_back(r);
        }
    }

    return true;
}


/*
 * we assume that splitIfOSrcMatchesFw was called before, so that if firewall
 * was in OSrc, it is now a single object in that rule element
 */
bool NATCompiler_ipt::DNATforFW::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::DNAT)
    {
        Address *osrc=compiler->getFirstOSrc(rule);
        if ( compiler->complexMatch(osrc,compiler->fw) ) 
        {
            rule->setStr("ipt_chain","OUTPUT");  
            if (osrc->getId()==compiler->fw->getId())  
            {
                rule->getOSrc()->clearChildren();
                rule->getOSrc()->setAnyElement();
            }
        }
    }

    return true;
}


bool NATCompiler_ipt::decideOnChain::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_chain").empty() ) return true; // already defined

    switch (rule->getRuleType()) {
    case NATRule::SNAT:     rule->setStr("ipt_chain","POSTROUTING"); break;
    case NATRule::SNetnat:  rule->setStr("ipt_chain","POSTROUTING"); break;
    case NATRule::Masq:     rule->setStr("ipt_chain","POSTROUTING"); break;
    case NATRule::DNAT:     rule->setStr("ipt_chain","PREROUTING");  break;
    case NATRule::DNetnat:  rule->setStr("ipt_chain","PREROUTING");  break;
    case NATRule::Redirect: rule->setStr("ipt_chain","PREROUTING");  break;
    case NATRule::NONAT:    
// processor splitNONATRule took care of NONAT rule
        break;
    default: ;
    }
    return true;
}


bool NATCompiler_ipt::decideOnTarget::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_target").empty() ) return true; // already defined

    switch (rule->getRuleType()) {
    case NATRule::NONAT:    rule->setStr("ipt_target","ACCEPT");   break;
    case NATRule::SNAT:     rule->setStr("ipt_target","SNAT");     break;
    case NATRule::SNetnat:  rule->setStr("ipt_target","NETMAP");   break;
    case NATRule::DNAT:     rule->setStr("ipt_target","DNAT");     break;
    case NATRule::DNetnat:  rule->setStr("ipt_target","NETMAP");   break;
    case NATRule::Masq:     rule->setStr("ipt_target","MASQUERADE"); break;
    case NATRule::Redirect: rule->setStr("ipt_target","REDIRECT"); break;
    case NATRule::Return:   rule->setStr("ipt_target","RETURN");   break;
    default: ;
    }
    return true;
}

/*
 * this processor works together with ReplaceFirewallObjectsTSrc and
 * ConvertToAtomicRules. If the first two left interface object in
 * TSrc, AssignInterfaces assigns this rule to the corresponding
 * interface. Rule will be split and assigned to all interfaces here
 * if object in TSrc is not an interface or an address of interface.
 *
 * Summary: SNAT rules are now assigned to interfaces (using "-o
 * iface_name") as follows:
 *
 * - if firewall's interface or its address is in TSrc, the rule the
 * uses its address for "--to-source" and its name for "-o"
 *
 * - if firewall object is in TSrc, then it gets replaced with its
 * interfaces (except unnumbered and loopback interfaces) and rule is
 * processed using each interface as described above
 *
 * - if some other object is in TSrc, the rule is assigned to all
 * interfaces of the firewall (using notation with '+') and address of
 * this object is used for "--to-source". There are reasons why rule
 * has to be explicitly assigned to all interfaces using "-o" as
 * opposed to skipping "-o" all together. consider for example a
 * configuration with an unnumbred tunnel interface (e.g. ipsec0) used
 * for "road varrior" connections where IP address on the other end of
 * the tunnel is unknown. We can not add a "no nat" rule because we do
 * not know address of the net on the other side of the tunnel, but
 * fortunately ipsec0 is skipped in the assignment of SNAT rule
 * because it is unnumbered, so the firewall won't translate packets
 * going through this interface.
 * 
 */
bool NATCompiler_ipt::AssignInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

//    Address  *a=NULL;
//    FWObject *ref;

    if (regular_interfaces.size()==0)
    {
        list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
            Interface *iface=Interface::cast(*i);
            assert(iface);

            if (iface->isLoopback() ||
                iface->isUnnumbered() ||
                iface->isBridgePort()
            ) continue;

            char *in=strdup( iface->getName().c_str() );
            char *cptr=in;
            while (*cptr && *cptr!='*' && !isdigit(*cptr)) ++cptr;
/* if interface name ends with '*', this is wildcard interface. Just
 * replace '*' with '+'. If interace name does not end with '*',
 * replace numeric interface index with '+'. Either way, cptr points
 * at the first caracter after the 'family' name of the interface (is
 * there a better term?) which will be either a digit or '*'.
 */
            *cptr='\0';
            string inexp=string(in)+"+";
            if ( std::find(regular_interfaces.begin(),
                           regular_interfaces.end(),
                           inexp)==regular_interfaces.end() )
                regular_interfaces.push_back( inexp );

            free(in);
        }
    }

    switch (rule->getRuleType()) {
    case NATRule::SNAT:
    case NATRule::Masq:
    {
        Address* a=compiler->getFirstTSrc(rule);

        if ( (Interface::isA(a) || IPv4::isA(a)) && a->isChildOf(compiler->fw))
        {
            FWObject *p=a;
            while ( ! Interface::isA(p) ) p=p->getParent();
            rule->setInterfaceId( p->getId() );
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then TSrc is not an interface or address of an
 * interface. This processor will simply pass a rule along if firewall
 * has no interfaces at all.  I wonder if I really have to do this,
 * but I do it anyway.
 */
        int n=0;
        for (list<string>::iterator i=regular_interfaces.begin(); i!=regular_interfaces.end(); i++)
        {
            NATRule *r = NATRule::cast(
                compiler->dbcopy->create(NATRule::TYPENAME) );
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);

            r->setInterfaceStr( *i );
            
            tmp_queue.push_back(r);
            n++;
        }
        if (n==0) tmp_queue.push_back(rule);
        return true;
    }
    break;
    default: ;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::verifyRuleWithMAC::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    string chain=rule->getStr("ipt_chain");

    if (chain!="PREROUTING" &&
        chain!="FORWARD" &&
        chain!="INPUT" )
    {
/* scan all objects in OSrc, look for physAddress or combinedAddress
 * with pa present.  Objects like that are not allowed in chain POSTROUTING.
 * Issue warning and remove physAddress from the list.
 */
        RuleElementOSrc *rel= rule->getOSrc();
        if (rel->isAny()) return true;

        list<FWObject*> cl;
        FWObject       *pa=NULL;
        for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
            FWObject *o= *i;
            FWObject *o1= o;
            if (FWReference::cast(o)!=NULL) o1=FWReference::cast(o)->getPointer();

            if (physAddress::isA(o1))
            {
                pa=o1;
                cl.push_back(o1);
            }
            combinedAddress *ca=combinedAddress::cast(o1);
            if (ca!=NULL &&  ca->getPhysAddress()!="" )
            {                
/*  there are two possibilities:
 * 1 - combinedAddress consists of the IPv4 component and MAC address component
 * 2 - combinedAddress consists of an empty IPv4 component and MAC address .
 */
                pa=o1;
                if ( ca->isAny() ) cl.push_back(o1);
                else ca->setPhysAddress("");
            }
        }

        if (!cl.empty()) 
        {
            for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
                rel->removeRef( (*i1) );
        }

        if (pa!=NULL)
        {
            char errmsg[2048];
            if (rel->isAny())
            {
                sprintf(errmsg,
_("SNAT rule can not match MAC address, however after removing object %s from OSrc it becomes 'Any' in the rule %s"),
                        pa->getName().c_str(),
                        rule->getLabel().c_str() );
                compiler->abort( errmsg );
            }
            else
            {
                sprintf(errmsg,
_("SNAT rule can not match MAC address. Object %s removed from the rule %s"),
                        pa->getName().c_str(),
                        rule->getLabel().c_str() );
                compiler->warning( errmsg );
            }
        }
    }

    return true;
}

bool NATCompiler_ipt::processMultiAddressObjectsInRE::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    OSConfigurator_linux24 *osconf = 
        dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator);

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    if (re->size()==1) 
    {
        FWObject *o = re->front();
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=NULL)
        {
            // we have just one object in RE and this object is MutiAddressRunTime
            if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                rule->setStr("address_table_file",atrt->getSourceName());
                osconf->registerMultiAddressObject(atrt);
            }
            if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
            {
                // this is DNSName converted to its run-time counterpart,
                // we do not need to touch it at all
            }
            tmp_queue.push_back(rule);
            return true;
        }
    }

    list<MultiAddressRunTime*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=NULL && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            cl.push_back(atrt);
    }

    if (!cl.empty())
    {
        RuleElement *nre;
        RuleElement *ore=RuleElement::cast( rule->getFirstByType(re_type) );
        NATRule *r;
        for (list<MultiAddressRunTime*>::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            MultiAddressRunTime *atrt = *i;
            r = NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nre=RuleElement::cast( r->getFirstByType(re_type) );
            nre->clearChildren();
            nre->addRef( atrt );
            r->setStr("address_table_file",atrt->getSourceName());
            osconf->registerMultiAddressObject(atrt);
            tmp_queue.push_back(r);

            ore->removeRef( *i );
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::countChainUsage::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *rule = NATRule::cast( *k );
        ipt_comp->chain_usage_counter[rule->getStr("ipt_target")] += 1;
    }

    return true;
}



void NATCompiler_ipt::compile()
{
//    FWOptions* options=fw->getOptionsObject();

    cout << " Compiling rules for 'nat' table";
    if (ipv6) cout << ", IPv6";
    cout <<  endl << flush;

    try {

	Compiler::compile();
#if 0
        cerr << "Checking objects" << endl;
        dbcopy->findInIndex("id45035B9F499")->dump(true,true);
        dbcopy->findInIndex("id4504CDCF27596")->dump(true,true);
        cerr << endl;
#endif
        add( new NATCompiler::Begin());

        add( new printTotalNumberOfRules());

        add( new recursiveGroupsInOSrc("check for recursive groups in OSRC"));
        add( new recursiveGroupsInODst("check for recursive groups in ODST"));
        add( new recursiveGroupsInOSrv("check for recursive groups in OSRV"));

        add( new recursiveGroupsInTSrc("check for recursive groups in TSRC"));
        add( new recursiveGroupsInTDst("check for recursive groups in TDST"));
        add( new recursiveGroupsInTSrv("check for recursive groups in TSRV"));

        add( new emptyGroupsInOSrc("check for empty groups in OSRC"      ));
        add( new emptyGroupsInODst("check for empty groups in ODST"      ));
        add( new emptyGroupsInOSrv("check for empty groups in OSRV"      ));

        add( new emptyGroupsInTSrc("check for empty groups in TSRC"      ));
        add( new emptyGroupsInTDst("check for empty groups in TDST"      ));
        add( new emptyGroupsInTSrv("check for empty groups in TSRV"      ));

	add( new ExpandGroups(            "Expand groups"                ));

        // processors that expand objects with multiple addresses
        // check addresses against current address family using member
        // ipv6. If all addresses do not match, we may end up with
        // empty rule element.
        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

        if (ipv6)
            add( new DropIPv4Rules("drop ipv4 rules"));
        else
            add( new DropIPv6Rules("drop ipv6 rules"));

        add( new eliminateDuplicatesInOSRC("eliminate duplicates in OSRC"));
        add( new eliminateDuplicatesInODST("eliminate duplicates in ODST"));
        add( new eliminateDuplicatesInOSRV("eliminate duplicates in OSRV"));

        add( new swapMultiAddressObjectsInOSrc(
                 " swap MultiAddress -> MultiAddressRunTime in OSrc") );
        add( new swapMultiAddressObjectsInODst(
                 " swap MultiAddress -> MultiAddressRunTime in ODst") );

        add( new processMultiAddressObjectsInOSrc(
                 "process MultiAddress objects in OSrc") );
        add( new processMultiAddressObjectsInODst(
                 "process MultiAddress objects in ODst") );

        add( new doOSrvNegation(   "process negation in OSrv"            ));

        add( new convertToAtomicportForOSrv("convert to atomic rules in OSrv"));

        add( new classifyNATRule(           "classify NAT rule"           ));
        add( new splitSDNATRule(            "split SDNAT rules"           ));
        add( new classifyNATRule(           "reclassify rules"            ));
        add( new ConvertLoadBalancingRules( "convert load balancing rules"));
        add( new VerifyRules(               "verify rules"                ));

        add( new doOSrcNegation(            "process negation in OSrc"    ));
        add( new doODstNegation(            "process negation in ODst"    ));

/* call splitOnODst after processing negation */
        add( new splitOnODst(            "split on ODst"                  ));

        add( new portTranslationRules(   "port translation rules"         ));
        add( new specialCaseWithRedirect(
                 "special case with redirecting port translation rules" ) );

        if (fwopt->getBool("local_nat") )
        {
            if ( fwopt->getBool("firewall_is_part_of_any_and_networks") )
                add( new splitIfOSrcAny(  "split rule if OSrc is any"     ));

            add( new splitIfOSrcMatchesFw("split rule if OSrc matches FW" ));
        }

        add( new splitNONATRule("NAT rules that request no translation"));
        add( new localNATRule("process local NAT rules"));
//        add( new DNATforFW("process DNAT rules for packets originated on the firewall"));
        add( new decideOnChain(    "decide on chain"                    ) );
        add( new decideOnTarget(   "decide on target"                   ) );

        add( new splitODstForSNAT(
                 "split rule if objects in ODst belong to different subnets") );
        add( new ReplaceFirewallObjectsODst("replace firewall in ODst" ) );
	add( new ReplaceFirewallObjectsTSrc("replace firewall in TSrc" ) );
        add( new splitOnDynamicInterfaceInODst(
                 "split rule if ODst is dynamic interface" ) );
        add( new splitOnDynamicInterfaceInTSrc(
                 "split rule if TSrc is dynamic interface" ) );

        add( new ExpandMultipleAddresses("expand multiple addresses") );
        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

        if (ipv6)
            add( new DropIPv4Rules("drop ipv4 rules"));
        else
            add( new DropIPv6Rules("drop ipv6 rules"));

        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

        add( new specialCaseWithUnnumberedInterface(
                 "special cases with dynamic and unnumbered interfaces" ) );
        add( new checkForDynamicInterfacesOfOtherObjects(
                 "dynamic interfaces of other hosts and firewalls" ) );

        add( new verifyRuleWithMAC("verify rules using MAC address filtering"));
        add( new ExpandAddressRanges("expand address ranges") );
        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

        add( new splitMultiSrcAndDst(
                 "split rules where multiple srcs and dsts are present" ) );

        add( new splitServices("split on services") );
        add( new VerifyRules2("check correctness of TSrv") );
        add( new separatePortRanges("separate port ranges") );
        add( new separateSourcePorts("separate objects with src") );
        add( new separateSourceAndDestinationPorts(
                 "separate objects with both src and dest ports" ) );
        add( new prepareForMultiport("prepare for multiport") );
        add( new splitMultipleICMP("split rule with multiple ICMP services") );

        add( new ConvertToAtomicForAddresses("convert to atomic rules") );

        add( new addVirtualAddress("add virtual addresses") );

        add( new AssignInterface("assign rules to interfaces") );
        add( new dynamicInterfaceInODst("split if dynamic interface in ODst") );
	add( new dynamicInterfaceInTSrc(
                 "set target if dynamic interface in TSrc" ) );
        add( new convertInterfaceIdToStr("prepare interface assignments") );

        add( new countChainUsage("Count chain usage"));

        if (fwopt->getBool("use_iptables_restore"))
        {
            // bug #1812295: we should use PrintRuleIptRstEcho not only
            // when we have dynamic interfaces, but also when we have
            // address tables expanded at run time. Instead of checking
            // for all these conditions, just always use PrintRuleIptRstEcho
            printRule=new PrintRuleIptRstEcho(
                "generate code for iptables-restore using echo");
        } else
            printRule=new PrintRule("generate iptables shell script");

        add( printRule );

        add( new simplePrintProgress() );

        runRuleProcessors();


    } catch (FWException &ex) {
	error(ex.toString());
	exit(1);
    }
}


void NATCompiler_ipt::epilog()
{
    if (fwopt->getBool("use_iptables_restore"))
    {
        output << "#" << endl;
    }
}

string NATCompiler_ipt::flushAndSetDefaultPolicy()
{
    string res="";

/* printRule may be null if there are no NAT rules and we never ran compile() */
    if(printRule!=NULL)
    {
        res += printRule->_declareTable();
        res += printRule->_flushAndSetDefaultPolicy();
//    res += printRule->_printOptionalGlobalRules();
    }

    return res;
}

string NATCompiler_ipt::commit()
{
    string res="";

    if(printRule!=NULL)
    {
        res += printRule->_commit();
    }
    return res;
}


