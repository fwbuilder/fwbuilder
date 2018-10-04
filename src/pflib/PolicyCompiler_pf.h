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

#ifndef __POLICYCOMPILER_PF_HH
#define __POLICYCOMPILER_PF_HH

#include "fwcompiler/PolicyCompiler.h"

#include "NATCompiler_pf.h"
#include "TableFactory.h"

#include <QMap>
#include <QSet>
#include <QString>


namespace libfwbuilder
{
    class IPService;
    class ICMPService;
    class TCPService;
    class UDPService;
    class RuleElementSrc;
    class RuleElementDst;
    class RuleElementSrv;
    class IPv4;
};


namespace fwcompiler
{

    class PolicyCompiler_pf : public PolicyCompiler
    {
	protected:

        /**
         * virtual method to let policy compiler check rules using
         * options specific for the given fw platform. Base class
         * PolicyCompiler has no visibility into platform-specific
         * options and can not do this.
         */
        virtual bool checkForShadowingPlatformSpecific(libfwbuilder::PolicyRule *r1,
                                                       libfwbuilder::PolicyRule *r2);

	/**
	 *   splits rule if one of the objects in Src * is firewall
	 *   itself. This is needed to properly choose direction *
	 *   later in filDirection
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfFirewallInSrc);


	/**
	 *   splits rule if one of the objects in Dst * is firewall
	 *   itself. This is needed to properly choose direction *
	 *   later in filDirection
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfFirewallInDst);


        /**
	 *   decides on direction if it is empty.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(fillDirection);

	/**
	 * split rules if direction is "Both"
	 */
        DECLARE_POLICY_RULE_PROCESSOR(SplitDirection);

	/**
	 * Option "scrub" does not accept "quick" and therefore does not
	 * stop matching sequence. We need to split rule onto two, the first
	 * will be generated with action "scrub", while the second one with
	 * action "pass" and option "quick"
	 */
        DECLARE_POLICY_RULE_PROCESSOR(ProcessScrubOption);

	/**
	 *  checks for the services which require * special treatment.
	 *  Some of these will be checking for * source or destination
	 *  object as well because special * command may need to be
	 *  generated in case source or * destination is a firewall
	 *  itself. Therefore this processor * should be called after
	 *  converting to atomic rules, but * before interface
	 *  addresses in source and destination are * expanded.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(SpecialServices);

	/**
	 *  sets 'quick' flag on rules
	 */
        DECLARE_POLICY_RULE_PROCESSOR(setQuickFlag);

	/**
	 *  deals with negation in Src in policy rules.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(doSrcNegation);

	/**
	 *  deals with negation in Dst in policy rules.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(doDstNegation);

	/**
	 *  deals with negation in Srv in policy rules.
	 *
	 * NOT IMPLEMENTED 
	 */
        DECLARE_POLICY_RULE_PROCESSOR(doSrvNegation);

        /**
         *  Replace carp interface in the "Interface" rule element
         *  with firewall's real interface
         */
        class replaceFailoverInterfaceInItf : public replaceFailoverInterfaceInRE
        {
            public:
            replaceFailoverInterfaceInItf(const std::string &n) :
                replaceFailoverInterfaceInRE(n, libfwbuilder::RuleElementItf::TYPENAME) {}
        };

        /**
         * like standard processor swapMultiAddressObjectsInRE,
         * but swaps compile-time address tables
         *
         * We need this because unlike on other platforms, we need to
         * generate code for compile-time AddressTables using their
         * object name (to name the table after that). This
         * substantially complicates things, we have to register
         * AddressTable objects with TableFactory and then replace
         * them with corresponding run time objects. This is unique 
         * feature of the compiler for PF.
         */
        class swapAddressTableObjectsInRE : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            swapAddressTableObjectsInRE(const std::string &name,
                      const std::string &t) : PolicyRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };


        class swapAddressTableObjectsInSrc : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInSrc(const std::string &n) :
                swapAddressTableObjectsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class swapAddressTableObjectsInDst : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInDst(const std::string &n) :
                swapAddressTableObjectsInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };
            
        /**
         * Split rule if MultiAddress object is used in RE to make
         * sure it is single object. Also check for the case where
         * MultiAddress object is used in combination with negation,
         * this case is not supported. NOTE: this restriction can be
         * removed if PF adds support for recursively defined tables
         * (tables as elements inside tables).
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
         * This is to work around a "feature" specific to PF: If NAT
         * policy defines a redirect rule (a rule which sends packets
         * to the firewall itself, possibly changing port numbers),
         * then the packet appears on the same _ingress_ interface
         * twice. The first time it is inspected, it has an original
         * destination address, but the second time it has destination
         * address of 127.0.0.1. This address appears there because
         * our NAT compiler uses it for redirection rules. Our normal
         * ExpandMultipleAddresses processor replaces firewall object
         * with a set of addresses of all its interfaces, but skips
         * loopback interface. Rule processor addLoopbackForRedirect
         * consults with NATCompiler_pf to find out whether we have
         * any Redirect rules to accomodate for. In case we do, and
         * destination service in the current policy rule matches TSrv
         * in the redirect rule and destination contains the same
         * object that was in TDst in the NAT rule, it adds a new
         * policy rule with the same source, destination being a new
         * object used in TDst by the NAT compiler and the same
         * service.
         *
         * Caveat: as everywhere in compiler for PF, we assume rule
         * elements may contain multiple objects.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(addLoopbackForRedirect);
        friend class PolicyCompiler_pf::addLoopbackForRedirect;


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
	 *  we can not put interface name in the table, so we need to
	 *  split the rule if src or dst contains both interface and
	 *  host or network objects.
	 */
        class splitIfInterfaceInRE : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            splitIfInterfaceInRE(const std::string &name,
                                 const std::string &t) : PolicyRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };

	/**
	 *  we can not put interface name in the table, so we need to
	 *  split the rule if src contains both interface and host or
	 *  network objects.
	 */
        class splitIfInterfaceInSrc : public splitIfInterfaceInRE
        {
            public:
            splitIfInterfaceInSrc(const std::string &n) :
                splitIfInterfaceInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

	/**
	 *  we can not put interface name in the table, so we need to
	 *  split the rule if dst contains both interface and host or
	 *  network objects.
	 */
        class splitIfInterfaceInDst : public splitIfInterfaceInRE
        {
            public:
            splitIfInterfaceInDst(const std::string &n) :
                splitIfInterfaceInRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        /**
         * this processor is only called if we are using tables. It
         * creates two tables for each rule: one for source and
         * another for destination. Processor PrintRule uses these
         * tables later.
         */
        class createTables : public PolicyRuleProcessor
        {
            void createTablesForRE(libfwbuilder::RuleElement *re,
                                   libfwbuilder::Rule        *rule);
            public:
            createTables(const std::string &name) :
                PolicyRuleProcessor(name) { }
            virtual bool processNext();
        };
        friend class PolicyCompiler_pf::createTables;

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

        class printScrubRule : public PolicyRuleProcessor
        {
            protected:
            bool                             init;
            public:
            printScrubRule(const std::string &name) : PolicyRuleProcessor(name) { init=false; }
            virtual bool processNext();
        };

        /**
         * this processor accumulates all rules fed to it by previous
         * processors, then prints commands for all tables,
         * then feeds all rules to the next processor. Usually this
         * processor is in chain right before PrintRules
         */
        class PrintTables : public PolicyRuleProcessor
        {
            public:
            PrintTables(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_pf::PrintTables;


	/**
	 *  prints single policy rule, assuming all groups have been
	 *  expanded, so source, destination and service hold exactly
	 *  one object each, and this object is not a group.  Negation
	 *  should also have been taken care of before this method is
	 *  called.
	 */
        class PrintRule : public PolicyRuleProcessor
        {
            protected:

            bool                             init;
            std::string                      current_rule_label;

            virtual void _printSrcService(libfwbuilder::RuleElement  *o);
            virtual void _printDstService(libfwbuilder::RuleElement  *o);
            virtual void _printProtocol(libfwbuilder::Service *srv);

            virtual std::string _printPort(int rs,int re,bool neg=false);
            virtual std::string _printSrcService(libfwbuilder::Service *srv,bool neg=false);
            virtual std::string _printDstService(libfwbuilder::Service *srv,bool neg=false);
            virtual std::string _printTCPFlags(libfwbuilder::TCPService *srv);

            virtual void _printAddrList(libfwbuilder::FWObject  *o,bool negflag);
            virtual void _printSrcAddr(libfwbuilder::RuleElement  *o);
            virtual void _printDstAddr(libfwbuilder::RuleElement  *o);
            virtual void _printAddr(libfwbuilder::FWObject  *o, bool neg=false);

            virtual void _printNegation(libfwbuilder::RuleElement  *o);

            virtual void _printAction(libfwbuilder::PolicyRule *r);
            virtual void _printRouteOptions(libfwbuilder::PolicyRule *r);
            virtual void _printLogging(libfwbuilder::PolicyRule *r);
            virtual void _printDirection(libfwbuilder::PolicyRule *r);
            virtual void _printInterface(libfwbuilder::PolicyRule *r);
            virtual void _printAF(libfwbuilder::PolicyRule *r);
            virtual void _printLabel(libfwbuilder::PolicyRule *r);
            virtual void _printQueue(libfwbuilder::PolicyRule *r);
            virtual void _printUser(libfwbuilder::PolicyRule *r);
            virtual void _printTag(libfwbuilder::PolicyRule *r);
            virtual std::string _printLogPrefix(libfwbuilder::PolicyRule *r,const std::string &prefix);

            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
        };
        friend class PolicyCompiler_pf::PrintRule;
 

	virtual std::string myPlatformName();

	public:

	PolicyCompiler_pf(libfwbuilder::FWObjectDatabase *_db,
			  libfwbuilder::Firewall *fw,
                          bool ipv6_policy,
			  fwcompiler::OSConfigurator *_oscnf,
                          const std::list<NATCompiler_pf::redirectRuleInfo> *rri,
                          TableFactory *tbf = nullptr) :
        PolicyCompiler(_db, fw, ipv6_policy, _oscnf) 
        {
            redirect_rules_info = rri;
            tables = tbf;
        }

        virtual ~PolicyCompiler_pf();
        
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

	protected:

        TableFactory           *tables;
	libfwbuilder::IPv4     *loopback_address;
        const std::list<NATCompiler_pf::redirectRuleInfo> *redirect_rules_info;


        private:
        
    };


}

#endif
