/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#ifndef __POLICYCOMPILER_NXOSACL_HH
#define __POLICYCOMPILER_NXOSACL_HH


#include "fwcompiler/PolicyCompiler.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"

#include "Helper.h"
#include "ACL.h"
#include "PolicyCompiler_cisco.h"

#include <functional>

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

    class PolicyCompiler_nxosacl : public PolicyCompiler_cisco {

        protected:

        std::string comment_symbol;
        
        /**
         * dynamic interfaces can not be used in policy rules in NXOS ACLs
         */
        friend class checkForDynamicInterface;
        class checkForDynamicInterface : public PolicyRuleProcessor
        {
            bool findDynamicInterface(libfwbuilder::PolicyRule *rule,
                                      libfwbuilder::RuleElement *re);
            public:
            checkForDynamicInterface(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

        /*
         *************************************************************************
         *
         * the following rule processors are intended for NXOSACL < 7.0
         * the code is in the module PolicyCompiler_nxosacl_v6_acls.cpp
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

	/**
	 * split rules with direction "both".
	 * TODO: This is used in OpenBSD pf. Move to class PolicyCompiler
	 */
	DECLARE_POLICY_RULE_PROCESSOR( SplitDirection_v6 );

	/**
	 * in NXOSACL, ACLs are always applied on interface and direction
	 * can only be "inbound". We emulate outbound ACLs though.
	 */
	DECLARE_POLICY_RULE_PROCESSOR( EmulateOutboundACL_v6 );

	/**
	 * determine acl rules should belong to
	 */
	DECLARE_POLICY_RULE_PROCESSOR( pickACL_v6   );
        friend class PolicyCompiler_nxosacl::pickACL_v6;

        /*
         *************************************************************************
         *
         * end of module PolicyCompiler_nxosacl_v6_acls.cpp
         * 
         *************************************************************************
         */

        /*
         *************************************************************************
         *
         * rule processors intended to manage ACLs for NXOSACL < 7.0 are inherited
         * from PolicyCompiler_cisco.
         * The code is in the module PolicyCompiler_cisco_acls.cpp
         *
         * The processors assume that all objects in src and dst
         * belong to the same network zone (respectively)
         *
         * All these rule processors assume outbound ACLs are supported.
         * Check corresponding capability flag and do not include these
         * processors in the processors chain in nxosacl.cpp if outbound acls
         * are not supported.
         *
         *************************************************************************
         */

	/**
	 * this processor checks for the services which require
	 * special treatment.  Some of these will be checking for
	 * source or destination object as well because special
	 * command may need to be generated in case source or
	 * destination is a firewall itself. Therefore this processor
	 * should be called after converting to atomic rules, but
	 * before interface addresses in source and destination are
	 * expanded.
	 */
	DECLARE_POLICY_RULE_PROCESSOR( SpecialServices );
        friend class PolicyCompiler_nxosacl::SpecialServices;

	/**
	 * to implement action "Reject" add command "service resetinbound"
	 */
	DECLARE_POLICY_RULE_PROCESSOR( RejectAction );
        friend class PolicyCompiler_nxosacl::RejectAction;

	/**
	 * Implements "mirrored" rules
	 */
        class mirrorRule : public PolicyRuleProcessor
        {
            void duplicateRuleElement(libfwbuilder::RuleElement *re1,
                                      libfwbuilder::RuleElement *re2);
            public:
            mirrorRule(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_nxosacl::mirrorRule;

	/**
	 * this processor accumulates all rules fed to it by previous
	 * * processors, prints commands to clear access-lists, then
	 * feeds all rules to the next processor. Usually this
	 * processor is in chain right before PrintRules.
         *
         * We use this processor to print "clear" commands because
         * they need to be generated when all access lists have been
         * created but before they are printed.
	 */
        class ClearACLs : public PolicyRuleProcessor
        {
            public:
            ClearACLs(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_nxosacl::ClearACLs;

        /**
         * "object-group service" does not seem to support matching of
         * tcp flags and "established". Need to separate objects using
         * these into separate rules to avoid object-group
         */
        DECLARE_POLICY_RULE_PROCESSOR(splitTCPServiceWithFlags);
        friend class PolicyCompiler_nxosacl::splitTCPServiceWithFlags;
        
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
            int                                 aclLineCounter;

            std::string _printPortRangeOp(int rs, int re);
            
            std::string getTcpFlagName(const libfwbuilder::TCPService::TCPFlag f);
            std::string _printSrcService(libfwbuilder::Service *srv);
            std::string _printDstService(libfwbuilder::Service *srv);
            std::string _printAddr(libfwbuilder::Address  *o);
            std::string _printProtocol(libfwbuilder::Service *srv);
            std::string _printTCPFlags(libfwbuilder::TCPService *srv);
            std::string _printAction(libfwbuilder::PolicyRule *r);
            std::string _printACL(libfwbuilder::PolicyRule *r);
            std::string _printLog(libfwbuilder::PolicyRule *r);
            std::string _printIPServiceOptions(libfwbuilder::PolicyRule *r);

            std::string _printRule(libfwbuilder::PolicyRule *rule);

            public:
            PrintRule(const std::string &name) : PolicyRuleProcessor(name) { aclLineCounter=0; }
            virtual bool processNext();
        };
        friend class PolicyCompiler_nxosacl::PrintRule;

	/**
	 * this processor accumulates all rules fed to it by previous
	 * * processors, prints commands to clear access-lists, then
	 * generates commands for the new ACLs.
         *
	 */
        class PrintCompleteACLs : public PrintRule
        {
            public:
            PrintCompleteACLs(const std::string &n) : PrintRule(n) {}
            virtual bool processNext();

            struct printRulesForACL : public std::unary_function<libfwbuilder::Rule*, void>
            {
                ciscoACL *acl;
                std::stringstream *output;
                PolicyCompiler_nxosacl *nxosacl_comp;
                PolicyCompiler_nxosacl::PrintCompleteACLs *print_acl_p;

                printRulesForACL(PolicyCompiler_nxosacl *_comp,
                                 PolicyCompiler_nxosacl::PrintCompleteACLs *pp,
                                 ciscoACL* _acl,
                                 std::stringstream *_out)
                { nxosacl_comp = _comp; print_acl_p = pp; acl = _acl; output = _out; }

                // print rule if it belongs to ACL <acl>
                void operator() (libfwbuilder::Rule* x);
            };
            friend struct PrintCompleteACLs::printRulesForACL;
        };
        friend class PolicyCompiler_nxosacl::PrintCompleteACLs;;


	bool resetinbound;
	bool fragguard;

	protected:

	virtual std::string myPlatformName();
        virtual std::string printAccessGroupCmd(ciscoACL *acl, bool neg=false);

	public:

	PolicyCompiler_nxosacl(libfwbuilder::FWObjectDatabase *_db,
                              libfwbuilder::Firewall *fw,
                              bool ipv6_policy,
                              fwcompiler::OSConfigurator *_oscnf);
        virtual ~PolicyCompiler_nxosacl() {}

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        virtual std::string printClearCommands();
        
        static std::string getAccessGroupCommandForAddressFamily(bool ipv6);

    };


}

#endif
