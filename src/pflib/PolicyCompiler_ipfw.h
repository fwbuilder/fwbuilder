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

#ifndef __POLICYCOMPILER_IPFW_HH
#define __POLICYCOMPILER_IPFW_HH

#include "PolicyCompiler_pf.h"


namespace libfwbuilder {
class TCPService;
class UDPService;
class ICMPService;
};


#define ANY_IP_OBJ_ID    "__any_ip_obj__"
#define ANY_ICMP_OBJ_ID  "__any_icmp_obj__"
#define ANY_TCP_OBJ_ID   "__any_tcp_obj__"
#define ANY_UDP_OBJ_ID   "__any_udp_obj__"

namespace fwcompiler {


    class PolicyCompiler_ipfw : public PolicyCompiler_pf {


	protected:

	libfwbuilder::TCPService   *anytcp;
	libfwbuilder::UDPService   *anyudp;
	libfwbuilder::ICMPService  *anyicmp;
        int ipfw_num;

	virtual std::string myPlatformName();

	virtual void _expand_addr(libfwbuilder::Rule *rule,
                                  libfwbuilder::FWObject *s,
                                  bool expand_cluster_interfaces_fully);

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls PolicyCompiler::_internalPrintPolicyRule and then adds
         * chain and target at the end of the printed line
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);
        

	/**
	 * disabled rules with action Pipe, use this processor in the
	 * run detecting rule shadowing
	 */
        DECLARE_POLICY_RULE_PROCESSOR(SpecialRuleActionsForShadowing);

	/**
	 * splits rules with service 'any' because ipf can keep state
	 * only for UDP/TCP/ICMP
	 */
        DECLARE_POLICY_RULE_PROCESSOR(expandAnyService);

        /**
         *  deals with negation in Src in policy rules.
	 * 
         *  this method is different from that in PolicyCompiler_pf
         */
        DECLARE_POLICY_RULE_PROCESSOR(doSrcNegation);

	/**
	 *  deals with negation in Dst in policy rules.
	 * 
         *  this method is different from that in PolicyCompiler_pf
	 */
        DECLARE_POLICY_RULE_PROCESSOR(doDstNegation);

	/**
	 *  deals with negation in Srv in policy rules.
	 * 
         * this method is different from that in PolicyCompiler_pf
	 */
        DECLARE_POLICY_RULE_PROCESSOR(doSrvNegation);

	/**
	 * This processor separates TCP/UDP services with port ranges
	 * (can only have one port range per group of ports in one
	 * rule). Call this processor after TCP and UDP services were
	 * separated by splitServices
	 */
        DECLARE_POLICY_RULE_PROCESSOR(separatePortRanges);

	/**
	 * This processor rearranges order of TCP/UDP services to make
	 * sure those with port ranges come first. Call this processor
	 * after TCP and UDP services were separated by splitServices
	 * and port ranges were separated by separatePortRanges
	 */
        DECLARE_POLICY_RULE_PROCESSOR(sortTCPUDPServices);

	/**
	 * checks for the following situations: 
         *
         *  1. dynamic interface is in source and direction is inbound
         *  (drop interface from src since source address is
         *  undertermined)
         *
         *  2. dynamic interface is in source, direction is outbound
         *  (drop interface from the list, this rule has been created
         *  while processing negation. TODO: this is kludge, need to
         *  find a better way to process negation if firewall is in rule
         *  element and it has dynamic interface)
         *
         *  3. dynamic interface is in destination and direction is
         *  outbound (drop interface since dest. address is undefined)
         * 
	 */
        class specialCaseWithDynInterface : public PolicyRuleProcessor
        {
            void dropDynamicInterface(libfwbuilder::RuleElement *re);
            public:
            specialCaseWithDynInterface(const std::string &name) :
            PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * ipf supports "keep state" only for icmp/udp/tcp
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkForKeepState);

	/**
	 * increments numbers for rules (ipfw numbers, that is)
         * The number itself is stored in the compiler class.
	 */
        class calculateNum : public PolicyRuleProcessor
        {
            public:
            calculateNum(const std::string &n);
            virtual bool processNext();
        };

	/**
	 *  eliminates duplicate atomic rules
	 */
        class eliminateDuplicateRules : public PolicyRuleProcessor
        {
            private:
            std::deque<libfwbuilder::PolicyRule*> rules_seen_so_far;
            public:
            eliminateDuplicateRules(const std::string &n) :
            PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class fwcompiler::PolicyCompiler_ipfw::eliminateDuplicateRules;

        /**
         * Placeholders for MultiAddressRunTime objects which are not
         * supported for ipfw
         */
        class processMultiAddressObjectsInRE : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : PolicyRuleProcessor(name)
            { re_type=t; }
            virtual bool processNext();
        };


        class processMultiAddressObjectsInSrc :
        public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInSrc(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInDst :
        public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInDst(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementDst::TYPENAME) {}
        };

	/**
	 *   prints single policy rule, assuming all groups have been
	 *   expanded, so source, destination and service hold exactly
	 *   one object each, and this object is not a group.
	 *   Negation should also have been taken care of before this
	 *   method is called.
	 */
        class PrintRule : public PolicyCompiler_pf::PrintRule
        {
            virtual std::string _printPort(int rs,int re,bool neg=false);
            virtual void _printProtocol(libfwbuilder::Service *srv);
            virtual void _printAction(libfwbuilder::PolicyRule *r);
            virtual void _printAddr(libfwbuilder::FWObject  *o,bool neg=false);
            virtual void _printDirection(libfwbuilder::PolicyRule *r);
            virtual void _printOppositeDirection(libfwbuilder::PolicyRule *r);
            virtual void _printInterface(libfwbuilder::PolicyRule *r);
            virtual void _printSrcService(libfwbuilder::RuleElement  *o);
            virtual void _printDstService(libfwbuilder::RuleElement  *o);
            virtual std::string _printSrcService(libfwbuilder::Service *srv,
                                                 bool neg=false);
            virtual std::string _printDstService(libfwbuilder::Service *srv,
                                                 bool neg=false);
            virtual std::string _printTCPFlags(libfwbuilder::TCPService *srv);

            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
        };





	public:

	PolicyCompiler_ipfw(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw,
                            bool ipv6_policy,
			   fwcompiler::OSConfigurator *_oscnf) :
        PolicyCompiler_pf(_db, fw, ipv6_policy, _oscnf, nullptr)
        {
            ipfw_num = 0;
        }

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        std::string defaultRules();

        int getIPFWNumber() { return ipfw_num; }
        void setIPFWNumber(int n) { ipfw_num = n; }
    };


}

#endif
