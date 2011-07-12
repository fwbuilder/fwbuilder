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

#ifndef __NAT_COMPILER_HH__
#define __NAT_COMPILER_HH__

#include "fwcompiler/Compiler.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include <string>

namespace fwcompiler {

    #define DECLARE_NAT_RULE_PROCESSOR(_Name) \
        friend class _Name; \
	class _Name : public NATRuleProcessor { \
		public: \
		_Name(const std::string &n) :  \
			NATRuleProcessor(n) {}; \
                virtual ~_Name() {}; \
		virtual bool processNext(); \
	};



    class NATCompiler : public Compiler {

	protected:

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);


	public:

        NATCompiler(libfwbuilder::FWObjectDatabase *_db,
		    libfwbuilder::Firewall *fw, bool ipv6_policy,
		    fwcompiler::OSConfigurator *_oscnf) :
        Compiler(_db, fw, ipv6_policy, _oscnf) {}

        virtual int prolog();

        /**
         * detect if rule r2 shades rule r1
         * returns:
         *
         *  true if r2 shades r1 
         */
        bool checkForShadowing(libfwbuilder::NATRule    &r1,
                               libfwbuilder::NATRule    &r2);

        /**
         * detects if rules r1 and r2 are identical (that is, have the
         * same effect, rather than use the same objects)
         *
         * returns:
         *
         *  true if r1 is identical to r2
         */
        bool cmpRules(libfwbuilder::NATRule &r1,
                      libfwbuilder::NATRule &r2);



        /**
         * this processor assigns NATRuleType value to the rule, thus
         * classifying it for further processing
         */
        DECLARE_NAT_RULE_PROCESSOR(classifyNATRule);

        /**
         * this class expands groups in src,dst,srv. It creates
         * references to new objects "in place" (that is, it does not
         * create new rules but rather uses rule elements of the old
         * ones)
         */
        DECLARE_NAT_RULE_PROCESSOR(ExpandGroups);

        /**
         *  this inspector replaces hosts and firewalls in src or dst
         *  with references to their interfaces
         */
        DECLARE_NAT_RULE_PROCESSOR(ExpandMultipleAddresses);

        /**
         *  checks for unnumbered interface in rule elements (one can
         *  not use unnumbered interfaces in rules). Call after
         *  ExpandMultipleAddresses so that we have single object in
         *  each rule element.
         */
        class checkForUnnumbered : public NATRuleProcessor
        {
            public:
            checkForUnnumbered(const std::string &n) : NATRuleProcessor(n) {}
            virtual bool processNext();
        };



	/**
	 * this processor splits rule element if src or dst contains
	 * address range
	 */
        DECLARE_NAT_RULE_PROCESSOR(ExpandAddressRanges);

        /**
         * splits SNAT rule if ODst has multiple objects that belong
         * to subnets different interfaces of the firewall are on. We
         * need this to be able to pick interfaces of the firewall
         * for SNAT rule in ReplaceFirewallObjectsTSrc
         */
        DECLARE_NAT_RULE_PROCESSOR(splitODstForSNAT);

	/**
	 * this processor converts to atomic rules using all combinations
	 * of objects in OSrc,ODst,TSrc,TDSt. It ignores OSrv and TSrv.
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForAddresses);

	/**
	 * this processor converts to atomic rules using all
	 * combinations of objects in OSrc,ODst,OSrv.  It ignores
	 * all "translated" rule elements.
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForOriginal);

	/**
	 * this processor converts to atomic rules only for OSrcv
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForOSrv);

	/**
	 * this processor converts to atomic rules only for TSrc
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForTSrc);

	/**
	 * this processor converts to atomic rules only for TDst
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForTDst);

	/**
	 * this processor converts to atomic rules only for TSrv
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForTSrv);

	/**
	 * this processor converts to atomic rules only for ItfInb
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForItfInb);

	/**
	 * this processor converts to atomic rules only for ItfOutb
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomicForItfOutb);

	/**
	 * this processor converts to atomic rules using all combinations
	 * of OSrc,ODst,OSrv,TSrc,TDst,TSrv
	 */
        DECLARE_NAT_RULE_PROCESSOR(ConvertToAtomic);

	/**
         * single object negation in OSrc
	 */
        class singleObjectNegationOSrc : public singleObjectNegation
        {
            public:
            singleObjectNegationOSrc(const std::string &n):
                singleObjectNegation(n,libfwbuilder::RuleElementOSrc::TYPENAME)
                {}
        };

	/**
         * single object negation in ODst
	 */
        class singleObjectNegationODst : public Compiler::singleObjectNegation
        {
            public:
            singleObjectNegationODst(const std::string &n):
                singleObjectNegation(n,libfwbuilder::RuleElementODst::TYPENAME)
                {}
        };

	/**
         * single object negation in ItfInb
	 */
        class singleObjectNegationItfInb : public Compiler::singleObjectNegation
        {
            public:
            singleObjectNegationItfInb(const std::string &n):
                singleObjectNegation(n,libfwbuilder::RuleElementItfInb::TYPENAME)
                {}
        };

	/**
         * single object negation in ItfOutb
	 */
        class singleObjectNegationItfOutb : public Compiler::singleObjectNegation
        {
            public:
            singleObjectNegationItfOutb(const std::string &n):
                singleObjectNegation(n,libfwbuilder::RuleElementItfOutb::TYPENAME)
                {}
        };
        
	/**
	 * processes rules with negation in ItfOutb
	 */
        class ItfOutbNegation : public Compiler::fullInterfaceNegationInRE
        {
            public:
            ItfOutbNegation(const std::string &name) :
            fullInterfaceNegationInRE(
                name, libfwbuilder::RuleElementItfOutb::TYPENAME) {}
        };
           
	/**
	 * processes rules with negation in ItfInb
	 */
        class ItfInbNegation : public Compiler::fullInterfaceNegationInRE
        {
            public:
            ItfInbNegation(const std::string &name) :
            fullInterfaceNegationInRE(
                name, libfwbuilder::RuleElementItfInb::TYPENAME) {}
        };
           
	/**
	 * replace cluster interface objects with inetrfaces of the member
         * firewall in the Interface rule element
	 */
        class replaceClusterInterfaceInItfInb : public Compiler::replaceClusterInterfaceInItfRE
        {
            public:
            replaceClusterInterfaceInItfInb(const std::string &name) :
            replaceClusterInterfaceInItfRE(
                name, libfwbuilder::RuleElementItfInb::TYPENAME) {}
        };

	/**
	 * replace cluster interface objects with inetrfaces of the member
         * firewall in the Interface rule element
	 */
        class replaceClusterInterfaceInItfOutb : public Compiler::replaceClusterInterfaceInItfRE
        {
            public:
            replaceClusterInterfaceInItfOutb(const std::string &name) :
            replaceClusterInterfaceInItfRE(
                name,libfwbuilder::RuleElementItfOutb::TYPENAME) {}
        };

	/**
	 * expand groups in Interface rule element
	 */
        DECLARE_NAT_RULE_PROCESSOR(expandGroupsInItfInb);

	/**
	 * expand groups in Interface rule element
	 */
        DECLARE_NAT_RULE_PROCESSOR(expandGroupsInItfOutb);

        /**
         *  deals with recursive groups in OSrc. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInOSrc : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInOSrc(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in ODst. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInODst : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInODst(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in OSrv. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInOSrv : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInOSrv(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementOSrv::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in TSrc. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInTSrc : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInTSrc(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in TDst. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInTDst : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInTDst(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementTDst::TYPENAME) {}
        };

        /**
         *  deals with recursive groups in TSrv. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInTSrv : public Compiler::recursiveGroupsInRE
        {
            public:
            recursiveGroupsInTSrv(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementTSrv::TYPENAME) {}
        };




        /**
         *  deals with empty groups in OSrc. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInOSrc : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInOSrc(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        /**
         *  deals with empty groups in ODst. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInODst : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInODst(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        /**
         *  deals with empty groups in OSrv. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInOSrv : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInOSrv(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementOSrv::TYPENAME) {}
        };

        /**
         *  deals with empty groups in TSrc. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInTSrc : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInTSrc(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

        /**
         *  deals with empty groups in TDst. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInTDst : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInTDst(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementTDst::TYPENAME) {}
        };

        /**
         *  deals with empty groups in TSrv. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInTSrv : public Compiler::emptyGroupsInRE
        {
            public:
            emptyGroupsInTSrv(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementTSrv::TYPENAME) {}
        };




	/**
	 *  splits rule if one of the objects in OSrc is firewall
	 *  itself. This is needed because some platforms require
	 *  special processing for rules dealing with packets
	 *  originated on the firewall, or headed for the firewall
	 *  (notably iptables).
	 */
        class splitIfOSrcMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfOSrcMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

	/**
	 *  itself. This is needed because some platforms require
	 *  special processing for rules dealing with packets
	 *  originated on the firewall, or headed for the firewall
	 *  (notably iptables).
	 *  splits rule if one of the objects in ODst is firewall
	 */
        class splitIfODstMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfODstMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };

	/**
	 *  splits rule if one of the objects in TSrc is firewall
	 *  itself. This is needed because some platforms require
	 *  special processing for rules dealing with packets
	 *  originated on the firewall, or headed for the firewall
	 *  (notably iptables).
	 */
        class splitIfTSrcMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfTSrcMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

	/**
	 *  splits rule if one of the objects in TDst is firewall
	 *  itself. This is needed because some platforms require
	 *  special processing for rules dealing with packets
	 *  originated on the firewall, or headed for the firewall
	 *  (notably iptables).
	 */
        class splitIfTDstMatchesFw : public Compiler::splitIfRuleElementMatchesFW
        {
            public:
            splitIfTDstMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementTDst::TYPENAME) {}
        };

	/**
	 * many firewall platforms do not support filtering by MAC
	 * addresses. Issue warning if MAC address is used in the rule
	 * and remove it from the rule element. 
         *
         * Call this processor after ExpandMultipleAddresses.
	 */
        class MACFiltering : public NATRuleProcessor
        {
            protected:
            std::string last_rule_lbl;
/* return value:
 * true  - ok
 * false - one or more MAC addresses have been removed, issue warning
 */
            bool  checkRuleElement(libfwbuilder::RuleElement *re);
            public:
            MACFiltering(const std::string &n) : NATRuleProcessor(n) {}
            virtual bool processNext();
        };

        /**
         * rule processors that replace MultiAddress objects with
         * MultiAddressRunTime equivalents
         */
        class swapMultiAddressObjectsInOSrc : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInOSrc(const std::string &n) :
            swapMultiAddressObjectsInRE(
                n, libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        class swapMultiAddressObjectsInODst : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInODst(const std::string &n) :
            swapMultiAddressObjectsInRE(
                n, libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        class swapMultiAddressObjectsInTSrc : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInTSrc(const std::string &n) :
            swapMultiAddressObjectsInRE(
                n, libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

        class swapMultiAddressObjectsInTDst : public Compiler::swapMultiAddressObjectsInRE
        {
            public:
            swapMultiAddressObjectsInTDst(const std::string &n) :
            swapMultiAddressObjectsInRE(
                n, libfwbuilder::RuleElementTDst::TYPENAME) {}
        };

        class RegisterGroupsAndTablesInOSrc : public RegisterGroupsAndTablesInRE
        {
            public:
            RegisterGroupsAndTablesInOSrc(const std::string &n) :
            RegisterGroupsAndTablesInRE(n, libfwbuilder::RuleElementOSrc::TYPENAME)
            {}
        };

        class RegisterGroupsAndTablesInODst : public RegisterGroupsAndTablesInRE
        {
            public:
            RegisterGroupsAndTablesInODst(const std::string &n) :
            RegisterGroupsAndTablesInRE(n, libfwbuilder::RuleElementODst::TYPENAME)
            {}
        };

        
    };


}

#endif
