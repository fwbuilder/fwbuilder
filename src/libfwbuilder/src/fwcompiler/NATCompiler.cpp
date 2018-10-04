/*

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/CustomService.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;


int NATCompiler::prolog()
{
    Compiler::prolog();

    NAT *nat = NAT::cast(fw->getFirstByType(NAT::TYPENAME));
    assert(nat);

    if (source_ruleset == nullptr) source_ruleset = nat;

    source_ruleset->renumberRules();

    temp_ruleset = new NAT();   // working copy of the policy
    fw->add( temp_ruleset  );

    temp_ruleset->setName(source_ruleset->getName());

    int global_num = 0;

    string label_prefix = "";
    if (source_ruleset->getName() != "NAT") label_prefix = source_ruleset->getName();

    int rule_counter = 0;
    for (FWObject::iterator i=source_ruleset->begin(); i!=source_ruleset->end(); i++)
    {
	Rule *r = Rule::cast(*i);
        if (r == nullptr) continue; // skip RuleSetOptions object

        /*
         * do not remove disabled rules just yet because some
         * compilers might use RuleSet::insertRuleAtTop() and other
         * similar methods from prolog() or
         * addPredefinedPolicyRules()() and these methods renumber
         * rules (labels stop matching rule positions when this is
         * done because labels are configured in prolog() method of
         * the base class. See fwbuilder ticket 1173)
         */
	//if (r->isDisabled()) continue;

	//r->setInterfaceId(-1);
        if (r->getLabel().empty())
            r->setLabel( createRuleLabel(label_prefix, "NAT", r->getPosition()) );

	r->setAbsRuleNumber(global_num); global_num++;
        rule_counter++;
    }

    initialized = true;

    return rule_counter;
}



bool NATCompiler::checkForShadowing(NATRule &r1, NATRule &r2)
{
    Address  *osrc1;  //=getFirstOSrc(&r1);
    Address  *odst1;  //=getFirstODst(&r1);
    Service  *osrv1;  //=getFirstOSrv(&r1);
			    
    Address  *osrc2;  //=getFirstOSrc(&r2);
    Address  *odst2;  //=getFirstODst(&r2);
    Service  *osrv2;  //=getFirstOSrv(&r2);

    FWObject::iterator i1 = r1.begin();
    osrc1 = Address::cast(FWReference::cast((*i1)->front())->getPointer());
    i1++;
    odst1 = Address::cast(FWReference::cast((*i1)->front())->getPointer());
    i1++;
    osrv1 = Service::cast(FWReference::cast((*i1)->front())->getPointer());

    i1 = r2.begin();
    osrc2 = Address::cast(FWReference::cast((*i1)->front())->getPointer());
    i1++;
    odst2 = Address::cast(FWReference::cast((*i1)->front())->getPointer());
    i1++;
    osrv2 = Service::cast(FWReference::cast((*i1)->front())->getPointer());

    if (osrc1==nullptr || odst1==nullptr || osrv1==nullptr)
        throw FWException("Can not compare rules because rule "+r1.getLabel()+" has a group in one of its elements. Aborting.");

    if (osrc2==nullptr || odst2==nullptr || osrv2==nullptr)
        throw FWException("Can not compare rules because rule "+r2.getLabel()+" has a group in one of its elements. Aborting.");

    return ( Compiler::checkForShadowing(*osrc1, *osrc2) && 
             Compiler::checkForShadowing(*odst1, *odst2) && 
             Compiler::checkForShadowing(*osrv1, *osrv2) );

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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()!=NATRule::Unknown) return true;

    RuleElementTSrc *tsrcre = rule->getTSrc();
    RuleElementTDst *tdstre = rule->getTDst();
    RuleElementTSrv *tsrvre = rule->getTSrv();

    Service  *osrv = compiler->getFirstOSrv(rule);

    Address  *tsrc = compiler->getFirstTSrc(rule);
    Address  *tdst = compiler->getFirstTDst(rule);
    Service  *tsrv = compiler->getFirstTSrv(rule);

    if (rule->getAction() == NATRule::Branch)
    {
	rule->setRuleType(NATRule::NATBranch);
        if (!tsrcre->isAny() || !tdstre->isAny() || !tsrvre->isAny())
        {
            tsrcre->clearChildren();
            tsrcre->setAnyElement();

            tdstre->clearChildren();
            tdstre->setAnyElement();

            tsrvre->clearChildren();
            tsrvre->setAnyElement();

            compiler->warning(
                    rule,
                    "Translated Src, Dst and Srv are ignored in the NAT "
                    "rule with action 'Branch'");
        }
        return true;
    }

    if (tsrc->isAny() && tdst->isAny() && tsrv->isAny())
    {
	rule->setRuleType(NATRule::NONAT);
	return true;
    }

    bool osrv_defines_src_port = false;
    (void)osrv_defines_src_port; //UNUSED
    bool osrv_defines_dst_port = false;
    (void)osrv_defines_dst_port; //UNUSED
    bool tsrv_translates_src_port = false;
    bool tsrv_translates_dst_port = false;

    if (TCPUDPService::cast(osrv) && TCPUDPService::cast(tsrv))
    {
        TCPUDPService *tu_osrv = TCPUDPService::cast(osrv);
        TCPUDPService *tu_tsrv = TCPUDPService::cast(tsrv);

        osrv_defines_src_port = 
            (tu_osrv->getSrcRangeStart() != 0 && tu_osrv->getDstRangeStart() == 0);
        osrv_defines_dst_port = 
            (tu_osrv->getSrcRangeStart() == 0 && tu_osrv->getDstRangeStart() != 0);

        tsrv_translates_src_port =
            (tu_tsrv->getSrcRangeStart() != 0 && tu_tsrv->getDstRangeStart() == 0);
        tsrv_translates_dst_port =
            (tu_tsrv->getSrcRangeStart() == 0 && tu_tsrv->getDstRangeStart() != 0);

        if (tsrv_translates_dst_port &&
            tu_osrv->getDstRangeStart() == tu_tsrv->getDstRangeStart() &&
            tu_osrv->getDstRangeEnd() == tu_tsrv->getDstRangeEnd())
            tsrv_translates_dst_port = false;  // osrv and tsrv define the same ports

        if (tsrv_translates_src_port &&
            tu_osrv->getSrcRangeStart() == tu_tsrv->getSrcRangeStart() &&
            tu_osrv->getSrcRangeEnd() == tu_tsrv->getSrcRangeEnd())
            tsrv_translates_src_port = false;  // osrv and tsrv define the same ports
    }

    if (!osrv->isAny() && !tsrv->isAny() && !( *osrv == *tsrv ) )  // have operator==, but do not have operator!=
    {
        bool translation_ok = true;
        if (osrv->getTypeName() != tsrv->getTypeName())
        {
            translation_ok = false;
            // see #1685. Custom service needs special treatment
            if (CustomService::isA(osrv) &&
                (Service::cast(osrv)->getProtocolName() == Service::cast(tsrv)->getProtocolName()))
                translation_ok = true;
        }

        if (!translation_ok)
            compiler->abort(rule,
                            "NAT rule can not change service types: " +
                            osrv->getTypeName() + " to " + tsrv->getTypeName());
    }


/*
 * SDNAT rule is rather special. We should split it onto two normal
 * rules, one SNAT and another DNAT and run this rule processor again
 * for each. This algorithm should be implemented for each platform
 * separately.  Platforms where it does not seem possible to implement
 * at all should catch SDNAT rules and abort in their own
 * verifyNATRule processor.
 */
    if (
        ( ! tsrc->isAny() && ! tdst->isAny() ) ||
        ( ! tsrc->isAny() && tsrv_translates_dst_port) ||
        ( ! tdst->isAny() && tsrv_translates_src_port)
    )
    {
        rule->setRuleType(NATRule::SDNAT);
	return true;
    }

    if (
        (! tsrc->isAny() && tdst->isAny()) ||
        (tsrc->isAny() && tdst->isAny() && tsrv_translates_src_port)
    )
    {
        if ( ! tsrc->isAny() && Network::isA(tsrc) )
/* 
 * this is Netnat rule ( NETMAP in iptables) 
 * we always do additional sanity checks in VerifyRules
 */
            rule->setRuleType(NATRule::SNetnat);
        else 
            rule->setRuleType(NATRule::SNAT);
        return true;
    }

    if (
        (tsrc->isAny() && ! tdst->isAny() )  ||
        (tsrc->isAny() && tdst->isAny() && tsrv_translates_dst_port)
    )
    {
/* this is load balancing rule if there are multiple objects in TDst */
        if ( tdstre->size()>1 ) rule->setRuleType(NATRule::LB);
        else
        {
            if (! tdst->isAny() && Network::isA(tdst) )
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

    compiler->abort(rule, "Unsupported NAT rule");
    return false;
}

bool NATCompiler::ExpandMultipleAddresses::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    switch (rule->getRuleType())
    {
    case NATRule::NONAT:
    case NATRule::Return:
    {
	rel=rule->getOSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
	rel=rule->getODst();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
        break;
    }

    case NATRule::SNAT:
    case NATRule::SDNAT:
    {
	rel=rule->getOSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
	rel=rule->getODst();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
	rel=rule->getTSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
	rel=rule->getTDst();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
        break;
    }

    case NATRule::DNAT:
    {
	rel=rule->getOSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
	rel=rule->getODst();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
	rel=rule->getTSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
	rel=rule->getTDst();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
        break;
    }

    case NATRule::Redirect:
    {
	rel=rule->getOSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, true);
	rel=rule->getODst();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
	rel=rule->getTSrc();    assert(rel);
        compiler->_expand_addr(rule, rel, false);
        break;
    }

    default: break;
    }
    return true;
}

bool NATCompiler::ExpandAddressRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

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

bool NATCompiler::expandGroupsInItfInb::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItfInb *itf = rule->getItfInb();
    compiler->expandGroupsInRuleElement(itf);
    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler::expandGroupsInItfOutb::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItfOutb *itf = rule->getItfOutb();
    compiler->expandGroupsInRuleElement(itf);
    tmp_queue.push_back(rule);
    return true;
}


bool NATCompiler::checkForUnnumbered::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if ( compiler->catchUnnumberedIfaceInRE( rule->getOSrc() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getODst() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getTSrc() ) ||
         compiler->catchUnnumberedIfaceInRE( rule->getTDst() ) )
        compiler->abort(
                            rule, "Can not use unnumbered interfaces in rules. ");

    tmp_queue.push_back(rule);
    return true;
}



bool NATCompiler::ConvertToAtomicForOriginal::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrc *osrc=rule->getOSrc();    assert(osrc);
    RuleElementODst *odst=rule->getODst();    assert(odst);
    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

    for (FWObject::iterator i1=osrc->begin(); i1!=osrc->end(); ++i1) 
    {
	for (FWObject::iterator i2=odst->begin(); i2!=odst->end(); ++i2) 
        {
	    for (FWObject::iterator i3=osrv->begin(); i3!=osrv->end(); ++i3) 
            {
                NATRule *r = compiler->dbcopy->createNATRule();
                r->duplicate(rule);
                compiler->temp_ruleset->add(r);
                
                FWObject *s;

                s=r->getOSrc();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i1 );

                s=r->getODst();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i2 );

                s=r->getOSrv();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i3 );

                tmp_queue.push_back(r);
	    }
	}
    }


    return true;
}


bool NATCompiler::ConvertToAtomicForAddresses::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrc *osrc=rule->getOSrc();    assert(osrc);
    RuleElementODst *odst=rule->getODst();    assert(odst);
#ifndef NDEBUG
    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);
#endif

    RuleElementTSrc *tsrc=rule->getTSrc();    assert(tsrc);
    RuleElementTDst *tdst=rule->getTDst();    assert(tdst);
#ifndef NDEBUG
    RuleElementTSrv *tsrv=rule->getTSrv();    assert(tsrv);
#endif

    for (FWObject::iterator i1=osrc->begin(); i1!=osrc->end(); ++i1) 
    {
	for (FWObject::iterator i2=odst->begin(); i2!=odst->end(); ++i2) 
        {
	    for (FWObject::iterator i4=tsrc->begin(); i4!=tsrc->end(); ++i4) 
            {
		for (FWObject::iterator i5=tdst->begin(); i5!=tdst->end(); ++i5) 
                {
		    NATRule *r = compiler->dbcopy->createNATRule();
		    r->duplicate(rule);
                    compiler->temp_ruleset->add(r);

		    FWObject *s;

		    s=r->getOSrc();	assert(s);
		    s->clearChildren();
		    s->addCopyOf( *i1 );

		    s=r->getODst();	assert(s);
		    s->clearChildren();
		    s->addCopyOf( *i2 );

//		    s=r->getOSrv();	assert(s);
//		    *s=*osrv;

		    s=r->getTSrc();	assert(s);
		    s->clearChildren();
		    s->addCopyOf( *i4 );

		    s=r->getTDst();	assert(s);
		    s->clearChildren();
		    s->addCopyOf( *i5 );

//		    s=r->getTSrv();	assert(s);
//		    *s=*tsrv;

		    tmp_queue.push_back(r);
		}
	    }
	}
    }

    return true;
}

bool NATCompiler::ConvertToAtomicForOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

    for (FWObject::iterator i1=osrv->begin(); i1!=osrv->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getOSrv();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}

bool NATCompiler::ConvertToAtomicForTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementTSrc *tsrc=rule->getTSrc();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTSrc();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}


bool NATCompiler::ConvertToAtomicForTDst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementTDst *tsrc=rule->getTDst();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTDst();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}


bool NATCompiler::ConvertToAtomicForTSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementTSrv *tsrc=rule->getTSrv();    assert(tsrc);

    for (FWObject::iterator i1=tsrc->begin(); i1!=tsrc->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s=r->getTSrv();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}

bool NATCompiler::ConvertToAtomicForItfInb::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementItfInb *itf_inb_re=rule->getItfInb();    assert(itf_inb_re);

    for (FWObject::iterator i1=itf_inb_re->begin(); i1!=itf_inb_re->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s = r->getItfInb();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}

bool NATCompiler::ConvertToAtomicForItfOutb::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementItfOutb *itf_outb_re=rule->getItfOutb();    assert(itf_outb_re);

    for (FWObject::iterator i1=itf_outb_re->begin(); i1!=itf_outb_re->end(); ++i1) 
    {
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);

        FWObject *s;

        s = r->getItfOutb();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }

    return true;
}





bool NATCompiler::ConvertToAtomic::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

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
			    NATRule *r = compiler->dbcopy->createNATRule();
			    r->duplicate(rule);
                            compiler->temp_ruleset->add(r);

			    FWObject *s;

			    s=r->getOSrc();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i1 );

			    s=r->getODst();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i2 );

			    s=r->getOSrv();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i3 );


			    s=r->getTSrc();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i4 );

			    s=r->getTDst();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i5 );

			    s=r->getTSrv();	assert(s);
			    s->clearChildren();
			    s->addCopyOf( *i6 );

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
        FWObject *o = FWReference::getObject(*i);
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);


    RuleElement *osrc=rule->getOSrc();
    RuleElement *odst=rule->getODst();

    string lbl=rule->getLabel();

    if ( ! checkRuleElement(osrc) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning(
                
                    rule, "MAC address matching is not supported. One or several MAC addresses removed from Original Source ");

        if (osrc->empty() || osrc->isAny())
            compiler->abort(
                
                    rule, "Original Source becomes 'Any' after all MAC addresses have been removed");

        last_rule_lbl=lbl;
    }


    if ( ! checkRuleElement(odst) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning(
                
                    rule, "MAC address matching is not supported. One or several MAC addresses removed from Original Destination ");

        if (odst->empty() || odst->isAny())
            compiler->abort(
                
                    rule, "Original Destination becomes 'Any' after all MAC addresses have been removed");

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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getRuleType()==NATRule::SNAT)
    {
        RuleElementODst *rel=rule->getODst();
        if (!rel->isAny() && rel->size()>1)
        {
            map<string, list<FWObject*> > il;
            for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
            {
                FWObject *o = FWReference::getObject(*i);
                Address *a=Address::cast(o);
                string iid="";
                Interface *iface=compiler->findInterfaceFor( a , compiler->fw );
                if (iface!=nullptr) iid=iface->getId();
                il[iid].push_back( a );
            }
            if (il.size()>1)
            {
                map<string, list<FWObject*> >::iterator j;
                for (j=il.begin(); j!=il.end(); j++)
                {
                    NATRule *r= compiler->dbcopy->createNATRule();
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

string NATCompiler::debugPrintRule(libfwbuilder::Rule *r)
{
    NATRule *rule = NATRule::cast(r);

    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementOSrv *osrvrel = rule->getOSrv();

    RuleElementTSrc *tsrcrel = rule->getTSrc();
    RuleElementTDst *tdstrel = rule->getTDst();
    RuleElementTSrv *tsrvrel = rule->getTSrv();

    RuleElementItfInb *itf_inb_rel = rule->getItfInb();
    RuleElementItfOutb *itf_outb_rel = rule->getItfOutb();

    ostringstream str;

//    str << setw(70) << setfill('-') << "-";

    int no = 0;
    FWObject::iterator i1 = osrcrel->begin();
    FWObject::iterator i2 = odstrel->begin(); 
    FWObject::iterator i3 = osrvrel->begin();

    FWObject::iterator i4 = tsrcrel->begin();
    FWObject::iterator i5 = tdstrel->begin(); 
    FWObject::iterator i6 = tsrvrel->begin();

    FWObject::iterator i7 = itf_inb_rel->begin();
    FWObject::iterator i8 = itf_outb_rel->begin();

    while ( i1!=osrcrel->end() || i2!=odstrel->end() || i3!=osrvrel->end() ||
            i4!=tsrcrel->end() || i5!=tdstrel->end() || i6!=tsrvrel->end() ||
            i7!=itf_inb_rel->end() || i8!=itf_outb_rel->end())
    {
        str  << endl;

        string osrc = " ";
        string odst = " ";
        string osrv = " ";

        string tsrc = " ";
        string tdst = " ";
        string tsrv = " ";

        string itf_inb = " ";
        string itf_outb = " ";

        if (osrcrel->getNeg()) osrc = "!";
        if (odstrel->getNeg()) odst = "!";
        if (osrvrel->getNeg()) osrv = "!";

        if (tsrcrel->getNeg()) tsrc = "!";
        if (tdstrel->getNeg()) tdst = "!";
        if (tsrvrel->getNeg()) tsrv = "!";

        if (itf_inb_rel->getNeg()) itf_inb = "!";
        if (itf_outb_rel->getNeg()) itf_outb = "!";

        int osrc_id = -1;
        int odst_id = -1;
        int osrv_id = -1;

        int tsrc_id = -1;
        int tdst_id = -1;
        int tsrv_id = -1;

        int itf_inb_id = -1;
        int itf_outb_id = -1;

        if (i1!=osrcrel->end())
        {
            FWObject *o = FWReference::getObject(*i1);
            osrc += o->getName();
            osrc_id = o->getId();
        }

        if (i2!=odstrel->end())
        {
            FWObject *o = FWReference::getObject(*i2);
            odst += o->getName();
            odst_id = o->getId();
        }

        if (i3!=osrvrel->end())
        {
            FWObject *o = FWReference::getObject(*i3);
            osrv += o->getName();
            osrv_id = o->getId();
        }

        if (i4!=tsrcrel->end())
        {
            FWObject *o = FWReference::getObject(*i4);
            tsrc += o->getName();
            tsrc_id = o->getId();
        }

        if (i5!=tdstrel->end())
        {
            FWObject *o = FWReference::getObject(*i5);
            tdst += o->getName();
            tdst_id = o->getId();
        }

        if (i6!=tsrvrel->end())
        {
            FWObject *o = FWReference::getObject(*i6);
            tsrv += o->getName();
            tsrv_id = o->getId();
        }

        if (i7!=itf_inb_rel->end())
        {
            FWObject *o = FWReference::getObject(*i7);
            itf_inb += o->getName();
            itf_inb_id = o->getId();
        }

        if (i8!=itf_outb_rel->end())
        {
            FWObject *o = FWReference::getObject(*i8);
            itf_outb += o->getName();
            itf_outb_id = o->getId();
        }

        int w = 0;
        if (no==0)
        {
            str << rule->getLabel();
            w = rule->getLabel().length();
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

        str << setw(10) << setfill(' ') << itf_inb.c_str() << "(" << itf_inb_id << ")";
        str << setw(10) << setfill(' ') << itf_outb.c_str() << "(" << itf_outb_id << ")";

        ++no;

        if ( i1!=osrcrel->end() ) ++i1;
        if ( i2!=odstrel->end() ) ++i2;
        if ( i3!=osrvrel->end() ) ++i3;
        if ( i4!=tsrcrel->end() ) ++i4;
        if ( i5!=tdstrel->end() ) ++i5;
        if ( i6!=tsrvrel->end() ) ++i6;
        if ( i7!=itf_inb_rel->end() ) ++i7;
        if ( i8!=itf_outb_rel->end() ) ++i8;
    }
    return str.str();

}

