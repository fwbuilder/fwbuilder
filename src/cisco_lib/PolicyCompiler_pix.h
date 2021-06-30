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

#ifndef __POLICYCOMPILER_PIX_HH
#define __POLICYCOMPILER_PIX_HH


#include "fwcompiler/PolicyCompiler.h"
#include "fwbuilder/RuleElement.h"

#include "Helper.h"
#include "ACL.h"
#include "PolicyCompiler_cisco.h"
#include "specialServices.h"

namespace libfwbuilder {
    class IPService;
    class ICMPService;
    class TCPService;
    class UDPService;
    class RuleElementSrc;
    class RuleElementDst;
    class RuleElementSrv;
    class Group;
};

namespace fwcompiler {

    class NATCompiler_pix;

    class PolicyCompiler_pix : public PolicyCompiler_cisco {

        protected:

        /**
         * dynamic interfaces can be used in policy rules only in v6.3 and later
         *
         */
        friend class checkVersionAndDynamicInterface;
        class checkVersionAndDynamicInterface : public PolicyRuleProcessor
        {
            bool findDynamicInterface(libfwbuilder::PolicyRule *rule,
                                      libfwbuilder::RuleElement *re);
            public:
            checkVersionAndDynamicInterface(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

        /**
         * internal: checks if interface is a child of a cluster and calls
         * Compiler::_expand_interface() with a pointer to the master member
         * interface. If @iface is not cluster interface, just calls
         * Compiler::_expand_interface()
         */
        virtual void _expand_interface(libfwbuilder::Rule *rule,
                                       libfwbuilder::Interface *iface,
                                       std::list<libfwbuilder::FWObject*> &ol,
                                       bool expand_cluster_interfaces_fully);

        /*
         *************************************************************************
         *
         * the following rule processors are intended for PIX < 7.0
         * the code is in the module PolicyCompiler_pix_v6_acls.cpp
         *
         *************************************************************************
         */

        
	/**
         *   verifies combination of interface and direction and
         *   fills interface and direction. After this predicate it
         *   is guaranteed that both interface and direction have
         *   some value. In certain situations interface ID may be
         *   set to "nil" though (e.g. global policy rules). 
	 */
	DECLARE_POLICY_RULE_PROCESSOR( InterfaceAndDirection_v6 );

	/**
	 * if interface has not been defined (this is global policy
	 * rule), then multiply the rule for each interface and set
	 * direction to "Inbound"
	 */
	DECLARE_POLICY_RULE_PROCESSOR( assignRuleToInterface_v6 );

        class AddressRangesIfTcpServiceToFW : public PolicyCompiler::addressRanges
        {
            public:
            AddressRangesIfTcpServiceToFW(const std::string &n):
                addressRanges(n) {}
            virtual bool processNext();
        };
        friend class AddressRangesIfTcpServiceToFW;

	/**
	 * split rules with direction "both".
	 * TODO: This is used in OpenBSD pf. Move to class PolicyCompiler
	 */
	DECLARE_POLICY_RULE_PROCESSOR( SplitDirection_v6 );

	/**
	 * in PIX, ACLs are always applied on interface and direction
	 * can only be "inbound". We emulate outbound ACLs though.
	 */
	DECLARE_POLICY_RULE_PROCESSOR( EmulateOutboundACL_v6 );

	/**
	 * determine acl rules should belong to
	 */
	DECLARE_POLICY_RULE_PROCESSOR( pickACL_v6   );
        friend class PolicyCompiler_pix::pickACL_v6;

        /*
         *************************************************************************
         *
         * end of module PolicyCompiler_pix_v6_acls.cpp
         * 
         *************************************************************************
         */

        /*
         *************************************************************************
         *
         * rule processors intended to manage ACLs for PIX < 7.0 are inherited
         * from PolicyCompiler_cisco.
         * The code is in the module PolicyCompiler_cisco_acls.cpp
         *
         * The processors assume that all objects in src and dst
         * belong to the same network zone (respectively)
         *
         * All these rule processors assume outbound ACLs are supported.
         * Check corresponding capability flag and do not include these
         * processors in the processors chain in pix.cpp if outbound acls
         * are not supported.
         *
         *************************************************************************
         */

        class SpecialServicesSrv : public SpecialServices
        {
            public:
            SpecialServicesSrv(const std::string &n):
                SpecialServices(n, libfwbuilder::RuleElementSrv::TYPENAME) {}
        };
        friend class SpecialServices;
        
	/**
	 * sets boolean flag icmp_cmd to be able to generate command
	 * "icmp" instead of "access-list" later. Call this processor
	 * after SplitServices and splitIfDstMatchesFw
	 */
	DECLARE_POLICY_RULE_PROCESSOR( PrepareForICMPCmd );

	/**
	 * splits SRC if this is icmp_cmd rule
	 */
	DECLARE_POLICY_RULE_PROCESSOR( SplitSRCForICMPCmd );

	/**
	 * to implement action "Reject" add command "service resetinbound"
	 */
	DECLARE_POLICY_RULE_PROCESSOR( RejectAction );
        friend class PolicyCompiler_pix::RejectAction;


        /*
         * Rule processors that inherit this class match objects used
         * in policy rules to the nat rules and do something about
         * them.
         */
        class matchTranslatedAddresses : public PolicyRuleProcessor
        {
        protected:
            std::list<libfwbuilder::PolicyRule*> transformed_rules;
        public:
            matchTranslatedAddresses(const std::string &n):PolicyRuleProcessor(n) {}
            virtual bool processNext();
            virtual std::list<libfwbuilder::NATRule*> findMatchingNATRules(
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
            virtual void action(
                libfwbuilder::PolicyRule* policy_rule,
                libfwbuilder::NATRule* nat_rule,
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
        };

        /**
	 * this processor replaces objects in dst for which we have
	 * DNAT rule in a NAT policy. Call _after_ telnetToFirewall,
         * sshToFirewall and PrepareForICMPCmd
	 */
        class replaceTranslatedAddresses : public matchTranslatedAddresses
        {
            public:
            replaceTranslatedAddresses(const std::string &n) :
                matchTranslatedAddresses(n) {}
            virtual std::list<libfwbuilder::NATRule*> findMatchingNATRules(
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
            virtual void action(
                libfwbuilder::PolicyRule* policy_rule,
                libfwbuilder::NATRule* nat_rule,
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
        };
        friend class PolicyCompiler_pix::replaceTranslatedAddresses;

        /**
	 * this processor issues warning when translated addresses are
	 * used in policy rules. Use for PIX 8.3 and later.
	 */
        class warnWhenTranslatedAddressesAreUsed : public matchTranslatedAddresses
        {
            public:
            warnWhenTranslatedAddressesAreUsed(const std::string &n) :
                matchTranslatedAddresses(n) {}
            virtual std::list<libfwbuilder::NATRule*> findMatchingNATRules(
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
            virtual void action(
                libfwbuilder::PolicyRule* policy_rule,
                libfwbuilder::NATRule* nat_rule,
                libfwbuilder::Address *src,
                libfwbuilder::Address *dst,
                libfwbuilder::Service *srv);
        };
        friend class PolicyCompiler_pix::warnWhenTranslatedAddressesAreUsed;

        
        /**
         * can not use object-group in "icmp", "telnet" and "ssh" commands
         */
        DECLARE_POLICY_RULE_PROCESSOR( splitIfTelnetSSHICMPtoFw );

        class AvoidObjectGroup :  public PolicyRuleProcessor
        {
            public:
            AvoidObjectGroup(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_pix::AvoidObjectGroup;

        
	/**
	 *  this processor prints single policy rule, assuming all
	 *  groups have been expanded, so source, destination and
	 *  service hold exactly one object each, and this object is
	 *  not a group.  Negation should also have been taken care of
	 *  before this method is called.
	 */
        class PrintRule : public PolicyRuleProcessor
        {
            protected:
            std::string                         current_rule_label1;
            std::map<std::string,std::string>   current_rule_label2;
            std::list<std::string>              seen_icmp_commands;
            int                                 aclLineCounter;
            
            std::string _printPortRangeOp(int rs, int re);
            
            std::string _printSingleSSHTelnetCommand(int port,
                                        libfwbuilder::Address *a,
                                        const std::string &interfaceLabel);
            std::string _printSrcService(libfwbuilder::Service *srv);
            std::string _printDstService(libfwbuilder::Service *srv);
            std::string _printAddr(libfwbuilder::Address  *o);
            std::string _printAction(libfwbuilder::PolicyRule *r);
            std::string _printACL(libfwbuilder::PolicyRule *r);
            std::string _printSSHTelnetCommand(libfwbuilder::PolicyRule *r);
            std::string _printICMPCommand(libfwbuilder::PolicyRule *r);
            std::string _printLog(libfwbuilder::PolicyRule *r);
            bool suppressDuplicateICMPCommands(const std::string &cmd);

            public:
            PrintRule(const std::string &name) : PolicyRuleProcessor(name) { aclLineCounter=0; }
            virtual bool processNext();
        };
        friend class PolicyCompiler_pix::PrintRule;


	bool                               resetinbound;
	bool                               fragguard;

        NATCompiler_pix              *natcmp;

	protected:

	virtual std::string myPlatformName();
        std::string printAccessGroupCmd(ciscoACL *acl);

	public:

	PolicyCompiler_pix(libfwbuilder::FWObjectDatabase *_db,
                           libfwbuilder::Firewall *fw,
                           bool ipv6_policy,
                           fwcompiler::OSConfigurator *_oscnf,
                           NATCompiler_pix *_natcmp);
        virtual ~PolicyCompiler_pix() {}

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        virtual std::string printClearCommands();
        virtual std::string printPreambleCommands();
    };


}

#endif
