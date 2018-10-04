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

#ifndef __POLICY_COMPILER_HH__
#define __POLICY_COMPILER_HH__

#include "fwcompiler/Compiler.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include <string>

namespace fwcompiler {

    #define DECLARE_POLICY_RULE_PROCESSOR(_Name) \
        friend class _Name; \
	class _Name : public PolicyRuleProcessor { \
		public: \
		_Name(const std::string &n) :  \
			PolicyRuleProcessor(n) {}; \
                virtual ~_Name() {}; \
		virtual bool processNext(); \
	};


    class PolicyCompiler : public Compiler {

	protected:

	/**
	 *  this method scans source_ruleset looking for atomic rule
	 *  which yields non-empty intersection with atomic rule r.
	 *  
	 *  it can start scan either from the beginning of source_ruleset,
	 *  or from iterator 'start_here'
	 *
	 *  it returns iterator pointing at rule it has found (so we
	 *  can continue search later)
	 *
	 *  if parameter 'intersection' is not nullptr, it is assumed to
	 *  be a pointer to a pointer at the object of class Rule. It
	 *  is used to return a pointer at intersection of rule 'rule'
	 *  and rule this function had found
	 */
	std::list<libfwbuilder::FWObject*>::iterator 
	find_more_specific_rule(libfwbuilder::PolicyRule *r,
                                bool check_interface,
                                const std::list<libfwbuilder::FWObject*>::iterator &start_here,
                                const std::list<libfwbuilder::FWObject*>::iterator &stop_here,
                                libfwbuilder::PolicyRule **intersection=nullptr);


	public:

	PolicyCompiler(libfwbuilder::FWObjectDatabase *_db,
		       libfwbuilder::Firewall *fw,
                       bool ipv6_policy,
		       fwcompiler::OSConfigurator *_oscnf) :
        Compiler(_db, fw, ipv6_policy, _oscnf) {}

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

	
	/**
	 * this method substracts atomic rule r2 from atomic rule r1.
	 * Resulting rule may have multiple objects in src,dst or srv,
	 * so converting to atomic may be necessary. If rules can not
	 * be compared, then it throws an exception. If resultant rule
	 * is empty, method returns rule with empty src,dst,srv
	 *
	 * NOT IMPLEMENTED YET 
	 */
	libfwbuilder::Rule* getDifference(libfwbuilder::PolicyRule &r1,
					  libfwbuilder::PolicyRule &r2);

        /**
         * detects if rule r2  shades rule r1
         * returns:
         *
         *  true if r2 shades r1 
         */
        bool checkForShadowing(libfwbuilder::PolicyRule &r1,
                               libfwbuilder::PolicyRule &r2);

        /**
         * virtual method to let policy compiler check rules using
         * options specific for the given fw platform. Base class
         * PolicyCompiler has no visibility into platform-specific
         * options and can not do this.
         */
        virtual bool checkForShadowingPlatformSpecific(libfwbuilder::PolicyRule *r1,
                                                       libfwbuilder::PolicyRule *r2);

        
        /**
         * compare interfaces of rules r1 and r2.
         *
         * Return true if r2 shadows r1 (only inetrface rule element
         * is checked)
         *
         * If interface element is "all" (empty), it shadows any
         * specific interface in the other rule, also "all" shadows
         * "all". If neither is "all", return true if both rules refer
         * the same interface, otherwise return false.
         */
        bool checkInterfacesForShadowing(libfwbuilder::PolicyRule &r1,
                                         libfwbuilder::PolicyRule &r2);

        /**
         * detects if rules r1 and r2 are identical (that is, have the
         * same effect, rather than use the same objects)
         *
         * returns:
         *
         *  true if r1 is identical to r2
         */
        bool cmpRules(libfwbuilder::PolicyRule &r1,
                      libfwbuilder::PolicyRule &r2);

	/**
	 * this processor checks if the rule is associated with an
	 * interface and uses setInterfaceId to record its id. If the
	 * rule is associated with multiple interfaces, this processor
	 * splits the rule accordingly.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(InterfacePolicyRules);

	/**
	 * this class expands groups in src,dst,srv. It creates
	 * references to new objects "in place" (that is, it does not
	 * create new rules but rather uses rule elements of the old
	 * ones)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ExpandGroups);

	/**
	 * expand groups in Srv
	 */
        DECLARE_POLICY_RULE_PROCESSOR(expandGroupsInSrv);

	/**
	 * expand groups in Interface rule element
	 */
        DECLARE_POLICY_RULE_PROCESSOR(expandGroupsInItf);

        /**
         *  this inspector replaces references to hosts and firewalls
         *  in dst with references to their interfaces
         */
        class ExpandMultipleAddressesInSrc : public Compiler::expandMultipleAddressesInRE
        {
            public:
            ExpandMultipleAddressesInSrc(const std::string &n) :
                expandMultipleAddressesInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class ExpandMultipleAddressesInDst : public Compiler::expandMultipleAddressesInRE
        {
            public:
            ExpandMultipleAddressesInDst(const std::string &n) :
                expandMultipleAddressesInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };




        class ReplaceFirewallObjectWithSelfInSrc : public Compiler::ReplaceFirewallObjectWithSelfInRE
        {
            public:
            ReplaceFirewallObjectWithSelfInSrc(const std::string &n) :
                ReplaceFirewallObjectWithSelfInRE(
                    n, libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class ReplaceFirewallObjectWithSelfInDst : public Compiler::ReplaceFirewallObjectWithSelfInRE
        {
            public:
            ReplaceFirewallObjectWithSelfInDst(const std::string &n) :
                ReplaceFirewallObjectWithSelfInRE(
                    n, libfwbuilder::RuleElementDst::TYPENAME) {}
        };


	/**
         * single object negation in Src
	 */
        class singleObjectNegationSrc : public singleObjectNegation
        {
            public:
            singleObjectNegationSrc(const std::string &n):
                singleObjectNegation(n, libfwbuilder::RuleElementSrc::TYPENAME)
                {}
        };

	/**
         * single object negation in Dst
	 */
        class singleObjectNegationDst : public Compiler::singleObjectNegation
        {
            public:
            singleObjectNegationDst(const std::string &n):
                singleObjectNegation(n, libfwbuilder::RuleElementDst::TYPENAME)
                {}
        };

	/**
         * single object negation in Itf
	 */
        class singleObjectNegationItf : public Compiler::singleObjectNegation
        {
            public:
            singleObjectNegationItf(const std::string &n):
                singleObjectNegation(n, libfwbuilder::RuleElementItf::TYPENAME)
                {}
        };

        
	/**
	 * processes rules with negation in Itf.
         * Compiler::fullInterfaceNegationInRE replaces interface object
         * with a set of "other" interfaces of the firewall.
	 */
        class ItfNegation : public Compiler::fullInterfaceNegationInRE
        {
            public:
            ItfNegation(const std::string &name) :
            fullInterfaceNegationInRE(
                name, libfwbuilder::RuleElementItf::TYPENAME) {}
        };
           
	/**
	 * replace cluster interface objects with inetrfaces of the member
         * firewall in the Interface rule element
	 */
        class replaceClusterInterfaceInItf : public Compiler::replaceClusterInterfaceInItfRE
        {
            public:
            replaceClusterInterfaceInItf(const std::string &name) :
            replaceClusterInterfaceInItfRE(
                name, libfwbuilder::RuleElementItf::TYPENAME) {}
        };

        
	/**
	 *  this inspector replaces references to hosts and firewalls
	 *  in src or dst with references to their interfaces
	 *
	 * TODO: move to class Compiler because this might be useful
	 * for both PolicyCompiler and NATCompiler
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ExpandMultipleAddresses);

	/**
	 * this processor splits rule element if src or dst contains
	 * address range
	 */
        class addressRanges : public PolicyRuleProcessor
        {
            protected:
            void expandAddressRangesInSrc(libfwbuilder::PolicyRule *rule);
            void expandAddressRangesInDst(libfwbuilder::PolicyRule *rule);
            public:
            addressRanges(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };


	/**
	 *  checks for unnumbered interface in rule elements (one can
	 *  not use unnumbered interfaces in rules). Call after
	 *  ExpandMultipleAddresses 
	 */
        class checkForUnnumbered : public PolicyRuleProcessor
        {
            public:
            checkForUnnumbered(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };

	/**
	 * this processor converts to atomic rules using all combinations
	 * of objects in Src,Dst. It ignores Srv.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ConvertToAtomicForAddresses);

	/**
	 * this processor splits rule so that each atomic rule has
	 * exactly one Interval rule element
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ConvertToAtomicForIntervals);

	/**
	 * this processor converts to atomic rules
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ConvertToAtomic);

        /**
         *  deals with recursive groups in Src. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInSrc : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInSrc(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in Dst. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInDst : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInDst(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in Srv. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInSrv : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInSrv(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementSrv::TYPENAME) {}
        };




        /**
         *  deals with empty groups in Src. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInSrc : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInSrc(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        /**
         *  deals with empty groups in Dst. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInDst : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInDst(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        /**
         *  deals with empty groups in Srv. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInSrv : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInSrv(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementSrv::TYPENAME) {}
        };

        /**
         *  deals with empty groups in Itf. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInItf : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInItf(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementItf::TYPENAME) {}
        };


	/**
	 * checks for host and network objects with address 0.0.0.0
	 */
        class checkForZeroAddr : public PolicyRuleProcessor 
        {
            libfwbuilder::Address *findHostWithNoInterfaces(libfwbuilder::RuleElement *re);
            libfwbuilder::Address *findZeroAddress(libfwbuilder::RuleElement *re);
            public:
            checkForZeroAddr(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };

        /**
         * this is a base class for the family of rule processors that
         * need to be able to find more general rule rule
         */
        class findMoreGeneralRule : public PolicyRuleProcessor 
        {
            protected:
            /**
             *  this method scans tmp_queue looking for atomic rule
             *  which is more general than atomic rule 'r'. 
             *
             *  it can starts scan at position defined by iterator
             *  start_here and ends it at the position given by
             *  iterator stop_here
             *
             *  it returns iterator pointing at the rule it has found (so we
             *  can continue search later)
             */
            std::deque<libfwbuilder::Rule*>::iterator 
            find_more_general_rule(libfwbuilder::PolicyRule *r,
                                   bool check_interface,
                                   const std::deque<libfwbuilder::Rule*>::iterator &start_here,
                                   const std::deque<libfwbuilder::Rule*>::iterator &stop_here,
                                   bool reverse=false);
            public:
            findMoreGeneralRule(const std::string &n) : PolicyRuleProcessor(n) {}
        };


	/**
	 * this inspector scans rules and detects those which "shade"
	 * other rules below them
	 */
        class DetectShadowing : public findMoreGeneralRule
        {
            std::deque<libfwbuilder::Rule*> rules_seen_so_far;
            public:
            DetectShadowing(const std::string &n) : findMoreGeneralRule(n) {}
            virtual bool processNext();
        };

	/**
	 * this inspector scans rules and detects those which "shade"
	 * other rules above them. Use for non-terminating rules.
	 */
        class DetectShadowingForNonTerminatingRules : public findMoreGeneralRule
        {
            std::deque<libfwbuilder::Rule*> rules_seen_so_far;
            public:
            DetectShadowingForNonTerminatingRules(const std::string &n) : findMoreGeneralRule(n) {}
            virtual bool processNext();
        };
        
	/**
	 *  splits rule if one of the objects in Src is firewall
	 *  itself or a broadcast address for one of its interfaces or
	 *  multicast (multicast support is not implemented yet). This
	 *  is needed to properly choose direction and chain
	 *  later. Src may have multiple objects when this processor
	 *  is called.  For firewall the iptables code compiler
	 *  generates should go into OUTPUT chain, while for all other
	 *  objects it should go into FORWARD chain. That is why we
	 *  need to split the rule and place firewall object in the
	 *  src of the rule of its own.
	 */
        class splitIfSrcMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfSrcMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };


	/**
	 *  splits rule if one of the objects in Dst is firewall
	 *  itself. This is needed to properly choose direction and
	 *  chain later. See comment in splitIfInputChainObjectSrc.
	 */
        class splitIfDstMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfDstMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };


	/**
	 * many firewall platforms do not support filtering by MAC
	 * addresses. Issue warning if MAC address is used in the rule
	 * and remove it from the rule element. 
         *
         * Call this processor after ExpandMultipleAddresses.
	 */
        class MACFiltering : public PolicyRuleProcessor 
        {
            protected:
            std::string last_rule_lbl;
/* return value:
 * true  - ok
 * false - one or more MAC addresses have been removed, issue warning
 */
            bool  checkRuleElement(libfwbuilder::RuleElement *re);
            public:
            MACFiltering(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };


        /**
         * rule processors that replace MultiAddress objects with MultiAddressRunTime
         * equivalents
         */
        class swapMultiAddressObjectsInSrc : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInSrc(const std::string &n) :
                swapMultiAddressObjectsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class swapMultiAddressObjectsInDst : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInDst(const std::string &n) :
                swapMultiAddressObjectsInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        class RegisterGroupsAndTablesInSrc : public RegisterGroupsAndTablesInRE
        {
            public:
            RegisterGroupsAndTablesInSrc(const std::string &n) :
            RegisterGroupsAndTablesInRE(n, libfwbuilder::RuleElementSrc::TYPENAME)
            {}
        };

        class RegisterGroupsAndTablesInDst : public RegisterGroupsAndTablesInRE
        {
            public:
            RegisterGroupsAndTablesInDst(const std::string &n) :
            RegisterGroupsAndTablesInRE(n, libfwbuilder::RuleElementDst::TYPENAME)
            {}
        };

        
	virtual int prolog();

    };


};

#endif
