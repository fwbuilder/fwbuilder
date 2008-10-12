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

#include "NATCompiler.h"

#include "fwbuilder/NAT.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/InterfacePolicy.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;


int NATCompiler::prolog()
{
    Compiler::prolog();

    FWObject  *nat=fw->getFirstByType(NAT::TYPENAME);
    assert(nat);

    combined_ruleset = new NAT();
    fw->add( combined_ruleset );

    temp_ruleset = new NAT();   // working copy of the policy
    fw->add( temp_ruleset  );


/*
 *  build combined policy by collapsing all the rules together.
 *  store ID of the interface in each rule of interface policy.
 *
 *  also calculate global numbers for all rules and store them, too.
 *  These are used to detect rule shadowing.
 */
    int global_num=0;


//    list<FWObject*> l3=nat->getByType(NATRule::TYPENAME);
//    for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) {

    FWObject *ruleset = source_ruleset;
    if (ruleset == NULL)
    {
        source_ruleset = RuleSet::cast(nat);
        ruleset = nat;
    }

    combined_ruleset->setName(ruleset->getName());
    temp_ruleset->setName(ruleset->getName());

    string label_prefix = "";
    if (ruleset->getName() != "NAT") label_prefix = ruleset->getName();

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++)
    {

	Rule *r= Rule::cast(*i);
	if (r->isDisabled()) continue;
	r->setInterfaceId(-1);
	r->setLabel( createRuleLabel(label_prefix, "NAT", r->getPosition()) );
	r->setAbsRuleNumber(global_num); global_num++;
        r->setUniqueId( FWObjectDatabase::getStringId(r->getId()) );
	combined_ruleset->add( r );
    }

    initialized=true;

    return combined_ruleset->size();
}



bool NATCompiler::checkForShadowing(NATRule &r1, NATRule &r2)
{
    Address  *osrc1=getFirstOSrc(&r1);
    Address  *odst1=getFirstODst(&r1);
    Service  *osrv1=getFirstOSrv(&r1);
			    
    Address  *osrc2=getFirstOSrc(&r2);
    Address  *odst2=getFirstODst(&r2);
    Service  *osrv2=getFirstOSrv(&r2);

    if (osrc1==NULL || odst1==NULL || osrv1==NULL)
        throw FWException("Can not compare rules because rule "+r1.getLabel()+" has a group in one of its elements. Aborting.");

    if (osrc2==NULL || odst2==NULL || osrv2==NULL)
        throw FWException("Can not compare rules because rule "+r2.getLabel()+" has a group in one of its elements. Aborting.");

    return ( fwcompiler::checkForShadowing(*osrc1, *osrc2) && 
             fwcompiler::checkForShadowing(*odst1, *odst2) && 
             fwcompiler::checkForShadowing(*osrv1, *osrv2) );

//    if ( (*osrc2 <= *osrc1) && (*odst2 <= *odst1) && (*osrv2 <= *osrv1) ) return  1;

    return false;
}

/*
 * TODO: implement this
 */
bool NATCompiler::cmpRules(NATRule&, NATRule&)
{
    return false;
}


bool NATCompiler::classifyNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()!=NATRule::Unknown) return true;

    RuleElementTDst *tdstre=rule->getTDst();

//    Address  *osrc=compiler->getFirstOSrc(rule);
//    Address  *odst=compiler->getFirstODst(rule);
    Service  *osrv=compiler->getFirstOSrv(rule);

    Address  *tsrc=compiler->getFirstTSrc(rule);
    Address  *tdst=compiler->getFirstTDst(rule);
    Service  *tsrv=compiler->getFirstTSrv(rule);

    if (   tsrc->isAny() &&   tdst->isAny() && tsrv->isAny() ) 
    {
	rule->setRuleType(NATRule::NONAT);
	return true;
    }

    if ( ! tsrc->isAny() &&   tdst->isAny() ) 
    {
        if ( Network::isA(tsrc) )
/* 
 * this is Netnat rule ( NETMAP in iptables) 
 * we always do additional sanity checks in VerifyRules
 */
            rule->setRuleType(NATRule::SNetnat);
        else 
            rule->setRuleType(NATRule::SNAT);
        return true;
    }

    if (   tsrc->isAny() && ! tdst->isAny() ) 
    {
/* this is load balancing rule if there are multiple objects in TDst */
        if ( tdstre->size()>1 ) rule->setRuleType(NATRule::LB);
        else
        {
            if ( Network::isA(tdst) )
/* 
 * this is Netnat rule ( NETMAP in iptables) 
 * we always do additional sanity checks in VerifyRules
 */
                rule->setRuleType(NATRule::DNetnat);
            else {
/*
 * treat it as redirect only if TDst is a firewall object. Use DNAT
 * if it is interface or an address; this allows for "redirects" to specific
 * interface on the firewall which comes useful for example if http proxy is
 * running only on internal interface.
 */
                if ( tdst->getId()==compiler->fw->getId())  rule->setRuleType(NATRule::Redirect);
                else                                        rule->setRuleType(NATRule::DNAT);

//            if ( compiler->complexMatch(tdst,compiler->fw) ) rule->setRuleType(NATRule::Redirect);
//            else                                             rule->setRuleType(NATRule::DNAT);
            }
        }
	return true;
    }

/*
 * SDNAT rule is rather special. We should split it onto two normal
 * rules, one SNAT and another DNAT and run this rule processor again
 * for each. This algorithm should be implemented for each platform
 * separately.  Platforms where it does not seem possible to implement
 * at all should catch SDNAT rules and abort in their own
 * verifyNATRule processor.
 */
    if ( ! tsrc->isAny() && ! tdst->isAny() ) 
    {
        rule->setRuleType(NATRule::SDNAT);
	return true;
    }

    if ( !( *osrv == *tsrv ) )  // have operator==, but do not have operator!=
    {
        rule->setRuleType(NATRule::DNAT);
	return true;
    }

    throw FWException("Unsupported NAT rule: "+rule->getLabel());
    return false;
}

bool NATCompiler::ExpandMultipleAddresses::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;
    if (rule->getRuleType()==NATRule::NONAT ||
        rule->getRuleType()==NATRule::Return)
    {
	rel=rule->getOSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getODst();    assert(rel); compiler->_expandAddr(rule,rel);
    }
    if (rule->getRuleType()==NATRule::SNAT)
    {
	rel=rule->getOSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getODst();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getTSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getTDst();    assert(rel); compiler->_expandAddr(rule,rel);
    }
    if (rule->getRuleType()==NATRule::DNAT)
    {
	rel=rule->getOSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getODst();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getTSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getTDst();    assert(rel); compiler->_expandAddr(rule,rel);
    }
    if (rule->getRuleType()==NATRule::Redirect)
    {
	rel=rule->getOSrc();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getODst();    assert(rel); compiler->_expandAddr(rule,rel);
	rel=rule->getTSrc();    assert(rel); compiler->_expandAddr(rule,rel);
    }
    return true;
}

bool NATCompiler::ExpandAddressRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    rel=rule->getOSrc();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getODst();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getTSrc();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getTDst();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);

    return true;
}




bool NATCompiler::ExpandGroups::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElement *osrc=rule->getOSrc();    assert(osrc);
    RuleElement *odst=rule->getODst();    assert(odst);
    RuleElement *osrv=rule->getOSrv();    assert(osrv);
    RuleElement *tsrc=rule->getTSrc();    assert(tsrc);
    RuleElement *tdst=rule->getTDst();    assert(tdst);
    RuleElement *tsrv=rule->getTSrv();    assert(tsrv);

    compiler->expandGroupsInRuleElement(osrc);
    compiler->expandGroupsInRuleElement(odst);
    compiler->expandGroupsInRuleElement(osrv);
    compiler->expandGroupsInRuleElement(tsrc);
    compiler->expandGroupsInRuleElement(tdst);
    compiler->expandGroupsInRuleElement(tsrv);

    return true;
}

bool NATCompiler::checkForUnnumbered::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if ( compiler->catchUnnumberedIfaceInRE( rule->getOSrc() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getODst() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getTSrc() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getTDst() ) )
        compiler->abort("Can not use unnumbered interfaces in rules. Rule "+rule->getLabel());

    tmp_queue.push_back(rule);
    return true;
}



bool NATCompiler::ConvertToAtomicForOriginal::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrc *osrc=rule->getOSrc();    assert(osrc);
    RuleElementODst *odst=rule->getODst();    assert(odst);
    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

    for (FWObject::iterator i1=osrc->begin(); i1!=osrc->end(); ++i1) 
    {
	for (FWObject::iterator i2=odst->begin(); i2!=odst->end(); ++i2) 
        {
	    for (FWObject::iterator i3=osrv->begin(); i3!=osrv->end(); ++i3) 
            {
                NATRule *r = NATRule::cast(
                    compiler->dbcopy->create(NATRule::TYPENAME) );
                r->duplicate(rule);
                compiler->temp_ruleset->add(r);
                
                FWObject *s;

                s=r->getOSrc();	assert(s);
                s->clearChildren();
                s->add( *i1 );

                s=r->getODst();	assert(s);
                s->clearChildren();
                s->add( *i2 );

                s=r->getOSrv();	assert(s);
                s->clearChildren();
                s->add( *i3 );

                tmp_queue.push_back(r);
	    }
	}
    }


    return true;
}


bool NATCompiler::ConvertToAtomicForAddresses::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrc *osrc=rule->getOSrc();    assert(osrc);
    RuleElementODst *odst=rule->getODst();    assert(odst);
    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

    RuleElementTSrc *tsrc=rule->getTSrc();    assert(tsrc);
    RuleElementTDst *tdst=rule->getTDst();    assert(tdst);
    RuleElementTSrv *tsrv=rule->getTSrv();    assert(tsrv);

    for (FWObject::iterator i1=osrc->begin(); i1!=osrc->end(); ++i1) 
    {
	for (FWObject::iterator i2=odst->begin(); i2!=odst->end(); ++i2) 
        {
	    for (FWObject::iterator i4=tsrc->begin(); i4!=tsrc->end(); ++i4) 
            {
		for (FWObject::iterator i5=tdst->begin(); i5!=tdst->end(); ++i5) 
                {
		    NATRule *r = NATRule::cast(
			compiler->dbcopy->create(NATRule::TYPENAME) );
		    r->duplicate(rule);
                    compiler->temp_ruleset->add(r);

		    FWObject *s;

		    s=r->getOSrc();	assert(s);
		    s->clearChildren();
		    s->add( *i1 );

		    s=r->getODst();	assert(s);
		    s->clearChildren();
		    s->add( *i2 );

//		    s=r->getOSrv();	assert(s);
//		    *s=*osrv;

		    s=r->getTSrc();	assert(s);
		    s->clearChildren();
		    s->add( *i4 );

		    s=r->getTDst();	assert(s);
		    s->clearChildren();
		    s->add( *i5 );

//		    s=r->getTSrv();	assert(s);
//		    *s=*tsrv;

		    tmp_queue.push_back(r);
		}
	    }
	}
    }

    return true;
}


bool NATCompiler::ConvertToAtomicForTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementTSrc *tsrc=rule->getTSrc();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = NATRule::cast(
            compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTSrc();	assert(s);
        s->clearChildren();
        s->add( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}


bool NATCompiler::ConvertToAtomicForTDst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementTDst *tsrc=rule->getTDst();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = NATRule::cast(
            compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTDst();	assert(s);
        s->clearChildren();
        s->add( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}


bool NATCompiler::ConvertToAtomicForTSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementTSrv *tsrc=rule->getTSrv();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = NATRule::cast(
            compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTSrv();	assert(s);
        s->clearChildren();
        s->add( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}




bool NATCompiler::ConvertToAtomic::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrc *osrc=rule->getOSrc();    assert(osrc);
    RuleElementODst *odst=rule->getODst();    assert(odst);
    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

    RuleElementTSrc *tsrc=rule->getTSrc();    assert(tsrc);
    RuleElementTDst *tdst=rule->getTDst();    assert(tdst);
    RuleElementTSrv *tsrv=rule->getTSrv();    assert(tsrv);

    for (FWObject::iterator i1=osrc->begin(); i1!=osrc->end(); ++i1) 
    {
	for (FWObject::iterator i2=odst->begin(); i2!=odst->end(); ++i2) 
        {
	    for (FWObject::iterator i3=osrv->begin(); i3!=osrv->end(); ++i3) 
            {
		for (FWObject::iterator i4=tsrc->begin(); i4!=tsrc->end(); ++i4) 
                {
		    for (FWObject::iterator i5=tdst->begin(); i5!=tdst->end(); ++i5) 
                    {
			for (FWObject::iterator i6=tsrv->begin(); i6!=tsrv->end(); ++i6) 
                        {
			    NATRule *r = NATRule::cast(
				compiler->dbcopy->create(NATRule::TYPENAME) );
			    r->duplicate(rule);
                            compiler->temp_ruleset->add(r);

			    FWObject *s;

			    s=r->getOSrc();	assert(s);
			    s->clearChildren();
			    s->add( *i1 );

			    s=r->getODst();	assert(s);
			    s->clearChildren();
			    s->add( *i2 );

			    s=r->getOSrv();	assert(s);
			    s->clearChildren();
			    s->add( *i3 );


			    s=r->getTSrc();	assert(s);
			    s->clearChildren();
			    s->add( *i4 );

			    s=r->getTDst();	assert(s);
			    s->clearChildren();
			    s->add( *i5 );

			    s=r->getTSrv();	assert(s);
			    s->clearChildren();
			    s->add( *i6 );

			    tmp_queue.push_back(r);

			}
		    }
		}
	    }
	}
    }
    return true;
}

bool NATCompiler::MACFiltering::checkRuleElement(RuleElement *re)
{
    bool  res=true;
    std::list<FWObject*> lst;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        if (physAddress::isA(o)) 
        {
            lst.push_back(o);
            res=false;
        }
    }
    for (FWObject::iterator i1=lst.begin(); i1!=lst.end(); i1++)   re->removeRef(*i1);
    return res;
}

bool NATCompiler::MACFiltering::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);


    RuleElement *osrc=rule->getOSrc();
    RuleElement *odst=rule->getODst();

    string lbl=rule->getLabel();

    if ( ! checkRuleElement(osrc) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning( "MAC address matching is not supported. One or several MAC addresses removed from Original Source in the rule "+lbl);

        if (osrc->empty() || osrc->isAny())
            compiler->abort("Original Source becomes 'Any' after all MAC addresses have been removed in the rule "+lbl);

        last_rule_lbl=lbl;
    }


    if ( ! checkRuleElement(odst) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning("MAC address matching is not supported. One or several MAC addresses removed from Original Destination in the rule "+lbl);

        if (odst->empty() || odst->isAny())
            compiler->abort("Original Destination becomes 'Any' after all MAC addresses have been removed in the rule "+lbl);

        last_rule_lbl=lbl;
    }

    return true;
}

/*
 * splits rule if ODst has multiple objects that belong to different
 * subnets
 */
bool NATCompiler::splitODstForSNAT::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getRuleType()==NATRule::SNAT)
    {
        RuleElementODst *rel=rule->getODst();
        if (!rel->isAny() && rel->size()>1)
        {
            map<string, list<FWObject*> > il;
            for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
            {
                FWObject *o= *i;
                if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
                Address *a=Address::cast(o);
                string iid="";
                Interface *iface=compiler->findInterfaceFor( a , compiler->fw );
                if (iface!=NULL) iid=iface->getId();
                il[iid].push_back( a );
            }
            if (il.size()>1)
            {
                map<string, list<FWObject*> >::iterator j;
                for (j=il.begin(); j!=il.end(); j++)
                {
                    NATRule *r= NATRule::cast(
                        compiler->dbcopy->create(NATRule::TYPENAME) );
                    compiler->temp_ruleset->add(r);
                    r->duplicate(rule);
                    RuleElementODst *nodst=r->getODst();
                    nodst->clearChildren();

                    list<FWObject*>::iterator k;
                    for (k= j->second.begin(); k!=j->second.end(); k++)
                        nodst->addRef( *k );

                    tmp_queue.push_back(r);
                }
            } else tmp_queue.push_back(rule);
        } else tmp_queue.push_back(rule);
    } else tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler::DropRulesByAddressFamilyAndServiceType::processNext()
{
    NATRule *rule = getNext(); if (rule==NULL) return false;
    RuleElementOSrc *osrc = rule->getOSrc();
    RuleElementODst *odst = rule->getODst();
    RuleElementOSrv *osrv = rule->getOSrv();

    RuleElementTSrc *tsrc = rule->getTSrc();
    RuleElementTDst *tdst = rule->getTDst();
    RuleElementTSrv *tsrv = rule->getTSrv();

    compiler->DropAddressFamilyInRE(osrc, drop_ipv6);
    compiler->DropAddressFamilyInRE(odst, drop_ipv6);
    compiler->DropByServiceTypeInRE(osrv, drop_ipv6);

    compiler->DropAddressFamilyInRE(tsrc, drop_ipv6);
    compiler->DropAddressFamilyInRE(tdst, drop_ipv6);
    compiler->DropByServiceTypeInRE(tsrv, drop_ipv6);

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler::dropRuleWithEmptyRE::processNext()
{
    NATRule *rule = getNext(); if (rule==NULL) return false;
    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementTSrc *tsrcrel = rule->getTSrc();
    RuleElementTDst *tdstrel = rule->getTDst();
    if ((osrcrel->size() == 0) || (odstrel->size() == 0)) return true;
    if ((tsrcrel->size() == 0) || (tdstrel->size() == 0)) return true;
//    Address *osrc = compiler->getFirstOSrc(rule);
//    Address *odst = compiler->getFirstODst(rule);
//    Address *tsrc = compiler->getFirstTSrc(rule);
//    Address *tdst = compiler->getFirstTDst(rule);
//    if (osrc!=NULL && odst!=NULL && tsrc!=NULL && tdst!=NULL)
//      tmp_queue.push_back(rule);

    tmp_queue.push_back(rule);
    return true;
}

string NATCompiler::debugPrintRule(libfwbuilder::Rule *r)
{
    NATRule *rule = NATRule::cast(r);

    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementOSrv *osrvrel = rule->getOSrv();

    RuleElementTSrc *tsrcrel = rule->getTSrc();
    RuleElementTDst *tdstrel = rule->getTDst();
    RuleElementTSrv *tsrvrel = rule->getTSrv();


    ostringstream str;

//    str << setw(70) << setfill('-') << "-";

    int no = 0;
    FWObject::iterator i1 = osrcrel->begin();
    FWObject::iterator i2 = odstrel->begin(); 
    FWObject::iterator i3 = osrvrel->begin();

    FWObject::iterator i4 = tsrcrel->begin();
    FWObject::iterator i5 = tdstrel->begin(); 
    FWObject::iterator i6 = tsrvrel->begin();

    while ( i1!=osrcrel->end() || i2!=odstrel->end() || i3!=osrvrel->end() ||
            i4!=tsrcrel->end() || i5!=tdstrel->end() || i6!=tsrvrel->end() ) {

        str  << endl;

        string osrc = " ";
        string odst = " ";
        string osrv = " ";

        string tsrc = " ";
        string tdst = " ";
        string tsrv = " ";

        int osrc_id = -1;
        int odst_id = -1;
        int osrv_id = -1;

        int tsrc_id = -1;
        int tdst_id = -1;
        int tsrv_id = -1;

        if (i1!=osrcrel->end()) {
            FWObject *o=*i1;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            osrc=o->getName();
            osrc_id=o->getId();
        }

        if (i2!=odstrel->end()) {
            FWObject *o=*i2;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            odst=o->getName();
            odst_id=o->getId();
        }

        if (i3!=osrvrel->end()) {
            FWObject *o=*i3;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            osrv=o->getName();
            osrv_id=o->getId();
        }

        if (i4!=tsrcrel->end()) {
            FWObject *o=*i4;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            tsrc=o->getName();
            tsrc_id=o->getId();
        }

        if (i5!=tdstrel->end()) {
            FWObject *o=*i5;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            tdst=o->getName();
            tdst_id=o->getId();
        }

        if (i6!=tsrvrel->end()) {
            FWObject *o=*i6;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            tsrv=o->getName();
            tsrv_id=o->getId();
        }

        int w=0;
        if (no==0) {
            str << rule->getLabel();
            w=rule->getLabel().length();
        }
        
        str << setw(8-w)  << setfill(' ') << " ";

        str << setw(16) << setfill(' ') << osrc.c_str() << "(" << osrc_id << ")";
        str << setw(16) << setfill(' ') << odst.c_str() << "(" << odst_id << ")";
        str << setw(10) << setfill(' ') << osrv.c_str() << "(" << osrv_id << ")";
//        str <<  endl;
//
//        str <<  setw(8)  << setfill(' ') << " ";
        str << setw(16) << setfill(' ') << tsrc.c_str() << "(" << tsrc_id << ")";
        str << setw(16) << setfill(' ') << tdst.c_str() << "(" << tdst_id << ")";
        str << setw(10) << setfill(' ') << tsrv.c_str() << "(" << tsrv_id << ")";


        ++no;

        if ( i1!=osrcrel->end() ) ++i1;
        if ( i2!=odstrel->end() ) ++i2;
        if ( i3!=osrvrel->end() ) ++i3;
        if ( i4!=tsrcrel->end() ) ++i4;
        if ( i5!=tdstrel->end() ) ++i5;
        if ( i6!=tsrvrel->end() ) ++i6;
    }
    return str.str();

}

