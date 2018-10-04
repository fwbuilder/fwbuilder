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

#ifndef __NATCOMPILER_IPT_HH__
#define __NATCOMPILER_IPT_HH__


#include "fwcompiler/NATCompiler.h"
#include "fwbuilder/RuleElement.h"

#include <map>
#include <string>

#include <QMap>
#include <QSet>
#include <QString>


namespace libfwbuilder
{
    class Host;
    class IPService;
    class ICMPService;
    class TCPService;
    class UDPService;
    class RuleElementOSrc;
    class RuleElementODst;
    class RuleElementOSrv;
    class RuleElementTSrc;
    class RuleElementTDst;
    class RuleElementTSrv;
};

namespace fwcompiler
{

    class NATCompiler_ipt : public NATCompiler
    {
	protected:

        class PrintRule;

        NATCompiler_ipt::PrintRule *printRule;
        bool have_dynamic_interfaces;
        bool using_ipset;
        std::map<std::string, int> chain_usage_counter;

        // use minus_n_commands map to track creation of chains.
        // Using external map object for this to be able to track
        // new chains across different compiler runs (used to process
        // rules in different policy or nat objects)
        std::map<const std::string, bool> *minus_n_commands;

        // This map is located in CompilerDriver_ipt
        const std::map<std::string, std::list<std::string> > *branch_ruleset_to_chain_mapping;

        QMap<QString, libfwbuilder::FWObject*> regular_interfaces;

        
        static const std::list<std::string>& getStandardChains();
        std::string getInterfaceVarName(libfwbuilder::FWObject *iface,
                                        bool v6=false);
        std::string getAddressTableVarName(libfwbuilder::FWObject *iface);

        /**
         * internal: scans child objects of interface iface, both IPv4
         * and physAddress, and puts them in the list ol. Since iptables
         * supports matching on MAc addresses, we create objects of
         * the class combinedAddress here from each pair of physAddress
         * and IPV4
         */
        virtual void _expand_interface(libfwbuilder::Rule *rule,
                                       libfwbuilder::Interface *iface,
                                       std::list<libfwbuilder::FWObject*> &ol,
                                       bool expand_cluster_interfaces_fully);


        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

	/**
	 *  convert load balancing rules into DNAT rules with address
	 *  range in TDst. If objects in TDst do not constitute a
	 *  consecutive address range, abort with an error message
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertLoadBalancingRules);

	/**
	 * this processor spits SDNAT rule onto SNAT and DNAT rules.
         * SDNAT rule translates both source and destination.
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitSDNATRule);

	/**
	 * this processor spits NAT rule with action Branch to
         * generate iptables commands in all chains that it needs
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitNATBranchRule);

	/**
	 *  verifies correctness of the NAT rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules);

	/**
	 *  verifies correctness of the NAT rules - this one
	 *  specifically * checks for some inconsistencies between
	 *  OSrv and TSrv and should * only be used after
	 *  splitServices
	 */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules2);

	/**
	 *  verifies correctness of the combination of interface
	 *  specification and chain.
	 */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules3);

	/**
	 * splits rule with multiple objects in ODst. This needs to be
	 * done only * for DNAT rules. Call this processor when
	 * negation has been dealt with already.
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitOnODst);

	/**
	 *  splits rule with multiple service objects in OSrv onto
	 *  several rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitOnOSrv);

	/**
	 *  process special case: multiple objects in osrv and
	 *  tsrv!=any.  Need to convert to atomic by OSrv before
	 *  using classifyNATRule. 
	 */
        DECLARE_NAT_RULE_PROCESSOR(convertToAtomicportForOSrv);

	/**
	 *  process special case: rule that translates dest. port but
	 *  does not change addresses (tsrc==any, tdst==any, tsrv!=any)
	 *  Need to copy odst to tdst.
	 */
        DECLARE_NAT_RULE_PROCESSOR(portTranslationRules);

	/**
	 *  processor portTranslationRules copies ODst into TDst
         *  for rules that only do port translations and where TDst
	 *  is "any". In case ODst was firewall or one of its interfaces,
	 *  we should set rule type to NATRule::REDIRECT
	 */
        DECLARE_NAT_RULE_PROCESSOR(specialCaseWithRedirect);


        class splitRuleIfRuleElementIsDynamicInterface : public NATRuleProcessor
        {
            std::string re_type;
            public:
            splitRuleIfRuleElementIsDynamicInterface(const std::string &n,std::string _type):
                NATRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

	/**
	 *  splits rule if one of the objects in tsrc is * interface
	 *  with dynamic address
	 */
        class splitOnDynamicInterfaceInTSrc : public splitRuleIfRuleElementIsDynamicInterface
        {
            public:
            splitOnDynamicInterfaceInTSrc(const std::string &n):
                splitRuleIfRuleElementIsDynamicInterface(n,libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

	/**
	 *  splits rule if one of the objects in odst is * interface
	 *  with dynamic address
	 */
        class splitOnDynamicInterfaceInODst : public splitRuleIfRuleElementIsDynamicInterface
        {
            public:
            splitOnDynamicInterfaceInODst(const std::string &n):
                splitRuleIfRuleElementIsDynamicInterface(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };

	/**
	 * checks for the following situations: 
         *
         *  1. an unnumbered interface is in OSrc and rule rtype is Masq
         *  or SNAT (drop interface from src since source address is
         *  undertermined)
         *
         *  2. an unnumbered interface is in ODst and rule type is
         *  DNAT (drop interface since dest. address is undefined)
         * 
	 */
        friend class specialCaseWithUnnumberedInterface;
        class specialCaseWithUnnumberedInterface : public NATRuleProcessor
        {
            bool dropUnnumberedInterface(libfwbuilder::RuleElement *re);
            public:
            specialCaseWithUnnumberedInterface(const std::string &name) : NATRuleProcessor(name) {}
            virtual bool processNext();
        };

        friend class checkForDynamicInterfacesOfOtherObjects;
        class checkForDynamicInterfacesOfOtherObjects : public NATRuleProcessor
        {
            void findDynamicInterfaces(libfwbuilder::RuleElement *re,
                                       libfwbuilder::Rule        *rule);
            public:
            checkForDynamicInterfacesOfOtherObjects(const std::string &name) : NATRuleProcessor(name) {}
            virtual bool processNext();
        };


	/**
	 *  fills translated service with the copy of original srv
	 */
        DECLARE_NAT_RULE_PROCESSOR(fillTranslatedSrv);

	/**
	 * Assigns NAT rules to interfaces 
         *
         * This processor works together with
         * ReplaceFirewallObjectsTSrc and ConvertToAtomicRules. If the
         * first two left interface object in TSrc, AssignInterfaces
         * assigns this rule to the corresponding interface. Rule
         * wont'be assigned to any interface if object in TSrc is not
         * an interface or an address of interface.
	 */
        friend class AssignInterface;
        class AssignInterface : public NATRuleProcessor
        {
            public:
            AssignInterface(const std::string &name) : NATRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 *  calls OSConfigurator to add virtual * address to the
	 *  firewall if it is needed for NAT rule
	 */
        DECLARE_NAT_RULE_PROCESSOR(addVirtualAddress);

	/**
	 *   replaces references to the firewall in odst * with
	 *   references to its external interfaces
	 */
        DECLARE_NAT_RULE_PROCESSOR(ReplaceFirewallObjectsODst);

	/**
	 *   replaces references to the firewall in tsrc with
	 *   references to its interfaces in SNAT rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(ReplaceFirewallObjectsTSrc);

	/**
	 *  distinguishes SNAT from Masquerading (can do * this after
	 *  firewall objects has been replaced with its * interfaces
	 *  and basic NAT rule type has been determined)
	 */
        DECLARE_NAT_RULE_PROCESSOR(dynamicInterfaceInTSrc);

	/**
         *  forces use of masquerading instead of SNAT in all cases.
         */
        DECLARE_NAT_RULE_PROCESSOR(alwaysUseMasquerading);

	/**
	 *  takes care of dynamic interfaces in ODst  (if ODst contains
	 *  interface and its address is dynamic, replace it with any)
	 */
        DECLARE_NAT_RULE_PROCESSOR(dynamicInterfaceInODst);

        /**
         *  splits rule element if src or dst contains * address
         *  range. This inspector differs from the standard one * in
         *  the base class NATCompiler
         */
        DECLARE_NAT_RULE_PROCESSOR(ExpandAddressRanges);

	/**
	 * splits rules so multiport module can be used (only works for UDP and TCP)
	 */
        DECLARE_NAT_RULE_PROCESSOR(prepareForMultiport);

	/**
	 * splits rules using multiple ICMP services
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitMultipleICMP);

        /**
         *  deals with negation in OSrc 
         */
        DECLARE_NAT_RULE_PROCESSOR(doOSrcNegation);

        /**
         *  deals with negation in ODst
         */
        DECLARE_NAT_RULE_PROCESSOR(doODstNegation);

        /**
         *  deals with negation in OSrv 
         */
        DECLARE_NAT_RULE_PROCESSOR(doOSrvNegation);

        /**
         * splits DNAT rule if "Assume firewall is part of any" is ON
         * and OSrc is any. Need this to take care of the case with
         * packets originating on the firewall in DNAT rules.
         */
        DECLARE_NAT_RULE_PROCESSOR(splitIfOSrcAny);


	/**
	 * splits NONAT rule and assigns chains to PREROUTING,
	 * POSTROUTING and OUTPUT. Always call this processor before
	 * decideOnChain
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitNONATRule);


	/**
	 * sets chain and possibly splits a NAT rule if firewall or
	 * its interface is in OSrc.
	 */
        DECLARE_NAT_RULE_PROCESSOR(localNATRule);


	/**
	 * special case of DNAT rule for packets originated on the
	 * firewall itself * (should go to the OUTPUT chain)
	 */
        DECLARE_NAT_RULE_PROCESSOR(DNATforFW);

	/**
	 * decides what chain this rule should go to. 
	 */
        DECLARE_NAT_RULE_PROCESSOR(decideOnChain);

	/**
	 * decides on "jump to" chain
	 */
        DECLARE_NAT_RULE_PROCESSOR(decideOnTarget);

        /**
          * split rule for efficiency where multiple srcs & dsts are present
          */ 
   	DECLARE_NAT_RULE_PROCESSOR(splitMultiSrcAndDst);
	/**
	 * MAC address filtering is permitted only in DNAT rules (only
	 * in PREROUTING chain)
	 */
        DECLARE_NAT_RULE_PROCESSOR(verifyRuleWithMAC);

        /**
         *  eliminates duplicate objects in SRC. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInOSRC : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInOSRC(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in DST. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInODST : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInODST(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in SRV. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInOSRV : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInOSRV(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementOSrv::TYPENAME) {}
        };

        /**
         * Split rule if MultiAddress object is used in RE to make
         * sure it is single object.
         */
        class processMultiAddressObjectsInRE : public NATRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : NATRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };


        class processMultiAddressObjectsInOSrc : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInOSrc(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInODst : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInODst(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        class processMultiAddressObjectsInTSrc : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInTSrc(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInTDst : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInTDst(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementTDst::TYPENAME) {}
        };

	/**
	 * count how many times each user-defined chain we've created is
         * used. We should be able to drop unused chains.
	 */
        DECLARE_NAT_RULE_PROCESSOR(countChainUsage);

	/**
	 *   prints single policy rule, assuming all * groups have
	 *   been expanded, so source, destination and * service hold
	 *   exactly one object each, and this object is * not a
	 *   group.  Negation should also have been taken care of *
	 *   before this method is called.
	 */
        class PrintRule : public NATRuleProcessor
        {
            protected:

            bool init;
            bool print_once_on_top;
            bool minus_n_tracker_initialized;
            std::string current_rule_label;
            std::string version;
            void initializeMinusNTracker();

            QString getInterfaceName(libfwbuilder::RuleElement *itf_re);

            /*
             * Prints single --option with argument and negation "!"
             * taking into account the change that happened in iptables 1.4.3.1
             * that causes warning
             * Using intrapositioned negation (`--option ! this`) is deprecated in favor of extrapositioned (`! --option this`).
             */
            virtual std::string _printSingleOptionWithNegation(
                const std::string &option,
                libfwbuilder::RuleElement *rel,
                const std::string &arg);
            
            virtual std::string _createChain(const std::string &chain);
            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();
            virtual std::string _printRuleLabel(libfwbuilder::NATRule *r);
                
            virtual std::string _printProtocol(libfwbuilder::Service *srv);
            virtual std::string _printSrcService(libfwbuilder::RuleElementOSrv  *o);
            virtual std::string _printDstService(libfwbuilder::RuleElementOSrv  *o);

            virtual std::string _printICMP(libfwbuilder::ICMPService *srv);
            virtual std::string _printIP(libfwbuilder::IPService *srv);

            virtual std::string _printOPorts(int rs,int re);
            virtual std::string _printTPorts(int rs,int re);
            virtual std::string _printSrcPorts(libfwbuilder::Service *srv);
            virtual std::string _printDstPorts(libfwbuilder::Service *srv);
            virtual std::string _printSNATPorts(libfwbuilder::Service *srv);
            virtual std::string _printDNATPorts(libfwbuilder::Service *srv);

            virtual std::string _printMultiport(libfwbuilder::NATRule *r);
            virtual std::string _printAddr(libfwbuilder::Address  *o,
                                           bool print_mask=true,
                                           bool print_range=false);
            virtual std::string _printIpSetMatch(
                libfwbuilder::Address *o, libfwbuilder::RuleElement *rel);
            virtual std::string _printChainDirectionAndInterface(libfwbuilder::NATRule *r);
            virtual std::string _printSingleObjectNegation(libfwbuilder::RuleElement *rel);

            public:
            PrintRule(const std::string &name);
            void initialize();
            virtual std::string _declareTable();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);
            virtual bool processNext();
        };
        friend class NATCompiler_ipt::PrintRule;

        class PrintRuleIptRst : public PrintRule
        {
            virtual std::string _createChain(const std::string &chain);
            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();
            virtual std::string _printRuleLabel(libfwbuilder::NATRule *r);

            public:
            
            PrintRuleIptRst(const std::string &name) : PrintRule(name) {};
            virtual std::string _declareTable();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);
            virtual bool processNext();
        };
        friend class NATCompiler_ipt::PrintRuleIptRst;

        class PrintRuleIptRstEcho : public PrintRuleIptRst
        {
            virtual std::string _createChain(const std::string &chain);
            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();

            public:
            
            PrintRuleIptRstEcho(const std::string &name) : PrintRuleIptRst(name) {};
            virtual std::string _declareTable();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);
            virtual bool processNext();
        };
        friend class NATCompiler_ipt::PrintRuleIptRstEcho;


	virtual std::string myPlatformName();

	public:

	NATCompiler_ipt(libfwbuilder::FWObjectDatabase *_db,
                        libfwbuilder::Firewall *fw,
                        bool ipv6_policy,
                        fwcompiler::OSConfigurator *_oscnf,
                        std::map<const std::string, bool> *m_n_commands_map) :
        NATCompiler(_db, fw, ipv6_policy, _oscnf)
        {
            have_dynamic_interfaces=false;
            printRule=nullptr;
            minus_n_commands = m_n_commands_map;
            branch_ruleset_to_chain_mapping = nullptr;
        }

        /**
         * this method registers chain used for the ruleset (most
         * often branch rule set). Since rules in the same ruleset do
         * not use this chain as target, rule processor
         * countChainUsage considers it unused.  Registering it makes
         * sure its usage counter is > 0.
         */
        void registerRuleSetChain(const std::string &chain_name);

        virtual void verifyPlatform();
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
	
        void setHaveDynamicInterfaces(bool f) { have_dynamic_interfaces=f; }
        
        virtual std::string flushAndSetDefaultPolicy();
        virtual std::string printAutomaticRules();
        std::string commit();

        std::list<std::string> getUsedChains();
        
	static std::string getNewTmpChainName(libfwbuilder::NATRule *rule);

        void setRulesetToChainMapping(const std::map<std::string, std::list<std::string> > *m)
        { branch_ruleset_to_chain_mapping = m; }
       
    };


}

#endif
