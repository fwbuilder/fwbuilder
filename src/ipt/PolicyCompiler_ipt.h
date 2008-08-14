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

#ifndef __POLICYCOMPILER_IPT_HH__
#define __POLICYCOMPILER_IPT_HH__

#include <fwbuilder/libfwbuilder-config.h>

#include "fwcompiler/PolicyCompiler.h"
#include "fwbuilder/RuleElement.h"
#include "config.h"
namespace libfwbuilder {
    class Interface;
    class IPService;
    class ICMPService;
    class TCPService;
    class UDPService;
    class RuleElementSrc;
    class RuleElementDst;
    class RuleElementSrv;
};

#define ANY_IP_OBJ_ID    "__any_ip_obj__"
#define ANY_ICMP_OBJ_ID  "__any_icmp_obj__"
#define ANY_TCP_OBJ_ID   "__any_tcp_obj__"
#define ANY_UDP_OBJ_ID   "__any_udp_obj__"
#define TCP_SYN_OBJ_ID   "__tcp_syn_obj__"
#define BCAST_255_OBJ_ID "__bcast_255_obj__"

// a functor to join list<string> into a string with separator sep
class join : public std::unary_function<std::string, void>
{
    std::string *result;
    std::string  separator;
public:
    join(std::string *res, const std::string &s)
    { result = res; separator = s; }
    void operator()(std::string &s);
};
    

namespace fwcompiler {


    class PolicyCompiler_ipt : public PolicyCompiler {

	protected:

        class PrintRule;

        PolicyCompiler_ipt::PrintRule *printRule;
        bool                           have_dynamic_interfaces;
        bool                           have_connmark;
        bool                           have_connmark_in_output;
        std::string                    my_table;

        std::map<std::string, int>     tmp_chain_no;
        std::map<std::string, int>     chain_usage_counter;


        static const std::list<std::string>& getStandardChains();
        
        PolicyCompiler_ipt::PrintRule* createPrintRuleProcessor();

        std::string getInterfaceVarName(libfwbuilder::FWObject *iface,
                                        bool v6=false);
        std::string getAddressTableVarName(libfwbuilder::FWObject *iface);

        bool newIptables(const std::string &version);

	/**
	 * Add some predefined rules controlled by checkboxes in
	 * firewall settings dialog
	 */
	void addPredefinedPolicyRules();

        /**
         * internal: scans child objects of interface iface, both IPv4
         * and physAddress, and puts them in the list ol. Since iptables
         * supports matching on MAC addresses, we create objects of
         * the class combinedAddress here from each pair of physAddress
         * and IPV4
         */
        virtual void _expandInterface(libfwbuilder::Interface *iface,
                                     std::list<libfwbuilder::FWObject*> &ol);


        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls PolicyCompiler::_internalPrintPolicyRule and then adds
         * chain and target at the end of the printed line
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

        /**
         * this processor drops all rules that require mangle table
         */
        DECLARE_POLICY_RULE_PROCESSOR(dropMangleTableRules);

        /**
         * adds few predefined (or "builtin") rules on top of the policy
         */
        class addPredefinedRules : public PolicyRuleProcessor
        {
            bool add_once;
            public:
            addPredefinedRules(const std::string &name) : PolicyRuleProcessor(name) 
            { add_once=true; }
            virtual bool processNext();
        };
        friend class addPredefinedRules;

	/**
	 * need to duplicate original action of this rule. We use this
	 * information later to decide whether we need to use "-m
	 * state --state new"
	 */
        DECLARE_POLICY_RULE_PROCESSOR(storeAction);

	/**
	 * set target and chain in case of branching
	 */
        class Branching : public PolicyRuleProcessor
        {
            public:
            Branching(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
            void expandBranch(libfwbuilder::PolicyRule *rule,
                              const std::string &parentRuleNum );
        };
        friend class Branching;

	/**
	 * set target and chain in case of route rules
	 */
        DECLARE_POLICY_RULE_PROCESSOR(Route);

	/**
	 * turns logging on if global logging override is used
	 */
        DECLARE_POLICY_RULE_PROCESSOR(Logging1);

	/**
	 * splits rule if logging is required and either src or dst is
	 * not any
	 */
        DECLARE_POLICY_RULE_PROCESSOR(Logging2);


	/**
	 * this processor checks if the rule is associated with an
	 * interface and uses setInterfaceId to record its id. If the
	 * rule is associated with multiple interfaces, this processor
	 * splits the rule accordingly. Unlike basic processor
	 * PolicyCompiler::InterfacePolicyrules, this processor tries
	 * to optimize rules applied to multiple interfaces using
	 * user-defined chains
	 */
        DECLARE_POLICY_RULE_PROCESSOR(InterfacePolicyRulesWithOptimization);

	/**
	 * if option "firewall is part of any" is OFF, replace all
	 * "Any" with "!fw" before checking for rule shadowing (if fw is
	 * not * part of "any", then "any" does not shadow the
	 * firewall)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(convertAnyToNotFWForShadowing);

	/**
	 * processes rules with negation in Src if it holds only one object
	 */
        DECLARE_POLICY_RULE_PROCESSOR(singleSrcNegation);

	/**
	 * processes rules with negation in Dst if it holds only one object
	 */
        DECLARE_POLICY_RULE_PROCESSOR(singleDstNegation);

	/**
	 * processes rules with negation in Srv if it holds only one object
	 */
        DECLARE_POLICY_RULE_PROCESSOR(singleSrvNegation);



	/**
	 * processes rules with negation in Src
         *
         * Argument dm defines mode of operation for this rule processor:
         *   if it is false, processor compiles the rule
         *   if it is true, it works in the mode of shadowing detection
         *
         * difference is that in shadowing detection mode it does not
         * replace objects in dst,srv and time with any so that we can
         * properly check shadowing later. Regular rule processor that
         * deals with negation in SRC replaces objects in rule
         * elements DST, SRV and Time with any which causes problems
         * because these rule elements then match those in other
         * rules, but they really should not match them because
         * originally they had specific object so only some packets
         * would match these rules.
	 */

        class SrcNegation : public PolicyRuleProcessor
        {
            bool shadowing_mode;
            public:
            SrcNegation(bool dm,
                        const std::string &name) : PolicyRuleProcessor(name)
            {
                shadowing_mode = dm;
            }
            virtual bool processNext();
        };

        
	/**
	 * processes rules with negation in Dst
	 */

        class DstNegation : public PolicyRuleProcessor
        {
            bool shadowing_mode;
            public:
            DstNegation(bool dm,
                        const std::string &name) : PolicyRuleProcessor(name)
            {
                shadowing_mode = dm;
            }
            virtual bool processNext();
        };

        
	/**
	 * processes rules with negation in Srv
	 */

        class SrvNegation : public PolicyRuleProcessor
        {
            bool shadowing_mode;
            public:
            SrvNegation(bool dm,
                        const std::string &name) : PolicyRuleProcessor(name)
            {
                shadowing_mode = dm;
            }
            virtual bool processNext();
        };

        
	/**
	 * processes rules with negation in Interval
	 */

        class TimeNegation : public PolicyRuleProcessor
        {
            bool shadowing_mode;
            public:
            TimeNegation(bool dm,
                        const std::string &name) : PolicyRuleProcessor(name)
            {
                shadowing_mode = dm;
            }
            virtual bool processNext();
        };

        
        
        /**
	 *   verifies combination of interface and * direction and
	 *   fills interface and direction. After this * predicate it
	 *   is guaranteed that both interface and * direction have
	 *   some value. In certain situations interface * ID may be
	 *   set to "nil" though (e.g. global policy rules).
	 */
        DECLARE_POLICY_RULE_PROCESSOR(InterfaceAndDirection);

	/**
	 * splits rule onto two if interface is defined and direction is Both
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfIfaceAndDirectionBoth);

        /**
         * If this is bridging firewall, broadcasts and multicasts go
         * to FORWARD chain unconditionally. There may be other
         * special conditions to be added later.
         */
        class bridgingFw : public PolicyRuleProcessor
        {
            bool checkForMatchingBroadcastAndMulticast(libfwbuilder::Address *addr);
            public:
            bridgingFw(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };


	/**
	 * set chain if Tag rule should go into PREROUTING
	 *
        DECLARE_POLICY_RULE_PROCESSOR(setChainIfTagInPrerouting);
        */
        
	/**
	 * set chain if Tag rule should go into PREROUTING
	 */
        DECLARE_POLICY_RULE_PROCESSOR(setChainPreroutingForTag);

	/**
	 * set chain if Tag rule should go into POSTROUTING
	 */
        DECLARE_POLICY_RULE_PROCESSOR(setChainPostroutingForTag);

	/**
	 * set chain for mangle table
	 */
        DECLARE_POLICY_RULE_PROCESSOR(setChainForMangle);

	/**
	 * check if we need to do CONNMARK --restore-mark in OUTPUT chain
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkForRestoreMarkInOutput);

        
	/**
	 * split rule if action is Tag and connmark option is activated
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfTagAndConnmark);

	/**
	 * split rule if Src==any 
	 * 
	 * This is special case since we assume that "any" includes
	 * also a firewall object. Packets headed to or from the
	 * firewall must be inspected by INPUT or OUTPUT chain, while
	 * packets crossing the firewall are inspected by FORWARD
	 * chain. If we assume that "any" also includes firewall
	 * itself, then we need to generate code for both FORWARD and
	 * INPUT/OUTPUT chains from the same rule. This processor
	 * splits the rule onto two and sets chain and direction in
	 * the second copy appropriately. It preserves original src
	 * and dst in both copies, it only changes chain and direction
	 * in the second copy.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfSrcAny);

	/**
	 * split rule if Dst==any. See comment in splitIfSrcAny
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfDstAny);

	/**
	 * split rule if Src==any 
	 * 
         * This works just like splitIfSrcAny, except is used in the
         * part of compiler that detects rule shadowing. While
         * compiling rules, we split the rule and set chains
         * appropriately (one rule gets into chain OUTPUT) but leave
         * SRC 'any' to avoid generating lots of address matches since
         * setting chain to OUTPUT is sufficient. We can not do this
         * while detecting shadowing and need to explicitly put the
         * firewall object in the first of the two rules we produce.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfSrcAnyForShadowing);

        /**
	 * split rule if Dst==any for shadowing detection. See comment
	 * in splitIfSrcAnyForShadowing
         */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfDstAnyForShadowing);

	/**
	 * split rule if Src==network the firewall is connected to
	 * 
	 * This is special case since we assume that network object
	 * that firewall has interface on includes also a firewall
	 * object. See comment in splitIfSrcAny for further explanation.
         *
         * Unlike in splitIfSrcAny, we can not assume rule element
         * holds a single object (since in splitIfSrcAny we are
         * looking for "any", we could rely on rule element containing
         * single object because "any" can only be there alone).
         *
	 * This processor splits the rule onto two and sets chain and
	 * direction in the second copy appropriately. It preserves
	 * original src and dst in both copies, it only changes chain
	 * and direction in the second copy.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfSrcFWNetwork);

	/**
	 * split rule if Dst==network the firewall is connected to. 
         * See comment in splitIfSrcAny
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfDstFWNetwork);

        /**
         * this is a special case of splitIfSrcAny. It splits the rule
         * but only if SRC has negation turned on, contains two or more
         * objects and one of these objects is firewall. 
         *
         * This processor should be called immediately before
         * processing negation. I tried to modify splitIfSrcAny to
         * split if there is negation and use it, but that lead to too
         * much overhead in the generated code for rules with negation
         * but no firewall in the rule element.
         */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfSrcNegAndFw);

        /**
         * similar to splitIfSrcNegAndFw
         */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfDstNegAndFw);

	/**
	 * checks for illegal combination of src, dst and direction
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkSrcAndDst1);

	/**
	 * checks for illegal combination of src, dst and direction
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkSrcAndDst2);

        /**
         * Split rule if MultiAddress object is used in RE to make
         * sure it is single object.
         */
        class processMultiAddressObjectsInRE : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : PolicyRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };


        class processMultiAddressObjectsInSrc : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInSrc(const std::string &n) :
                processMultiAddressObjectsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInDst : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInDst(const std::string &n) :
                processMultiAddressObjectsInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

	/**
	 * splits rule if firewall is in src and dst
	 */
        DECLARE_POLICY_RULE_PROCESSOR(specialCaseWithFW1);

	/**
	 * expands src and dst if both contain fw object. Unlike
	 * standard processor ExpandMultipleAddresses, this one
	 * uses loopback interface as well.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(specialCaseWithFW2);

	/**
	 * checks for the following situations: 
         *
         *  1. unnumbered interface is in source and direction is inbound
         *  (drop interface from src since source address is
         *  undertermined)
         *
         *  2. unnumbered interface is in source, direction is outbound
         *  and temporary chain (drop interface from the list, this
         *  rule has been created while processing negation. TODO: this
         *  is kludge, need to create separate temporary chain while
         *  doing negation in src if one of the objects is firewall)
         *
         *  3. unnumbered interface is in destination and direction is
         *  outbound (drop interface since dest. address is undefined)
         * 
	 */
        friend class specialCaseWithUnnumberedInterface;
        class specialCaseWithUnnumberedInterface : public PolicyRuleProcessor
        {
            bool dropUnnumberedInterface(libfwbuilder::RuleElement *re);
            public:
            specialCaseWithUnnumberedInterface(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

        friend class checkForDynamicInterfacesOfOtherObjects;
        class checkForDynamicInterfacesOfOtherObjects : public PolicyRuleProcessor
        {
            void findDynamicInterfaces(libfwbuilder::RuleElement *re,
                                       libfwbuilder::Rule        *rule);
            public:
            checkForDynamicInterfacesOfOtherObjects(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * expand object with multiple addresses but only if it is NOT
	 * the firewall we are working with. This processor is called
	 * right before decideOnChain but after groups have been
	 * expanded and splitIfSrcMatchesFw and splitIfDstMatchesFw
	 * have been called. Latter two make sure that firewall, if it
	 * is part of Src or Dst, will be a single object there when
         * this processor is called.
         *
         * 1. We need to expand objects with multiple addresses (such
         * as interfaces with many addresses) so that decideOnChain
         * would properly match when it calls
         * complexMatch. complexMatch does not match if its first
         * argument is an object with multiple addresses.
         *
         * 2. At the same time we need to keep firewall as a whole, so
         * that we can drop it later in removeFW, but only after
         * decideOnChain has determined that chain is INPUT or OUTPUT.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(expandMultipleAddressesIfNotFWinSrc);
        DECLARE_POLICY_RULE_PROCESSOR(expandMultipleAddressesIfNotFWinDst);

        /**
         * Compiler::_expandAddr skips loopback interface, so we need
         * to explicitly process the case when user puts loopback
         * interface object in the rule
         */
        friend class expandLoopbackInterfaceAddress;
        class expandLoopbackInterfaceAddress : public PolicyRuleProcessor
        {
            void replaceLoopbackWithItsAddress(libfwbuilder::RuleElement *re,
                                               libfwbuilder::Rule        *rule);
            public:
            expandLoopbackInterfaceAddress(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * decides what chain this rule should go to if Src contains
	 * firewall object. This is a simple case and we need to set
	 * chain before we try to split the rule if it contains
	 * network the firewall has interface on (splitIfSrcFWNetwork
	 * / splitIfDstFWNetwork).
	 */
        DECLARE_POLICY_RULE_PROCESSOR(decideOnChainIfSrcFW);

	/**
	 * Similar to the above, except it decides what chain this
	 * rule should go to if Dst contains firewall object.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(decideOnChainIfDstFW);

	/**
	 * This processor takes care of a special case where a rule
	 * with 'any' in both src and dst is used on a loopback
	 * interface and option 'assume firewall is part of any' is
	 * OFF. Processor splitIfIfaceAndDirectionBoth splits
	 * interface rule if its direction is "Both". This means that
	 * by the time this processor is called, the original rule
	 * "any any any accept both" on the loopback interface has
	 * already been converted to two rules :
         *
         * any any any accept inbound
         * any any any accept outbound
         * 
         * We do not have to split rule here, but rather just assign it to
         * INPUT/OUTPUT chains.
         *
         * This is mostly a patch for those who do not understand how
         * does "assume firewall is part of any" work. It also
         * eliminates useless code in the FORWARD chain that appear in
         * the rules on a loopback interface if the option "assume
         * firewall is part of any" is ON.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(decideOnChainIfLoopback);

        /**
         * define chain for rules with action Classify
         */
        DECLARE_POLICY_RULE_PROCESSOR(decideOnChainForClassify);

        /**
         * find non-terminating targets (such as MARK and
         * CLASSIFY). Put such rule in a separate chain and pass
         * control to it using "-g"
         */
        DECLARE_POLICY_RULE_PROCESSOR(splitNonTerminatingTargets);

        /**
         * drop rules with terminating targets. Used as part of the
         * shadowing detection for non-terminating rules in the mangle
         * table.
         */
        DECLARE_POLICY_RULE_PROCESSOR(dropTerminatingTargets);
        
	/**
	 * decides what chain this rule should go to if it has not
	 * been decided in decideOnChainIfFW
	 */
        DECLARE_POLICY_RULE_PROCESSOR(finalizeChain);

	/**
	 * decides on "jump to" chain
	 */
        DECLARE_POLICY_RULE_PROCESSOR(decideOnTarget);

	/**
	 * If chain has been determined to be INPUT or OUTPUT, we can
	 * remove firewall object from dst or src (resp.) NB: we can
	 * remove only reference to the whole firewall. We DO NOT
	 * remove reference to its interface or (in the future)
	 * address objects under interfaces. We do this only if we do
	 * not add any virtual addresses for NAT and if original rule
	 * did not have negation.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(removeFW);

	/**
	 * if rule option action_on_reject is empty, initialize it
	 * with global setting of this option.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(fillActionOnReject);


	/**
	 * iptables does not permit using "--m mac --mac-source" in
	 * the OUTPUT chain
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkMACinOUTPUTChain);

	/**
	 * iptables permits using "--m owner --uid-owner" only in
	 * the OUTPUT chain
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkUserServiceInWrongChains);


	/**
	 * expand groups in Srv
	 */
        DECLARE_POLICY_RULE_PROCESSOR(expandGroupsInSrv);

	/**
         * split a rule if action Reject is used in a rule with
         * Service 'any' and rule options do not specify what should
         * we use for Reject
         */
        class splitRuleIfSrvAnyActionReject :public PolicyRuleProcessor
        {
            std::map<int,bool> seen_rules;
            public:
            splitRuleIfSrvAnyActionReject(const std::string &name) :
                PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::splitRuleIfSrvAnyActionReject;

	/**
	 * separate TCP/UDP services that specify source port (can
	 * not be used in combination with destination port with
	 * multiport)
         *
         * Call this processor after groups have been expanded in Srv
	 */
        class splitServicesIfRejectWithTCPReset :public PolicyRuleProcessor
        {
            std::map<int,bool> seen_rules;
            public:
            splitServicesIfRejectWithTCPReset(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::splitServicesIfRejectWithTCPReset;

	/**
	 * This processor separates TCP/UDP services with port ranges
	 * (they can not be used with multiport). It also separates
	 * rules using "Any UDP" and "Any TCP" objects (they have
	 * all ports set to zero)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(separatePortRanges);

	/**
	 * separate TCP/UDP services that specify source port (can
	 * not be used in combination with destination port with
	 * multiport)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(separateSrcPort);


	/**
	 * deals with special cases with some known custom services
	 */
        DECLARE_POLICY_RULE_PROCESSOR(specialCasesWithCustomServices);

	/**
	 * optimize rules in case we deal with one or few objects in
	 * one rule element and lots of objects in the other two
	 */
        class optimize1 : public PolicyRuleProcessor
        {
            void  optimizeForRuleElement(libfwbuilder::PolicyRule *rule,
                                         const std::string  &re_type);
            public:
            optimize1(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::optimize1;

	/**
	 * simple optimization: if the rule is "final" and its action
	 * does not need protocol specification (it is _not_ -j REJECT
	 * --reject-with tcp-reset), then make sure service is
	 * "any". The "final" is such rule that defines the actual
	 * built-in chain ACCEPT/DROP/REJECT and * should not be
	 * further split or processed in any way; such rule for
	 * example is created in Logging and negations)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(optimize2);
        friend class PolicyCompiler_ipt::optimize2;

        /**
         * remove duplicate rules
         */
        class optimize3 : public PolicyRuleProcessor
        {
            std::map<std::string, bool> rules_seen_so_far;
            PolicyCompiler_ipt::PrintRule *printRule;
            public:
            optimize3(const std::string &name) : PolicyRuleProcessor(name){
                printRule=NULL;
            }
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::optimize3;


	/**
	 * split rules so multiport module can be used
	 */
        DECLARE_POLICY_RULE_PROCESSOR(prepareForMultiport);

        /**
         *  eliminates duplicate objects in SRC. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInSRC : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInSRC(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in DST. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInDST : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInDST(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in SRV. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInSRV : public eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInSRV(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementSrv::TYPENAME) {}
        };

	/**
	 * process action 'Accounting'
	 */
        DECLARE_POLICY_RULE_PROCESSOR(accounting);

        
        /**
         * if action is Continue and logging is off, skip this rule.
         * We only use action Continue to log some packets without making
         * policy decision
         */
        DECLARE_POLICY_RULE_PROCESSOR(SkipActionContinueWithNoLogging);

	/**
	 * count how many times each user-defined chain we've created is
         * used. We should be able to drop unused chains.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(countChainUsage);

        
	/**
	 *  prints single policy rule, assuming all groups have been
	 *  expanded, so source, destination and service hold exactly
	 *  one object each, and this object is not a group.  Negation
	 *  should also have been taken care of before this method is
	 *  called.
         *
         *  This processor is not necessarily the last in the
         *  conveyor, so it should push rules back to tmp_queue (for
         *  example there could be progress indicator processor after
         *  this one)
	 */
        class PrintRule : public PolicyRuleProcessor
        {
            protected:
            
            bool                             init;
            bool                             print_once_on_top;
            std::string                      current_rule_label;
            std::map<const std::string,bool> chains;

            virtual std::string _createChain(const std::string &chain);
            virtual std::string _printRuleLabel(libfwbuilder::PolicyRule *r);

            virtual std::string _printSrcService(libfwbuilder::RuleElementSrv  *o);
            virtual std::string _printDstService(libfwbuilder::RuleElementSrv  *o);
            virtual std::string _printProtocol(libfwbuilder::Service *srv);

            virtual std::string _printPorts(int rs,int re);
            virtual std::string _printSrcPorts(libfwbuilder::Service *srv);
            virtual std::string _printDstPorts(libfwbuilder::Service *srv);
            virtual std::string _printICMP(libfwbuilder::ICMPService *srv);
            virtual std::string _printIP(libfwbuilder::IPService *srv);

            virtual std::string _printTCPFlags(libfwbuilder::TCPService *srv);

            virtual std::string _printAddr(libfwbuilder::Address  *o);
            virtual std::string _printSingleObjectNegation(libfwbuilder::RuleElement *rel);

            virtual std::string _printChain(libfwbuilder::PolicyRule *r);
            virtual std::string _printTarget(libfwbuilder::PolicyRule *r);
            virtual std::string _printModules(libfwbuilder::PolicyRule *r);
            virtual std::string _printDirectionAndInterface(libfwbuilder::PolicyRule *r);
            virtual std::string _printMultiport(libfwbuilder::PolicyRule *r);

            virtual std::string _printTimeInterval(libfwbuilder::PolicyRule *r);

            virtual std::string _printLogParameters(libfwbuilder::PolicyRule *r);
            virtual std::string _printLogPrefix(const std::string &rule_n,
						const std::string &action,
						const std::string &interf,
						const std::string &chain,
						const std::string &rule_label,
						const std::string &prefix);
            virtual std::string _printLogPrefix(libfwbuilder::PolicyRule *r,
						const std::string &prefix);
            virtual std::string _printActionOnReject(libfwbuilder::PolicyRule *r);
            virtual std::string _printLimit(libfwbuilder::PolicyRule *r);

            public:

            PrintRule(const std::string &name);
            virtual std::string _printGlobalLogParameters();
            virtual std::string _printOptionalGlobalRules();
            virtual std::string _declareTable();
            virtual std::string _flushAndSetDefaultPolicy();
            virtual std::string _clampTcpToMssRule();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);

            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();
            
            virtual bool processNext();

            std::string PolicyRuleToString(libfwbuilder::PolicyRule *r);

        };
        friend class PolicyCompiler_ipt::PrintRule;

        class PrintRuleIptRst : public PrintRule
        {
            virtual std::string _createChain(const std::string &chain);
            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();
            virtual std::string _printRuleLabel(libfwbuilder::PolicyRule *r);

            public:
            PrintRuleIptRst(const std::string &name) : PrintRule(name) {};
            virtual std::string _declareTable();
            virtual std::string _flushAndSetDefaultPolicy();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);

            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::PrintRuleIptRst;

        class PrintRuleIptRstEcho : public PrintRuleIptRst
        {
            virtual std::string _createChain(const std::string &chain);
            virtual std::string _startRuleLine();
            virtual std::string _endRuleLine();

            public:
            PrintRuleIptRstEcho(const std::string &name) : PrintRuleIptRst(name) {};
            virtual std::string _declareTable();
            virtual std::string _flushAndSetDefaultPolicy();
            virtual std::string _commit();
            virtual std::string _quote(const std::string &s);

            virtual bool processNext();
        };
        friend class PolicyCompiler_ipt::PrintRuleIptRstEcho;


	virtual std::string myPlatformName();

        /**
         * TODO: move these two to class fwcompiler::PolicyCompiler,
         * then create enum for all possible actions on reject in that
         * class and use it instead of string.
         */
        std::string getActionOnReject(libfwbuilder::PolicyRule *rule);
        bool isActionOnRejectTCPRST(libfwbuilder::PolicyRule *rule);
        void resetActionOnReject(libfwbuilder::PolicyRule *rule);

	public:

	PolicyCompiler_ipt(libfwbuilder::FWObjectDatabase *_db,
                           const std::string &fwname,
                           bool ipv6_policy,
                           fwcompiler::OSConfigurator *_oscnf) :
        PolicyCompiler(_db, fwname, ipv6_policy, _oscnf)
        {
            have_dynamic_interfaces = false;
            have_connmark = false;
            have_connmark_in_output = false;
            printRule = NULL;
            my_table = "filter";
        }


	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        /**
         * addRuleFilter() is a hook where we can add a rule processor to filter
         * some of the rules out before we begin actual processing
         */
        virtual void addRuleFilter();

        /**
         * this method registers chain used for the ruleset (most
         * often branch rule set). Since rules in the same ruleset do
         * not use this chain as target, rule processor
         * countChainUsage considers it unused.  Registering it makes
         * sure its usage counter is > 0.
         */
        void registerRuleSetChain(const std::string &chain_name);
        
        void setHaveDynamicInterfaces(bool f) { have_dynamic_interfaces=f; }
        
        virtual std::string flushAndSetDefaultPolicy();
        std::string commit();

	std::string getNewTmpChainName(libfwbuilder::PolicyRule *rule);
	std::string getNewChainName(libfwbuilder::PolicyRule *rule,libfwbuilder::Interface *rule_iface);

    };


}

#endif
