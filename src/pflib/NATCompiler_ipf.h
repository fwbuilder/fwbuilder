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

#ifndef __NATCOMPILER_IPF_HH
#define __NATCOMPILER_IPF_HH

#include "NATCompiler_pf.h"

#include <map>

#define FTP_PORT    21
#define RCMD_PORT   514
#define KRCMD_PORT  544
#define EKSHELL_PORT 2106
#define H323_PORT   1720
#define RAUDIO_PORT 5050
#define ISAKMP_PORT 500
#define PPTP_PORT   1723
#define IRC_PORT    6667

namespace fwcompiler {


    class NATCompiler_ipf : public NATCompiler_pf {



	protected:

	virtual std::string myPlatformName();


	/**
	 * verifies correctness of the NAT rules (some checks are the
	 * same as in pf, some are specific for ipf)
	 */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules);

	/**
	 *  splits NAT rules if user ordered using * application proxy
	 *  code for "map" rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(appProxy);

	/**
	 * splits rules with service 'any' because they need "proxy
	 * tcp/udp auto"
	 */
        DECLARE_NAT_RULE_PROCESSOR(expandAnyService);

	/**
	 *  ipf nat and rdr rules do not support port tanges; need to
	 *  generate a separate rule for each port of the range.
	 */
        DECLARE_NAT_RULE_PROCESSOR(ExpandPortRange);

	/**
	 * LB-type rules allow no more than two destination hosts on
	 * the right side of '->'. This processor splits NAT rule if
         * necessary to satisfy this rule
	 */
        DECLARE_NAT_RULE_PROCESSOR(prepareForLB);

	/**
	 *  assigns NAT rules to interfaces 
	 */
        DECLARE_NAT_RULE_PROCESSOR(AssignInterface);

	/**
	 *  replaces object in tdst with reference to firewall's
	 *  interface in 'Redirect' rules
	 */
        DECLARE_NAT_RULE_PROCESSOR(RedirectRules);
        friend class fwcompiler::NATCompiler_ipf::RedirectRules;

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

        /**
         * Placeholder for MultiAddressRunTime objects that are not
         * supported for ipf
         */
        class processMultiAddressObjectsInRE : public NATRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : NATRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };


        class processMultiAddressObjectsInOSrc : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInOSrc(const std::string &n) :
                processMultiAddressObjectsInRE(n,libfwbuilder::RuleElementOSrc::TYPENAME) {}
        };

        class processMultiAddressObjectsInODst : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInODst(const std::string &n) :
                processMultiAddressObjectsInRE(n,libfwbuilder::RuleElementODst::TYPENAME) {}
        };


	/**
	 *  prints single policy rule, assuming all groups have been
	 *  expanded, so source, destination and service hold exactly
	 *  one object each, and this object is not a group.  Negation
	 *  should also have been taken care of before this method is
	 *  called.
	 */
        class PrintRule : public NATCompiler_pf::PrintRule
        {
            protected:
            virtual void _printProtocol(libfwbuilder::Service *srv);
            virtual void _printAddr_L(libfwbuilder::Address  *o, bool print_netmask=true);
            virtual void _printAddr_R(libfwbuilder::Address  *o, bool print_netmask=true);
            virtual void _printAddr_R_LB(libfwbuilder::RuleElementTDst *re);
            virtual void _printPort(libfwbuilder::Service *srv,bool eq);

            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
        };




	public:

	NATCompiler_ipf(libfwbuilder::FWObjectDatabase *_db,
			libfwbuilder::Firewall *fw,
                        bool ipv6_policy,
			fwcompiler::OSConfigurator *_oscnf) :
        NATCompiler_pf(_db, fw, ipv6_policy, _oscnf) {}


	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
	
    };


}

#endif
