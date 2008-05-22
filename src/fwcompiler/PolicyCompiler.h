/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler.h 1021 2007-05-08 02:13:37Z vkurland $

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
	 *  this method scans combined_ruleset looking for atomic rule
	 *  which yields non-empty intersection with atomic rule r.
	 *  
	 *  it can start scan either from the beginning of combined_ruleset,
	 *  or from iterator 'start_here'
	 *
	 *  it returns iterator pointing at rule it has found (so we
	 *  can continue search later)
	 *
	 *  if parameter 'intersection' is not NULL, it is assumed to
	 *  be a pointer to a pointer at the object of class Rule. It
	 *  is used to return a pointer at intersection of rule 'rule'
	 *  and rule this function had found
	 */
	std::list<libfwbuilder::FWObject*>::iterator 
	find_more_specific_rule(libfwbuilder::PolicyRule *r,
                                bool check_interface,
                                const std::list<libfwbuilder::FWObject*>::iterator &start_here,
                                const std::list<libfwbuilder::FWObject*>::iterator &stop_here,
                                libfwbuilder::PolicyRule **intersection=NULL);


	public:

	PolicyCompiler(libfwbuilder::FWObjectDatabase *_db,
		       const std::string &fwname,
                       bool ipv6_policy,
		       fwcompiler::OSConfigurator *_oscnf) :
        Compiler(_db, fwname, ipv6_policy, _oscnf) {}

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
	libfwbuilder::Rule* getDifference(const libfwbuilder::PolicyRule &r1,
					  const libfwbuilder::PolicyRule &r2);

        /**
         * detects if rule r2  shades rule r1
         * returns:
         *
         *  true if r2 shades r1 
         */
        bool checkForShadowing(const libfwbuilder::PolicyRule &r1,
                             const libfwbuilder::PolicyRule &r2);

        /**
         * detects if rules r1 and r2 are identical (that is, have the
         * same effect, rather than use the same objects)
         *
         * returns:
         *
         *  true if r1 is identical to r2
         */
        bool cmpRules(const libfwbuilder::PolicyRule &r1,
                      const libfwbuilder::PolicyRule &r2);


	/**
	 * processes rules with negation in Itf
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ItfNegation);

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
         *  this inspector replaces references to hosts and firewalls
         *  in dst with references to their interfaces
         *
         * TODO: move to class Compiler because this might be useful
         * for both PolicyCompiler and NATCompiler
         */
        DECLARE_POLICY_RULE_PROCESSOR(ExpandMultipleAddressesInDST);

        /**
         *  this inspector replaces references to hosts and firewalls
         *  in src with references to their interfaces
         *
         * TODO: move to class Compiler because this might be useful
         * for both PolicyCompiler and NATCompiler
         */
        DECLARE_POLICY_RULE_PROCESSOR(ExpandMultipleAddressesInSRC);

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
        DECLARE_POLICY_RULE_PROCESSOR(addressRanges);

	/**
	 * split rules with more than one service object, so that each
	 * rule has services with the same protocol
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitServices);

	/**
	 * separate TCP services with flags (can't use those in combination
	 * with others in groups of services)
	 */
        DECLARE_POLICY_RULE_PROCESSOR(separateTCPWithFlags);

	/**
	 * verify if custom services used in rules are configured for
	 * this platform
	 */
        DECLARE_POLICY_RULE_PROCESSOR(verifyCustomServices);

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
	 * simply check if TCPService object with "established" flag
         * set is used in Service and abort with an error saying that
         * target firewall does not support this. Use for pretty much
         * every platform except ipfw and router ACLs
	 */
        DECLARE_POLICY_RULE_PROCESSOR(CheckForTCPEstablished);

        /**
         * drop rules that have ipv4 or ipv6 addresses  (depending
         * on the argument ipv6 passed to the constructor)
         */
        class DropRulesByAddressFamily : public PolicyRuleProcessor
        {
            bool drop_ipv6;
            public:
            DropRulesByAddressFamily(const std::string &n,
                                     bool ipv6) : PolicyRuleProcessor(n)
            { drop_ipv6 = ipv6; }
            virtual bool processNext();
        };

        /**
         * Drop rule if any address object in source or destination is
         * ipv4 address.
         */
        class DropIPv4Rules : public DropRulesByAddressFamily
        {
            public:
            DropIPv4Rules(const std::string &n) :
              DropRulesByAddressFamily(n, false) {};
        };

        /**
         * Drop rule if any address object in source or destination is
         * ipv6 address.
         */
        class DropIPv6Rules : public DropRulesByAddressFamily
        {
            public:
            DropIPv6Rules(const std::string &n) :
              DropRulesByAddressFamily(n, true) {};
        };

	/**
	 * drop rules that have empty rule elements
	 */
        DECLARE_POLICY_RULE_PROCESSOR(dropRuleWithEmptyRE);

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

        
	virtual int prolog();

    };


};

#endif
