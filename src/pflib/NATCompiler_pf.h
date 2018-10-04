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

#ifndef __NATCOMPILER_PF_HH
#define __NATCOMPILER_PF_HH

#include "fwcompiler/NATCompiler.h"

#include "TableFactory.h"


#include <map>
#include <QMap>
#include <QSet>
#include <QString>


namespace libfwbuilder
{
    class Host;
    class IPv4;
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

    class NATCompiler_pf : public NATCompiler
    {
        public:

        struct redirectRuleInfo
        {
            std::string natrule_label;
            int old_tdst;
            int new_tdst;
            int tsrv;
            redirectRuleInfo(const std::string &rl,
                             libfwbuilder::FWObject *oa,
                             libfwbuilder::FWObject *na,
                             libfwbuilder::Service *s) 
            {
                natrule_label = rl;
                old_tdst = oa->getId();
                new_tdst = na->getId();
                tsrv = s->getId();
            }
        };


	protected:

	libfwbuilder::FWObject *loopback_address;
        TableFactory   *tables;

        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

	virtual void _expand_addr(libfwbuilder::Rule *rule,
                                  libfwbuilder::FWObject *s,
                                  bool expand_cluster_interfaces_fully);

        /**
         * analyse given address and decide which interface this NAT
         * rule should be tied to. If interface is found, use
         * rule->setInterfaceId() to save its ID and return
         * true. Otherwise, return false. Most importantly, this
         * function checks if @obj is cluster interface and then
         * uses corresponding real interface instead of it.
         */
        bool assignInterfaceToNATRule(libfwbuilder::NATRule *rule,
                                      libfwbuilder::Address *obj);
        
	/**
	 *  determines type of the NAT rule
	 */
        DECLARE_NAT_RULE_PROCESSOR(NATRuleType);


	/**
	 * this processor spits SDNAT rule onto SNAT and DNAT rules.
         * SDNAT rule translates both source and destination.
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitSDNATRule);

	/**
	 *  verifies correctness of the NAT rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules);

	/**
	 *  splits rule with multiple service objects in OSrv * onto
	 *  several rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitOnOSrv);

	/**
	 *  fills translated service with the copy of original srv
	 */
        DECLARE_NAT_RULE_PROCESSOR(fillTranslatedSrv);

	/**
	 *  split rule if addresses in TSrc are from the networks
	 *  different interfaces of the firewall belong to.
	 */
        DECLARE_NAT_RULE_PROCESSOR(splitForTSrc);

	/**
	 *  assigns NAT rules to interfaces 
	 */
        friend class AssignInterface;
        class AssignInterface : public NATRuleProcessor
        {
            std::string  regular_interfaces;
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
	 *  replaces references to the firewall in odst with
	 *  references to its external interfaces
	 */
        DECLARE_NAT_RULE_PROCESSOR(ReplaceFirewallObjectsODst);

	/**
	 *  replaces references to the firewall in tsrc with
	 *  references to its external interfaces
	 */
        DECLARE_NAT_RULE_PROCESSOR(ReplaceFirewallObjectsTSrc);

	/**
	 *  replaces object in tdst with reference to firewall's
	 *  loopback interface address object
	 */
        DECLARE_NAT_RULE_PROCESSOR(ReplaceObjectsTDst);
        friend class fwcompiler::NATCompiler_pf::ReplaceObjectsTDst;

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

        class checkForDynamicInterfacesOfOtherObjects : public NATRuleProcessor
        {
            void findDynamicInterfaces(libfwbuilder::RuleElement *re,
                                       libfwbuilder::Rule        *rule);
            public:
            checkForDynamicInterfacesOfOtherObjects(const std::string &name) :
              NATRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class checkForDynamicInterfacesOfOtherObjects;

        /**
         * like standard processor swapMultiAddressObjectsInRE, but
         * swaps compile-time address tables. See comment for this
         * rule processor in PolicyCompiler_pf
         */
        class swapAddressTableObjectsInRE : public NATRuleProcessor
        {
            std::string re_type;
            public:
            swapAddressTableObjectsInRE(const std::string &name,
                      const std::string &t) : NATRuleProcessor(name)
            { re_type=t; }
            virtual bool processNext();
        };
        friend class swapAddressTableObjectsInRE;

        class swapAddressTableObjectsInOSrc : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInOSrc(const std::string &n) :
                swapAddressTableObjectsInRE(n,
                                  libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };
        friend class swapAddressTableObjectsInOSrc;

        class swapAddressTableObjectsInODst : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInODst(const std::string &n) :
                swapAddressTableObjectsInRE(n,
                                  libfwbuilder::RuleElementODst::TYPENAME) {}
        };
        friend class swapAddressTableObjectsInODst;
            
        class swapAddressTableObjectsInTSrc : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInTSrc(const std::string &n) :
                swapAddressTableObjectsInRE(n,
                                  libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };
            
        class swapAddressTableObjectsInTDst : public swapAddressTableObjectsInRE
        {
            public:
            swapAddressTableObjectsInTDst(const std::string &n) :
                swapAddressTableObjectsInRE(n,
                                  libfwbuilder::RuleElementTDst::TYPENAME) {}
        };
            
        /**
         * Split rule if MultiAddress object is used in RE to make
         * sure it is single object. Also check for the case where
         * MultiAddress object is used in combination with negation,
         * this case is not supported. NOTE: this restriction can be
         * removed if PF adds support for recursively defined tables
         * (tables as elements inside tables).
         */
        class processMultiAddressObjectsInRE : public NATRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : NATRuleProcessor(name)
            { re_type=t; }
            virtual bool processNext();
        };


        class processMultiAddressObjectsInOSrc :
                  public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInOSrc(const std::string &n) :
                processMultiAddressObjectsInRE(n,
                                 libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInODst :
                  public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInODst(const std::string &n) :
                processMultiAddressObjectsInRE(n,
                                 libfwbuilder::RuleElementODst::TYPENAME) {}
        };

        class processMultiAddressObjectsInTSrc :
                  public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInTSrc(const std::string &n) :
                processMultiAddressObjectsInRE(n,
                                 libfwbuilder::RuleElementTSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInTDst :
                  public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInTDst(const std::string &n) :
                processMultiAddressObjectsInRE(n,
                                 libfwbuilder::RuleElementTDst::TYPENAME) {}
        };
                  
        /**
         * this processor is only called if we are using tables. It
         * creates two tables for each rule element Processor
         * PrintRule uses these tables later.
         */
        class createTables : public NATRuleProcessor
        {
            void createTablesForRE(libfwbuilder::RuleElement *re,
                                   libfwbuilder::Rule        *rule);
            public:
            createTables(const std::string &name) : NATRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class NATCompiler_pf::createTables;

        /**
         * this processor accumulates all rules fed to it by previous
         * processors, then prints commands for all tables,
         * then feeds all rules to the next processor. Usually this
         * processor is in chain right before PrintRules
         */
        class PrintTables : public NATRuleProcessor
        {
            public:
            PrintTables(const std::string &n) : NATRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class NATCompiler_pf::PrintTables;


	/**
	 *  prints single policy rule, assuming all groups have been
	 *  expanded, so source, destination and service hold exactly
	 *  one object each, and this object is not a group.  Negation
	 *  should also have been taken care of before this method is
	 *  called.
	 */
        class PrintRule : public NATRuleProcessor
        {
            void _printAnchorRule(const std::string &anchor_command,
                                  const std::string &ruleset_name,
                                  libfwbuilder::NATRule *rule);
                
    protected:
            bool init;
            std::string current_rule_label;

            virtual void _printProtocol(libfwbuilder::Service *srv);
            virtual void _printPort(libfwbuilder::Service *srv, bool lhs);
            virtual void _printSrcPort(libfwbuilder::Service *srv, bool lhs);

            virtual void _printAddrList(libfwbuilder::FWObject  *o,bool negflag);
            virtual void _printREAddr(libfwbuilder::RuleElement *o);
            virtual void _printAddr(libfwbuilder::FWObject *o);
            virtual void _printNATRuleOptions(libfwbuilder::Rule *rule);
            virtual void _printInterface(libfwbuilder::NATRule *r);

            virtual void _printNegation(libfwbuilder::RuleElement *o);

            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
        };


	virtual std::string myPlatformName();

        std::list<redirectRuleInfo> redirect_rules;

	public:

	NATCompiler_pf(libfwbuilder::FWObjectDatabase *_db,
		       libfwbuilder::Firewall *fw,
                       bool ipv6_policy,
		       fwcompiler::OSConfigurator *_oscnf,
                       TableFactory *tbf = nullptr
        ) : NATCompiler(_db, fw, ipv6_policy, _oscnf)
        {
            tables = tbf;
        }

        virtual ~NATCompiler_pf();

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
	
//	virtual string atomicRuleToString(libfwbuilder::Rule *r);

        const std::list<redirectRuleInfo>& getRedirRulesInfo() { return redirect_rules; }

    };


}

#endif
