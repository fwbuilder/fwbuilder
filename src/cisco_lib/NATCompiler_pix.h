/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#ifndef _NATCOMPILER_PIX_HH
#define _NATCOMPILER_PIX_HH

#include "fwcompiler/NATCompiler.h"

#include "Helper.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "splitByNetworkZonesForRE.h"
#include "specialServices.h"

#include <map>
#include <deque>

namespace fwcompiler
{


    typedef enum { UNKNOWN,
                   INTERFACE,
                   SINGLE_ADDRESS,
                   NETWORK_ADDRESS,
                   ADDRESS_RANGE } global_pool_type;

    typedef enum { NONAT_NAT0, NONAT_STATIC } nonat_types;

    struct NATCmd
    {
        bool        ignore_nat;
        bool        ignore_nat_and_print_acl;
        bool        ignore_global;
        bool        use_nat_0_0;
        bool        outside;
        std::string rule_label;
        std::string comment;

        libfwbuilder::Address    *o_src;   // for "nat" command
        libfwbuilder::Address    *o_dst;   // for "nat" command
        libfwbuilder::Service    *o_srv;   // for acl in "nat" command for 6.3
        libfwbuilder::Address    *t_addr;  // for "global" command
        libfwbuilder::Interface  *i_iface; // inbound
        libfwbuilder::Interface  *o_iface; // ountbound
        int                       nat_id;
        std::string               nat_acl_name;
        global_pool_type          type;
    };

    struct StaticCmd
    {
        bool ignore_scmd_and_print_acl;
        std::string acl_name;
        std::string rule;
        libfwbuilder::Address  *iaddr;
        libfwbuilder::Address  *oaddr;
        libfwbuilder::Address  *osrc;
        libfwbuilder::Service  *osrv;
        libfwbuilder::Service  *tsrv;
        libfwbuilder::Interface *i_iface; // inbound
        libfwbuilder::Interface *o_iface; // outbound
        StaticCmd() { };
        bool operator==(const StaticCmd &other);
    };

    class NATCompiler_pix : public NATCompiler
    {
        public:

        Helper helper;
        NamedObjectsManager *named_objects_manager;

	int global_pool_no;
        std::map<int,NATCmd*> nat_commands;
        std::map<int,StaticCmd*> static_commands;

        struct nonat
        {
            std::string              acl_name;
            //libfwbuilder::Interface *i_iface;
            //libfwbuilder::Interface *o_iface;
            libfwbuilder::Address   *src;
            libfwbuilder::Address   *dst;
            bool                     last;
            nonat() { last=false; }
        };

//  first: rule->getId(), second: nonat object
        std::map<int, nonat> nonat_rules;
//  first: interface->getId(), second: rule->getId()
        std::map<int,int> first_nonat_rule_id;

        int final_ruleset_id;

        std::string debugPrintRule(libfwbuilder::Rule *r);

        void _expand_addr_recursive_pix(libfwbuilder::Rule *rule,
                                        libfwbuilder::FWObject *re,
                                        libfwbuilder::FWObject *s,
                                        std::list<libfwbuilder::FWObject*> &ol,
                                        bool expand_cluster_interfaces_fully);

        
        virtual void _expand_addr_recursive(libfwbuilder::Rule *rule,
                                            libfwbuilder::FWObject *s,
                                            std::list<libfwbuilder::FWObject*> &ol,
                                            bool expand_cluster_interfaces_fully);

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

/* this is a dictionary of all nat acl names and associated boolean
 * flag that indicates that corresponding 'clear' command has been
 * issued. We use this to keep track of all names that are created to
 * make sure they are unique
 */
        std::map<std::string,int> nat_acl_names;

        std::string getNATACLname(libfwbuilder::Rule *r,int nat_id);
        std::string getNATACLname(libfwbuilder::Rule *r,std::string suffix);

        /**
         *  verifies correctness of the NAT rules
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules);
        friend class NATCompiler_pix::VerifyRules;

	/**
	 * using network zone information determine inside and outside
	 * interfaces for the NAT rule
	 */
	DECLARE_NAT_RULE_PROCESSOR( AssignInterface );
        friend class NATCompiler_pix::AssignInterface;

	/**
	 *  if previous processor assigned the same interface as both
	 *  internal and external one for the NAT operation, drop the rule
	 */
	DECLARE_NAT_RULE_PROCESSOR( verifyInterfaces );
        friend class NATCompiler_pix::verifyInterfaces;

	/**
	 * in case of DNAT both odst and tdst should be of the same size:
	 * either both hosts, or both networks of the same size and can not
	 * be address ranges. There are other limitations, too.
	 */
	DECLARE_NAT_RULE_PROCESSOR( verifyRuleElements );
        friend class NATCompiler_pix::verifyRuleElements;

	/**
	 *  expands address ranges in OSrc and ODst
	 */
	DECLARE_NAT_RULE_PROCESSOR( ExpandAddressRanges );
        friend class NATCompiler_pix::ExpandAddressRanges;

        /**
         *  in case OSrv is not "any" but TSrv is "original", copy it over
         */
	DECLARE_NAT_RULE_PROCESSOR( fillTranslatedSrv );
        friend class NATCompiler_pix::fillTranslatedSrv;

        /**
         * replaces firewall objects in ODst with its external interface(s)
         */
	DECLARE_NAT_RULE_PROCESSOR( ReplaceFirewallObjectsODst );
        friend class NATCompiler_pix::ReplaceFirewallObjectsODst;

        /**
         * replaces firewall objects in TSrc with its external interface(s)
         */
	DECLARE_NAT_RULE_PROCESSOR( ReplaceFirewallObjectsTSrc );
        friend class NATCompiler_pix::ReplaceFirewallObjectsTSrc;

        /**
         * replace host object with firewall's interace if host object
         * has the same address
         */
        class UseFirewallInterfaces : public NATRuleProcessor
        {
            void scanInterfaces(libfwbuilder::RuleElement *rel);
            public:
            UseFirewallInterfaces(const std::string &name) : NATRuleProcessor(name){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::UseFirewallInterfaces;

	/**
	 * this processor creates ACL for nat 0 rules
	 */
        class processNONATRules : public NATRuleProcessor
        {
            protected:
            int nonat_no;
            public:
            processNONATRules(const std::string &name) : NATRuleProcessor(name){ nonat_no=1; }
            virtual bool processNext();
        };
        friend class NATCompiler_pix::processNONATRules;

	/**
	 * this processor creates object of class NATCmd if rule is of
	 * type SNAT
	 */
	class createNATCmd  : public NATRuleProcessor
        {
            protected:
            int nat_id_counter;
            public:
            createNATCmd(const std::string &name) : NATRuleProcessor(name){ nat_id_counter=1; }
            virtual bool processNext();
        };
        friend class NATCompiler_pix::createNATCmd;

        /**
         * Creates objects of class StaticCmd for all DNAT rules
         */
	class createStaticCmd  : public NATRuleProcessor
        {
            protected:
            int sc_id_counter;
            public:
            createStaticCmd(const std::string &name) : NATRuleProcessor(name){ sc_id_counter=1; }
            virtual bool processNext();
        };
        friend class NATCompiler_pix::createStaticCmd;

        /**
         *  this processor manipulates list of NATCmd objects so that
         *  to reuse identical "nat" and "global" rules. It also
         *  merges objects of class StaticCmd in DNAT rules to avoid *
         *  duplicate "static" commands
         */
	DECLARE_NAT_RULE_PROCESSOR( mergeNATCmd );
        friend class NATCompiler_pix::mergeNATCmd;


        /**
         * takes ID of the original object used in OSrc that has been
         * stored in storeOriginalOSrcID and compares this object with
         * network zones of interfaces. If FirewallOption
         * "pix_optimize_default_nat" is set and original OSrc is the
         * same as network zone of one of the interfaces, replaces
         * OSrc in this rule with "Any".
         */
	DECLARE_NAT_RULE_PROCESSOR( optimizeDefaultNAT );
        friend class NATCompiler_pix::optimizeDefaultNAT;

        /**
         * this rule processor uses boolean flag "clear_osrc" set by
         * optimizeDefaultNAT and removes objects in OSrc if this flag is set.
         */
	DECLARE_NAT_RULE_PROCESSOR( clearOSrc );
        friend class NATCompiler_pix::clearOSrc;

        class SpecialServicesOSrv : public SpecialServices
        {
            public:
            SpecialServicesOSrv(const std::string &n):
                SpecialServices(n, libfwbuilder::RuleElementOSrv::TYPENAME) {}
        };

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
        friend class PrintRule;
        class PrintRule : public NATRuleProcessor
        {
            protected:

            bool          init;
            std::string   current_rule_label;

            std::string _printPortRangeOp(int rs, int re);            

            std::string  _printSrcService(libfwbuilder::Service *srv);
            std::string  _printDstService(libfwbuilder::Service *srv);
            virtual void _printPort(libfwbuilder::Service *srv);
            std::string  _printAddress(libfwbuilder::Address *a,bool print_netmask);
            std::string  _printConnOptions(libfwbuilder::NATRule *rule);
            std::map<int,bool> printed_global_pools;

            public:
            
            PrintRule(const std::string &n);
            virtual bool processNext();
            virtual void printNONAT(libfwbuilder::NATRule *rule);
            virtual void printSNAT(libfwbuilder::NATRule *rule);
            virtual void printSDNAT(libfwbuilder::NATRule *rule);
            virtual void printDNAT(libfwbuilder::NATRule *rule);
            
        };
        friend class NATCompiler_pix::PrintRule;


        /**
         *  detects duplicate nat
         */
        class DetectDuplicateNAT : public NATRuleProcessor
        {
            public:
            DetectDuplicateNAT(const std::string &n) : NATRuleProcessor(n){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::DetectDuplicateNAT;

        /**
         * base class that has a method that checks for overlapping addresses
         * taking into account address ranges and other situations
         */
        friend class DetectOverlap;
        class DetectOverlap : public NATRuleProcessor
        {
            protected:
            bool checkOverlapping(const libfwbuilder::Address   &a1,
                                  const libfwbuilder::InetAddr &a2);
            std::string printGlobalPoolAddress(const libfwbuilder::Address &pool);
            public:
            DetectOverlap(const std::string &n) : NATRuleProcessor(n){}
            virtual ~DetectOverlap();
        };
        friend class NATCompiler_pix::DetectOverlap;


        /**
         *  detects overlapping and some other problems with global pools 
         */
        friend class DetectGlobalPoolProblems;
        class DetectGlobalPoolProblems : public DetectOverlap
        {
            public:
            DetectGlobalPoolProblems(const std::string &n) : DetectOverlap(n){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::DetectGlobalPoolProblems;

        /**
         *  detects overlapping global pools and static rules
         */
        friend class DetectOverlappingGlobalPoolsAndStaticRules;
        class DetectOverlappingGlobalPoolsAndStaticRules : public DetectOverlap
        {
            public:
            DetectOverlappingGlobalPoolsAndStaticRules(const std::string &n) : DetectOverlap(n){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::DetectOverlappingGlobalPoolsAndStaticRules;


        /**
         *  suppress identical nonat static (this happens in rules
         *  that request no translation in access from a low security
         *  zone to a high security zone and have group or multiple
         *  objects in OSrc).
         */
        friend class SuppressDuplicateNONATStatics;
        class SuppressDuplicateNONATStatics : public NATRuleProcessor
        {
            protected:
            typedef struct {
                std::string             iface1, iface2;
                libfwbuilder::InetAddr addr;
                libfwbuilder::InetAddr mask;
            } nonat_static_parameters;
            std::deque<nonat_static_parameters> all_nonat_statics;
            public:
            SuppressDuplicateNONATStatics(const std::string &n)  : NATRuleProcessor(n){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::SuppressDuplicateNONATStatics;



        /**
         *  detects overlapping static
         */
        friend class DetectOverlappingStatics;
        class DetectOverlappingStatics : public NATRuleProcessor
        {
            protected:
            public:
            DetectOverlappingStatics(const std::string &n)  : NATRuleProcessor(n){}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::DetectOverlappingStatics;

        /**
         *  this processor stores processed NAT rules in final_ruleset
         */
	DECLARE_NAT_RULE_PROCESSOR( storeProcessedRules );
        friend class NATCompiler_pix::storeProcessedRules;


	protected:

	virtual std::string myPlatformName();



	public:

	NATCompiler_pix(libfwbuilder::FWObjectDatabase *_db,
                        libfwbuilder::Firewall *fw,
                        bool ipv6_policy,
                        fwcompiler::OSConfigurator *_oscnf);
        virtual ~NATCompiler_pix();
        
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

        void regroup();

        virtual std::string printClearCommands();
        virtual std::string printPreambleCommands();

        /**
         * scans all rules in source_ruleset and finds rules (if
         * any) that define DNAT translation for a combination of
         * src,dst and srv where src matches OSrc, srv matches OSrv
         * and dst matches rule element defined by argument
         * nat_re_type_to_match_dst.  If such rules could be found, returns
         * a list of triplets (src,odst,osrv)
         */
        std::list<libfwbuilder::NATRule*> findMatchingDNATRules(
            libfwbuilder::Address *src,
            libfwbuilder::Address *dst,
            libfwbuilder::Service *srv,
            const std::string &nat_re_type_to_match_dst);
	
	
//	virtual string atomicRuleToString(libfwbuilder::Rule *r);

        void registerACL(const std::string& acl_name) {
            setACLFlag(acl_name,0);
        }

        int  getACLFlag(const std::string& acl_name) {
            if (nat_acl_names.count(acl_name)!=0)
                return nat_acl_names[acl_name];
            return -1;
        }

        void setACLFlag(const std::string& acl_name, int f) {
            nat_acl_names[acl_name] = f;
        }

        void setNamedObjectsManager(NamedObjectsManager *mgr);
    };


}

#endif
