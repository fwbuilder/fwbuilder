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

#ifndef __POLICYCOMPILER_IPF_HH
#define __POLICYCOMPILER_IPF_HH

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


    class PolicyCompiler_ipf : public PolicyCompiler_pf {


	protected:

	libfwbuilder::TCPService   *anytcp;
	libfwbuilder::UDPService   *anyudp;
	libfwbuilder::ICMPService  *anyicmp;


	virtual std::string myPlatformName();

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls PolicyCompiler::_internalPrintPolicyRule and then adds
         * chain and target at the end of the printed line
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);
        
	/**
	 * split rules if direction is "Both"
	 */
        DECLARE_POLICY_RULE_PROCESSOR(SplitDirectionIpfilter);

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
         * Placeholders for MultiAddressRunTime objects which are not
         * supported for ipf
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
            specialCaseWithDynInterface(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * ipf supports "keep state" only for icmp/udp/tcp
	 */
        DECLARE_POLICY_RULE_PROCESSOR(checkForKeepState);

	/**
	 * calculates N for action skip (used in negation)
	 */
        class calculateSkip : public PolicyRuleProcessor
        {
            std::map<std::string,int>   allrules;
            public:
            calculateSkip(const std::string &n);
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
            eliminateDuplicateRules(const std::string &n) : PolicyRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class fwcompiler::PolicyCompiler_ipf::eliminateDuplicateRules;

	/**
	 * optimize rules - instead of generating all possible
	 * combinations of src,dst and srv we split the rule onto
	 * three rules, checking on * rule element at a time and using
	 * 'any' in the other two. This reduces the number of
	 * generated elementary rules from N^3 to 3N (and reduces
	 * compile time about the same).
	 */
        class optimize1 : public PolicyRuleProcessor
        {
            protected:
            void  optimizeForRuleElement(libfwbuilder::PolicyRule *rule,
                                         const std::string  &re_type);
            public:
            optimize1(const std::string &name) : PolicyRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipf::optimize1;

        class optimizeSrc : public optimize1
        {
            public:
            optimizeSrc(const std::string &name) : optimize1(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipf::optimizeSrc;

        class optimizeDst : public optimize1
        {
            public:
            optimizeDst(const std::string &name) : optimize1(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipf::optimizeDst;

        class optimizeSrv : public optimize1
        {
            public:
            optimizeSrv(const std::string &name) : optimize1(name) {}
            virtual bool processNext();
        };
        friend class PolicyCompiler_ipf::optimizeSrv;



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
            virtual void _printWith(libfwbuilder::Service *srv);
            virtual void _printAction(libfwbuilder::PolicyRule *r);
            virtual void _printAddr(libfwbuilder::Address  *o,bool neg=false);
            virtual void _printDstService(libfwbuilder::RuleElement  *o);

            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
        };





	public:

	PolicyCompiler_ipf(libfwbuilder::FWObjectDatabase *_db,
			   libfwbuilder::Firewall *fw,
                           bool ipv6_policy,
			   fwcompiler::OSConfigurator *_oscnf) :
        PolicyCompiler_pf(_db, fw, ipv6_policy, _oscnf, nullptr) {}


	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
	
    };


}

#endif
