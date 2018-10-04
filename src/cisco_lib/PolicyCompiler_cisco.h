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

#ifndef __POLICYCOMPILER_CISCO_HH
#define __POLICYCOMPILER_CISCO_HH


#include "fwcompiler/PolicyCompiler.h"
#include "fwbuilder/RuleElement.h"

#include "Helper.h"
#include "ACL.h"
#include "BaseObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "splitByNetworkZonesForRE.h"


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

    class PolicyCompiler_cisco : public PolicyCompiler {

protected:

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls PolicyCompiler::_internalPrintPolicyRule and then adds
         * fields specific to PIX rules at the end of the printout
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);


        virtual ciscoACL* createACLObject(const std::string &n,
                                          libfwbuilder::Interface *intf,
                                          const std::string &d="in",
                                          bool _ip_list=false);

        /* see #1690. After recent changes (aug 2010) in how we deal with
         * network zones, we do not assume that interfaces have network
         * zone "any" if they don't have network zone configured at all.
         * To work around this, will set network zone to "any" on all
         * interfaces. Note that this needs to be done only for IOS ACL and
         * Procurve ACL but not PIX where network zone must be configured
         * by the user.
         */
        virtual void setAllNetworkZonesToAny();

        /*
         * complementary operation: sets all interface's network zones
         * to blank to make sure compiler operates with predictable
         * configuration. This can be important if user switches from
         * platform that requires network zones (PIX) to the one that
         * does not support them, but compiler code uses the same
         * classes.
         */
        virtual void setAllNetworkZonesToNone();
       
	/**
	 * drops dynamic interface from the rule in the following
	 * cases because its address is unknown and we can not build
	 * ACL rule for it.
         *
         *  assuming interface 'INT' is dynamic
         *
         *   src.      rule bound to interface     direction     decision
         * -----------------------------------------------------------------
         *   INT       INT                        outbound      keep
         *   INT       any other                  outbound      remove
         *   INT       INT                        inbound       remove
         *   INT       any other                  inbound       remove
         *            
         *   dest.     rule bound to interface     direction     decision
         * -------------------------------------------------------------------
         *   INT       INT                        inbound       keep
         *   INT       any other                  inbound       remove
         *   INT       INT                        outbound      remove
         *   INT       any other                  outbound      remove
         *
         *
	 */
        friend class specialCaseWithDynInterface;
        class specialCaseWithDynInterface : public PolicyRuleProcessor
        {
            bool dropDynamicInterface(libfwbuilder::PolicyRule *rule,
                                      libfwbuilder::PolicyRule::Direction dir,
                                      libfwbuilder::RuleElement *re);
            public:
            specialCaseWithDynInterface(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
         * only for rules with interface element 'all' and direction
         * 'both' or 'inbound'; for interfaces with zones that match
         * src: create rule with that interface, direction
         * inbound. 'Any' in src matches all interfaces.
         *
         * Set flag (boolean) 'interface_and_direction_set_from_src'
         * so other rule processors in the same batch can skip the
         * rule.
	 */
	DECLARE_POLICY_RULE_PROCESSOR( setInterfaceAndDirectionBySrc );

	/**
         * only for rules with interface element 'all'and direction
         * 'both' or 'outbound'; for interfaces with zones that match
         * dst: create rule with that interface, direction
         * outbound. 'Any' in dst matches all interfaces.
         *
         * Set flag (boolean) 'interface_and_direction_set_from_dst'
         * so other rule processors in the same batch can skip the
         * rule.
	 */
	DECLARE_POLICY_RULE_PROCESSOR( setInterfaceAndDirectionByDst );

        /**
         * for rules with interface element not 'all' and direction
         * 'both': create two rules with the same interface and
         * directions Inbound and Outbound
         *
         * for rules with interface element not 'all' and direction
         * 'inbound' or 'outbound': setInterfaceId to this interface
         *
         * Skip rule if flag 'interface_and_direction_set_from_src' or
         * 'interface_and_direction_set_from_dst' is set
         *
         * Set flag (boolean) 'interface_and_direction_set'
         * so other rule processors in the same batch can skip the
         * rule.
         */
	DECLARE_POLICY_RULE_PROCESSOR( setInterfaceAndDirectionIfInterfaceSet );

	/**
	 * determine acl rules should belong to
	 */
        class pickACL : public PolicyRuleProcessor
        {
            bool using_named_acl;
            public:
            pickACL(bool use_named_acl,const std::string &name) :
            PolicyRuleProcessor(name) {using_named_acl = use_named_acl;}
            virtual bool processNext();
        };
        friend class PolicyCompiler_cisco::pickACL;


        
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
	 * the second copy appropriately. It preserve original src and
	 * dst in both copies, it only changes chain and direction in
	 * the second copy.
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfSrcAny);

	/**
	 * split rule if Dst==any. See comment in splitIfSrcAny
	 */
        DECLARE_POLICY_RULE_PROCESSOR(splitIfDstAny);

        /**
         * TODO: move this processor to class PolicyCompiler. The same
         * processor is used in ipt and in pf (although in pf there it
         * is present in two copies that have different names
         * splitIfFirewallInSrc and splitIfFirewallInDst). Move also
         * splitIfSrcMatchesFw and splitIfDstMatchesFw
         */
        friend class splitIfRuleElementMatchesFW;
        class splitIfRuleElementMatchesFW : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            splitIfRuleElementMatchesFW(const std::string &n,
                                                  std::string _type) :
                PolicyRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

        friend class splitIfSrcMatchesFw;
        class splitIfSrcMatchesFw : public splitIfRuleElementMatchesFW
        {
            public:
            splitIfSrcMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class splitIfDstMatchesFw : public splitIfRuleElementMatchesFW
        {
            public:
            splitIfDstMatchesFw (const std::string &n) :
                splitIfRuleElementMatchesFW(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };
        friend class PolicyCompiler_cisco::splitIfDstMatchesFw;


        /**
         * find redundant objects in rule element and eliminate
         * them. This only works for SRC and DST since all objects are
         * assumed to be addresses. Redundant object is such that has
         * narrower address range than some other object in the same
         * rule element.
         */
        friend class removeRedundantAddresses;
        class removeRedundantAddresses : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            removeRedundantAddresses(const std::string &n,std::string _type) :
                PolicyRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

        friend class removeRedundantAddressesFromSrc;
        class removeRedundantAddressesFromSrc : public removeRedundantAddresses
        {
            public:
            removeRedundantAddressesFromSrc (const std::string &n) :
                removeRedundantAddresses(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        friend class removeRedundantAddressesFromDst;
        class removeRedundantAddressesFromDst : public removeRedundantAddresses
        {
            public:
            removeRedundantAddressesFromDst (const std::string &n) :
                removeRedundantAddresses(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };



       /**
         * this processor splits rules if it finds rule that controls
         * access for tcp service with port number "port" to the firewall
         */
        class tcpServiceToFW : public PolicyRuleProcessor
        {
            int port;
            public:
            tcpServiceToFW(int p,const std::string &name) :
                PolicyRuleProcessor(name) {port=p;}
            virtual bool processNext();
        };
        friend  class PolicyCompiler_cisco::tcpServiceToFW;

        /**
         * this processor splits rules if it finds telnet to firewall
         */
        class telnetToFirewall : public tcpServiceToFW
        {
            public:
            telnetToFirewall(const std::string &n):tcpServiceToFW(23, n) {}
        };
        friend  class telnetToFirewall;

        /**
         * this processor splits rules if it finds ssh to firewall
         */
        class sshToFirewall : public tcpServiceToFW
        {
            public:
            sshToFirewall(const std::string &n):tcpServiceToFW(22, n) {}
        };
        friend  class sshToFirewall;

        /**
         * this processor splits rules if it finds telnet to firewall
         */
        class httpToFirewall : public tcpServiceToFW
        {
            public:
            httpToFirewall(const std::string &n):tcpServiceToFW(80, n) {}
        };
        friend  class httpToFirewall;

        /**
         * replace fw with one of its interfaces in SRC in interface
         * policy rule
         */
        DECLARE_POLICY_RULE_PROCESSOR( replaceFWinSRCInterfacePolicy );

        /**
         * replace fw with one of its interfaces in DST in interface
         * policy rule
         */
        DECLARE_POLICY_RULE_PROCESSOR( replaceFWinDSTInterfacePolicy );

        /**
         * replace fw with one of its interfaces in DST in global
         * policy rule
         */
        DECLARE_POLICY_RULE_PROCESSOR( replaceFWinDSTPolicy );


        class splitByNetworkZonesForSrc : public splitByNetworkZonesForRE
        {
            public:
            splitByNetworkZonesForSrc(const std::string &n):
                splitByNetworkZonesForRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class splitByNetworkZonesForDst : public splitByNetworkZonesForRE
        {
            public:
            splitByNetworkZonesForDst(const std::string &n):
                splitByNetworkZonesForRE(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };


	/**
	 * this processor deals with negation in all fields by
	 * splitting the rule and using temporary action
	 * "CONTINUE". Rules must be filtered through NegationPhase2
	 * later
	 */
	DECLARE_POLICY_RULE_PROCESSOR(  NegationPhase1 );

        /**
         *  eliminates duplicate objects in rule element
         *  're_type'. Uses special comparison function class to
         *  detect equivalent ICMP objects
         */
        class equalObjCISCO : public PolicyCompiler::equalObj
        {
            public:
            virtual bool operator()(libfwbuilder::FWObject *o);
        };

        class eliminateDuplicatesInRE_cisco : public PolicyCompiler::eliminateDuplicatesInRE
        {
            public:
            eliminateDuplicatesInRE_cisco(const std::string &n,const std::string &re_type) :
                eliminateDuplicatesInRE(n,re_type) { comparator=new equalObjCISCO(); }
        };

        /**
         *  eliminates duplicate objects in SRC.
         */
        class eliminateDuplicatesInSRC : public eliminateDuplicatesInRE_cisco
        {
            public:
            eliminateDuplicatesInSRC(const std::string &n) :
                eliminateDuplicatesInRE_cisco(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in DST.
         */
        class eliminateDuplicatesInDST : public eliminateDuplicatesInRE_cisco
        {
            public:
            eliminateDuplicatesInDST(const std::string &n) :
                eliminateDuplicatesInRE_cisco(n,libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        /**
         *  eliminates duplicate objects in SRV
         */
        class eliminateDuplicatesInSRV : public eliminateDuplicatesInRE_cisco
        {
            public:
            eliminateDuplicatesInSRV(const std::string &n) :
                eliminateDuplicatesInRE_cisco(n,libfwbuilder::RuleElementSrv::TYPENAME) {}
        };

        /**
         * Placeholders for MultiAddressRunTime objects which are not
         * supported for Cisco devices (IOS and PIX, at least)
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

#ifdef OLD_STYLE_OBJECT_GROUP_SUPPORT
	/**
	 * this processor creates PIX-specific object groups
         * (PIX CLI command "object-group") for rules with
         * more than one object in src or dst or srv
	 */
        class CreateObjectGroups : public PolicyRuleProcessor
        {
            std::string re_type;
            std::string name_suffix;

            public:
            CreateObjectGroups(const std::string &name,
                               const std::string &_ns,
                               const std::string &_type) :
                PolicyRuleProcessor(name) {re_type=_type; name_suffix=_ns; }
            virtual bool processNext();
        };
        friend class PolicyCompiler_cisco::CreateObjectGroups;

        class CreateObjectGroupsForSrc : public CreateObjectGroups
        {
            public:
            CreateObjectGroupsForSrc(const std::string &n):
                CreateObjectGroups(n,"src",libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class CreateObjectGroupsForDst : public CreateObjectGroups
        {
            public:
            CreateObjectGroupsForDst(const std::string &n):
                CreateObjectGroups(n,"dst",libfwbuilder::RuleElementDst::TYPENAME) {}
        };

        class CreateObjectGroupsForSrv : public CreateObjectGroups
        {
            public:
            CreateObjectGroupsForSrv(const std::string &n):
                CreateObjectGroups(n,"srv",libfwbuilder::RuleElementSrv::TYPENAME) {}
        };

	/**
	 * this processor accumulates all rules fed to it by previous
	 * processors, then prints all object groups and feeds all
	 * rules to the next processor. Usually this processor is in
	 * chain right before PrintRules.
         *
	 */
        class printObjectGroups : public PolicyRuleProcessor
        {
            public:
            printObjectGroups(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_cisco::printObjectGroups;

#endif
        
        
protected:

        Helper helper;
        NamedObjectsManager *named_objects_manager;

	virtual std::string myPlatformName();

        std::string mangleInterfaceName(const std::string &interface_name);

public:
	std::map<std::string,ciscoACL*> acls;

	PolicyCompiler_cisco(libfwbuilder::FWObjectDatabase *_db,
                             libfwbuilder::Firewall *fw,
                             bool ipv6_policy,
                             fwcompiler::OSConfigurator *_oscnf);
        virtual ~PolicyCompiler_cisco() {}

        virtual std::string createRuleLabel(const std::string &txt,
                                            libfwbuilder::Interface *iface,
                                            int rule_num);

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        virtual std::string printClearCommands();
        virtual std::string printPreambleCommands();
       
        /**
         * sort commands ('icmp', 'telnet', 'ssh') and access lists
         * in some kind of 'natural' order. Useful for both IOS and PIX
         */
        void regroup();

        void setNamedObjectsManager(NamedObjectsManager *mgr);

    };


}

#endif
