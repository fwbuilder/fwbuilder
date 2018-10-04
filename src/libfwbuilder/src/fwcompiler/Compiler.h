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

#ifndef __COMPILER_HH__
#define __COMPILER_HH__

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"

#include "fwcompiler/BaseCompiler.h"
#include "fwcompiler/RuleProcessor.h"
#include "fwcompiler/exceptions.h"
#include "fwcompiler/GroupRegistry.h"

#include <list>
#include <vector>
#include <map>

#include <fstream>
#include <sstream>

namespace libfwbuilder {
    class FWObject;
    class FWOptions;
    class FWObjectDatabase;
    class InetAddr;
    class Address;
    class Cluster;
    class FailoverClusterGroup;
    class StateSyncClusterGroup;
    class Service;
    class Interval;
    class IPv4;
    class IPv6;
    class Network;
    class NetworkIPv6;
    class AddressRange;
    class Host;
    class physAddress;
    class Firewall;
    class Interface;
    class Rule;
    class RuleSet;
    class PolicyRule;
    class NATRule;
    class RuleElement;
};


namespace fwcompiler {

    class OSConfigurator;

/* 
 * operations    (see Compiler_ops.cc)
 */

    /**
     * this operator compares two objects to determine if they are
     * equivalent
     */
    bool operator==(const libfwbuilder::Address &o1,const libfwbuilder::Address &o2);
    bool operator==(const libfwbuilder::Service &o1,const libfwbuilder::Service &o2);
    bool operator==(const libfwbuilder::Interval &o1,const libfwbuilder::Interval &o2);

    /**
     * this method finds intersection of two objects. Objects must
     * be of such types that have address (Host, Firewall,
     * Interface, Network) , otherwise it throws an exception
     *
     * TODO:  implement this as a virtual method of respective classes
     *
     * this method is intended for internal use only
     */
    std::vector<libfwbuilder::FWObject*> 
    _find_obj_intersection(libfwbuilder::Address *o1,
			   libfwbuilder::Address *o2);
    /**
     * this method finds intersection of two services. If one or
     * both objects are not services, it throws exception
     *
     * this method is intended for internal use only
     */
    std::vector<libfwbuilder::FWObject*> 
    _find_srv_intersection(libfwbuilder::Service *o1,
			   libfwbuilder::Service *o2);

    /**
     * this method finds intersection of two ranges of ports
     *
     * this method is intended for internal use only
     */
    bool _find_portrange_intersection(int rs1,int re1,int rs2,int re2,int &rsr,int &rer);

    struct threeTuple {
        libfwbuilder::Address *src;
        libfwbuilder::Address *dst;
        libfwbuilder::Service *srv;
    };

    class Compiler : public BaseCompiler
    {
        void _init(libfwbuilder::FWObjectDatabase *_db, libfwbuilder::Firewall *fw);

        virtual void _expand_group_recursive(libfwbuilder::FWObject *o,
                                             std::list<libfwbuilder::FWObject*> &ol);

        virtual void _expand_addr_recursive(libfwbuilder::Rule *rule,
                                            libfwbuilder::FWObject *s,
                                            std::list<libfwbuilder::FWObject*> &ol,
                                            bool expand_cluster_interfaces_fully);

        /* bool _complexMatchWithInterface(libfwbuilder::Address   *obj1, */
        /*                                 libfwbuilder::Interface *iface, */
        /*                                 bool recognize_broadcasts=true); */

        /* bool _complexMatchWithAddress(const libfwbuilder::InetAddr *obj1_addr, */
        /*                               libfwbuilder::Interface *iface, */
        /*                               const std::string &address_type, */
        /*                               bool recognize_broadcasts); */

        bool checkIfAddressesMatch(const libfwbuilder::Address *a1,
                                   const libfwbuilder::Address *a2);

protected:

        int  _cntr_;
        bool initialized;
        int countIPv6Rules;
        bool ipv6;
        std::map<int, bool> object_comparison_cache;
        std::map<int, threeTuple*> rule_elements_cache;
        
        std::list<BasicRuleProcessor*> rule_processors;

        /**
         * if object <o> is Address, check if it matches address family
         * (i.e. if it is IPv6 or IPv4). If it is service, always return true.
         */
        bool MatchesAddressFamily(libfwbuilder::FWObject *o);
        
        /**
         * this method finds intersection of two atomic rules. Resulting
         * rule may have multiple objects in src,dst and srv, so
         * converting to atomic may be necessary. If rules can not be
         * compared, then it throws an exception. If rules are compatible
         * but have nothing in common, then this method returns rule with
         * empty src,dst,srv. Use isEmpty(Rule &r) to check for this
         * condition
         * 
         * This method creates and returns new object of class Rule
         * and does not modify r1 and r2
         *
         * This method works only with interface, src, dst and srv and
         * completely ignores action and other rule options.
         *
         */
        void  getIntersection(libfwbuilder::PolicyRule &r1,
                              libfwbuilder::PolicyRule &r2,
                              libfwbuilder::PolicyRule &res);

        /**
         * this function checks if two rules intersect - that is, if there
         * is a non-empty intersection for each rule element. This
         * function does not calculate intersection, it just verifies that
         * it does exsit.
         */
        bool  intersect(libfwbuilder::PolicyRule &r1,
                        libfwbuilder::PolicyRule &r2);


        /**
         * add rule processor to the list
         */
        void add(BasicRuleProcessor* rp);

        /**
         *  assembles chain of rule processors and executes it
         */
        void runRuleProcessors();

        /**
         *  deletes chain of rule processors
         */
        void deleteRuleProcessors();

       /*
        * the following variables are simply a cache for frequently used
        * objects
        */
        int                                     fw_id;
        libfwbuilder::FWOptions                *fwopt;

public:

	int  debug;
	int  debug_rule;
        bool rule_debug_on;
	bool verbose;
        bool single_rule_mode;
        std::string single_rule_ruleset_name;
        int single_rule_position;
        libfwbuilder::Rule *single_rule_compile_rule;

	fwcompiler::OSConfigurator *osconfigurator;
	libfwbuilder::FWObjectDatabase *dbcopy;
	libfwbuilder::Library *persistent_objects;
	libfwbuilder::Firewall *fw;

        // group registry is optional, the object shuld be created outside
        // of the compiler and set using function setGroupRegistry().
        GroupRegistry *group_registry;
        
        std::string ruleSetName;;
        
	libfwbuilder::RuleSet *source_ruleset;
	libfwbuilder::RuleSet *temp_ruleset;

        libfwbuilder::Group *temp;

	std::stringstream output;

        void registerGroupObject(libfwbuilder::RuleElement *re,
                                 libfwbuilder::ObjectGroup *grp);
        
        void registerIPv6Rule() { countIPv6Rules++; }
        bool haveIPv6Rules() { return countIPv6Rules > 0; }

        /**
         * returns first object referenced by given rule
         * element. Dereferences FWReference if first object is
         * reference. Uses cache, therefore is faster than
         * RuleElement::getFirst(true)
         */
        libfwbuilder::Address* getFirstSrc(libfwbuilder::PolicyRule *rule);
        libfwbuilder::Address* getFirstDst(libfwbuilder::PolicyRule *rule);
        libfwbuilder::Service* getFirstSrv(libfwbuilder::PolicyRule *rule);
        libfwbuilder::Interval* getFirstWhen(libfwbuilder::PolicyRule *rule);
        libfwbuilder::Interface* getFirstItf(libfwbuilder::PolicyRule *rule);
        
        libfwbuilder::Address* getFirstOSrc(libfwbuilder::NATRule *rule);
        libfwbuilder::Address* getFirstODst(libfwbuilder::NATRule *rule);
        libfwbuilder::Service* getFirstOSrv(libfwbuilder::NATRule *rule);
                                                         
        libfwbuilder::Address* getFirstTSrc(libfwbuilder::NATRule *rule);
        libfwbuilder::Address* getFirstTDst(libfwbuilder::NATRule *rule);
        libfwbuilder::Service* getFirstTSrv(libfwbuilder::NATRule *rule);

        /**
         * these methods compare two objects to determine if one of them
         * "shades" the other
         */
        bool checkForShadowing(const libfwbuilder::Address &o1,
                               const libfwbuilder::Address &o2);
        bool checkForShadowing(const libfwbuilder::Service &o1,
                               const libfwbuilder::Service &o2);
        void resetObjectComparisonCache() { object_comparison_cache.clear(); }

	/**
	 *   a method to check for unnumbered interface in a rule
	 *   element (one can not use unnumbered interfaces in rules).
	 */
        bool catchUnnumberedIfaceInRE(libfwbuilder::RuleElement *re);

        /**
         * return true if any address object in source or destination is
         * of given type (can be IPv4 or IPv6).
         */
        bool FindAddressFamilyInRE(libfwbuilder::FWObject *re, bool ipv6);

        /**
         * find ipv6 or ipv4 address objects in the given rule element
         * and remove reference to them
         */
        void DropAddressFamilyInRE(libfwbuilder::RuleElement *rel,
                                   bool drop_ipv6);

        /**
         * similarly, remove reference to objects in service rule elements
         * based on their compatibility with ipv6 context.
         */
        void DropByServiceTypeInRE(libfwbuilder::RuleElement *rel,
                                   bool drop_ipv6);
        
        /**
         *  rule processor that "injects" rules into the conveyor
         */
        class Begin : public BasicRuleProcessor
        {
            bool init;
            public:
            Begin();
            Begin(const std::string &n);
            virtual bool processNext();
        };

	/**
	 * this processor prints number of rules in the queue on cout
	 * if compiler->verbose is true
	 */
        class printTotalNumberOfRules : public BasicRuleProcessor
        {
            public:
            virtual bool processNext();
        };

	/**
	 * this processor creates what amounts to the new compiler
	 * pass: it slurps all rules into buffer, then prints its own
	 * name on cout and the releases rules to the next processor
	 * one at a time.  This way processors after this one in the
	 * chain get to start working on the whole rule set from its
	 * beginning.
	 */
        class createNewCompilerPass : public BasicRuleProcessor
        {
            std::string pass_name;
            public:
            createNewCompilerPass(const std::string &_name) : BasicRuleProcessor("New compiler pass") { pass_name=_name; };
            virtual bool processNext();
        };

	/**
	 * this processor prints rule numbers on cout (trivial
	 * progress indicator)
	 */
        class simplePrintProgress : public BasicRuleProcessor
        {
            std::string current_rule_label;
            public:
            simplePrintProgress() : BasicRuleProcessor("Print progress") {};
            virtual bool processNext();
        };

        /**
         * this processor splits rule if one of its rule elements
         * contains firewall itself. This processor is actually only a
         * base class. Derive it and pass rule element type name as a
         * second argument of its constructor.
         */
        class splitIfRuleElementMatchesFW : public PolicyRuleProcessor
        {
            std::string re_type;
            public:
            splitIfRuleElementMatchesFW(const std::string &n,
                                                  std::string _type) :
                PolicyRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

        /**
         * prepare for negation of single objects in rule elements
         */
        class singleObjectNegation : public BasicRuleProcessor
        {
            std::string re_type;
            public:
            singleObjectNegation(const std::string &n,std::string _type):
                BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

        /*
         * replace interfaces in the give RE with a set of all other
         * interfaces of the firewall.
         */
        class fullInterfaceNegationInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
                fullInterfaceNegationInRE(const std::string &n, std::string _type) :
                    BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };

	/**
	 * replace cluster interface objects with inetrfaces of the member
         * firewall in the Interface rule element
	 */
        class replaceClusterInterfaceInItfRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
                replaceClusterInterfaceInItfRE(const std::string &n,
                                               std::string _type) :
                BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };
            
        /**
         *  eliminates duplicates in RuleElement 're_type'. Inherit
         *  your own class using this one and supply actual rule
         *  element type name through its constructor. 
         *
         *  Function class equalObj compares IDs of two object and
         *  declares objects equal if their ID are the same.  To
         *  change comparison algorithm, inherit from this class,
         *  overload its operator(), then create its instance in
         *  constructor of eliminateDuplicatesInRE and assign to
         *  member 'comparator'
         */
        class equalObj {
            protected:
            libfwbuilder::FWObject *obj;
            public:
            equalObj(){obj=nullptr;}
            virtual ~equalObj() {}
            void set(libfwbuilder::FWObject *o) {obj=o;}
            virtual bool operator()(libfwbuilder::FWObject *o);
        };

        class eliminateDuplicatesInRE : public BasicRuleProcessor
        {
            std::string re_type;
            protected:
            equalObj *comparator;
            public:
            eliminateDuplicatesInRE(const std::string &n,const std::string _type):
                BasicRuleProcessor(n) { re_type=_type; comparator=nullptr; }
            ~eliminateDuplicatesInRE() { if (comparator!=nullptr) delete comparator; }
            virtual bool processNext();
        };

        /**
         * this processor checks for recursive groups, i.e. groups
         * that reference themselves
         */
        class recursiveGroupsInRE : public BasicRuleProcessor
        {
            std::string re_type;
            void isRecursiveGroup(int grid, libfwbuilder::FWObject *gr);
            public:
            recursiveGroupsInRE(const std::string &n,const std::string &_type) : 
                BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };


	/**
	 * This rule processor checks for empty groups. Normally this
	 * is a fatal error and compilation should be aborted, but
	 * firewall option "ignore_rules_with_empty_groups" causes
	 * compiler to remove this object from the rule element and
	 * drop the rule all together if there are no more objects
	 * left (rule element becomes "any") and continue work
	 * (warning should be issued though).
	 */
        class emptyGroupsInRE : public BasicRuleProcessor
        {
            std::string re_type;
            int  countChildren(libfwbuilder::FWObject *obj);
            void findEmptyGroupsInRE();
            public:
            emptyGroupsInRE(const std::string &n,const std::string &_type) : 
                BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };


        /**
         * Replace MultiAddress objects that require run-time address
         * expansion with corresponding MultiAddressRunTime objects
         */
        class swapMultiAddressObjectsInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
            swapMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : BasicRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };

        /**
         *  this inspector replaces references to hosts and firewalls
         *  in rule element with references to their interfaces
         */
        class expandMultipleAddressesInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
            expandMultipleAddressesInRE(const std::string &name,
                                        const std::string &t) :
                BasicRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };

        /**
         * drop rules that have ipv4 or ipv6 addresses  (depending
         * on the argument ipv6 passed to the constructor)
         */
        class DropRulesByAddressFamilyAndServiceType : public BasicRuleProcessor
        {
            std::string warning_str;
            bool drop_ipv6;
            public:
            DropRulesByAddressFamilyAndServiceType(
                const std::string &n, bool ipv6) : BasicRuleProcessor(n)
            { drop_ipv6 = ipv6; warning_str = ""; }
            virtual bool processNext();
            protected:
            DropRulesByAddressFamilyAndServiceType(
                const std::string &n,
                const std::string &w,
                bool ipv6) : BasicRuleProcessor(n)
            { drop_ipv6 = ipv6; warning_str = w; }
        };

        /**
         * Drop rule if any address object in source or destination is
         * ipv4 address.
         */
        class DropIPv4Rules : public DropRulesByAddressFamilyAndServiceType
        {
            public:
            DropIPv4Rules(const std::string &n) :
              DropRulesByAddressFamilyAndServiceType(n, false) {};
        };

        /**
         * Drop rule if any address object in source or destination is
         * ipv6 address.
         */
        class DropIPv6Rules : public DropRulesByAddressFamilyAndServiceType
        {
            public:
            DropIPv6Rules(const std::string &n) :
              DropRulesByAddressFamilyAndServiceType(n, true) {};
        };
        
        class DropIPv6RulesWithWarning : public DropRulesByAddressFamilyAndServiceType
        {
            public:
            DropIPv6RulesWithWarning(const std::string &n, const std::string &w) :
                DropRulesByAddressFamilyAndServiceType(n, w, true) {};
        };

        /**
	 * drop rules that have empty rule elements
	 */
        class dropRuleWithEmptyRE : public BasicRuleProcessor
        {
            std::string warning_str;
            bool isREEmpty(libfwbuilder::Rule *rule, const std::string &re_type);
            public:
            dropRuleWithEmptyRE(const std::string &name) :
                BasicRuleProcessor(name) { warning_str = ""; }
            virtual bool processNext();
            protected:
            dropRuleWithEmptyRE(const std::string &name,
                                const std::string &_warning) :
                BasicRuleProcessor(name) { warning_str = _warning; }
        };

        class dropRuleWithEmptyREWithWarning : public dropRuleWithEmptyRE
        {
            public:
            dropRuleWithEmptyREWithWarning(const std::string &name,
                                const std::string &_warning) :
                dropRuleWithEmptyRE(name, _warning) { }
        };

        /**
         * if MultiAddress object failed to convert itself to a group
         * of addresses and compiler runs in a test mode, we use dummy
         * test addresses instead. The error is detected by
         * preprocessor and at the moment not associated with any rule
         * (because preprocessor works with MultiAddress objects and
         * does not check which rule they are used in to improve
         * performance).  This processor adds error message to the
         * rule using MultiAddress obejct like this.
         */
        class checkForObjectsWithErrors : public BasicRuleProcessor
        {
            public:
            checkForObjectsWithErrors(const std::string &name)
                : BasicRuleProcessor(name) {}
            virtual bool processNext();
        };

        /**
         * Replace cluster interface object with corresponding real
         * interface
         */
        class replaceFailoverInterfaceInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
            replaceFailoverInterfaceInRE(const std::string &name,
                      const std::string &t) : BasicRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };

	/**
	 * generic rule debugger: prints name of the previous rule
	 * processor in a chain and then a rule if its number is
	 * compiler->debug_rule.  Uses virtual method
	 * Compiler::debugPrintRule to actually print the rule
	 */
        class Debug : public BasicRuleProcessor
        {
            public:
            virtual bool processNext();
        };

	/**
         * this rule processor skips all rules except the one with ID
         * set by call to setSingleRuleMode()
	 */
        class singleRuleFilter : public BasicRuleProcessor
        {
            public:
            virtual bool processNext();
        };

	/**
	 * split rules with more than one service object, so that each
	 * rule has services that satisfy some common criteria defined by
         * the virtual function groupingCode()
	 */
        class groupServices : public BasicRuleProcessor
        {
            protected:
            virtual int groupingCode(const libfwbuilder::Service *srv) =0;
            public:
            groupServices(const std::string &name) : BasicRuleProcessor(name) {} 
            virtual bool processNext();
        };

	/**
	 * split rules with more than one service object, so that each
	 * rule has services with the same protocol
	 */
        class groupServicesByProtocol: public groupServices
        {
            protected:
            virtual int groupingCode(const libfwbuilder::Service *srv);
            public:
            groupServicesByProtocol(const std::string &name) : groupServices(name){}
        };

	/**
	 * split rules with more than one service object, so that all
	 * tcp and udp services are in one rule and all other
	 * protocols are in the other
	 */
        class groupTCPUDPServices: public groupServices
        {
            protected:
            virtual int groupingCode(const libfwbuilder::Service *srv);
            public:
            groupTCPUDPServices(const std::string &name) : groupServices(name){}
        };
        
        /**
	 * separate service object that satisfies condition
	 * implemented in the virtual method "condition" so we have
	 * exactly one such object per rule.
	 */
        class separateServiceObject : public BasicRuleProcessor
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv) =0;
            public:
            separateServiceObject(const std::string &name);
            virtual bool processNext();
        };

	/**
	 * separate TCP/UDP services (regardless of their source or
	 * destination port configuration)
	 */
        class separateTCPUDP : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateTCPUDP(const std::string &name) :
                separateServiceObject(name) {}
        };
        
	/**
	 * separate TCP/UDP services that specify source port (can
	 * not be used in combination with destination port with
	 * multiport)
	 */
        class separateSrcPort : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateSrcPort(const std::string &name) :
                separateServiceObject(name) {}
        };
        
	/**
	 * separate TCP/UDP services that specify both source and
	 * destination port
	 */
        class separateSrcAndDstPort : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateSrcAndDstPort(const std::string &name) :
                separateServiceObject(name) {}
        };
        /**
	 * separate Tag services so we have exactly one per rule.
	 */
        class separateTagged : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateTagged(const std::string &name) :
                separateServiceObject(name) {}
        };

        /**
	 * separate Custom services so we have exactly one per rule.
	 */
        class separateCustom : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateCustom(const std::string &name) :
                separateServiceObject(name) {}
        };

        class separateUserServices : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateUserServices(const std::string &name) :
                separateServiceObject(name) {}
        };

        /**
	 * separate IPService objects with tos attrubute so we have
	 * exactly one per rule.
	 */
        class separateTOS : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateTOS(const std::string &name) :
                separateServiceObject(name) {}
        };

        /**
	 * split rules with more than one IPService object with
	 * options, so that each rule has only one such service
	 */
        class splitIpOptions : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            splitIpOptions(const std::string &name) :
                separateServiceObject(name) {}
        };

	/**
	 * separate TCP services with flags (can't use those in combination
	 * with others in groups of services)
	 */
        class separateTCPWithFlags : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separateTCPWithFlags(const std::string &name) :
                separateServiceObject(name) {}
        };

        /**
         * separate TCP and UDP services that match port ranges and
         * "any tcp" or "any udp" objects
         */
        class separatePortRanges : public separateServiceObject
        {
            protected:
            virtual bool condition(const libfwbuilder::Service *srv);
            public:
            separatePortRanges(const std::string &name) :
                separateServiceObject(name) {}
        };
            
	/**
	 * verify if custom services used in rules are configured for
	 * this platform
	 */
        class verifyCustomServices : public BasicRuleProcessor
        {
            public:
            verifyCustomServices(const std::string &name) :
                BasicRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * simply check if TCPService object with "established" flag
         * set is used in Service and abort with an error saying that
         * target firewall does not support this. Use for pretty much
         * every platform except ipfw and router ACLs
	 */
        class CheckForTCPEstablished : public BasicRuleProcessor
        {
            public:
            CheckForTCPEstablished(const std::string &name) :
                BasicRuleProcessor(name) {}
            virtual bool processNext();
        };

	/**
	 * simply check if UserService objectis used in Service and
         * abort with an error saying that target firewall does not
         * support this.
	 */
        class CheckForUnsupportedUserService : public BasicRuleProcessor
        {
            public:
            CheckForUnsupportedUserService(const std::string &name) :
                BasicRuleProcessor(name) {}
            virtual bool processNext();
        };

        /**
         * This rule processor replaces firewall object with
         * DNSName object "self" configured as run-time with source
         * name "self".
         */
        class ReplaceFirewallObjectWithSelfInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
                ReplaceFirewallObjectWithSelfInRE(const std::string &n,
                                      std::string _type) :
                BasicRuleProcessor(n) { re_type=_type; }
            virtual bool processNext();
        };
        
        class RegisterGroupsAndTablesInRE : public BasicRuleProcessor
        {
            std::string re_type;
            public:
                RegisterGroupsAndTablesInRE(const std::string &n,
                                            const std::string _type) :
                BasicRuleProcessor(n) { re_type = _type; }
            virtual bool processNext();
        };
        friend class Compiler::RegisterGroupsAndTablesInRE;

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

        /**
         * returns cached firewall object ID
         */
        int getFwId() { return fw_id; }

        /**
         * returns pointer to the cached firewall options object
         */
        libfwbuilder::FWOptions* getCachedFwOpt() { return fwopt; }
        
	/**
	 * internal: scans children of 's' and, if finds host or
	 * firewall with multiple interfaces, replaces reference to
	 * that host or firewall with a set of references to its
	 * interfaces. Argument 's' should be a pointer at either src
	 * or dst in the rule. Some platforms may require alterations
	 * to * this algorithm, that's why it is virtual.
	 */
	virtual void _expand_addr(libfwbuilder::Rule *rule,
                                  libfwbuilder::FWObject *s,
                                  bool expand_cluster_interfaces_fully);

        /**
         * internal: scans child objects of interface iface, both IPv4
         * and physAddress, and puts them in the list ol. Policy
         * compilers for platforms that support matching on MAC
         * address should reimplement this method and do whatever is
         * right for them (e.g. create combined address objects to
         * fuse information from IPv4 and physAddress together).
         *
         * Parameter @expand_cluster_interfaces_fully is used when
         * interface @iface belogns to a cluster and is failover
         * interface. If @expand_cluster_interfaces_fully is true,
         * this function scans failover group associated with this
         * interface and takes address of the member firewall for
         * which the policy is being compiled. If the cluster
         * interface belongs to a cluster that is not being compiled,
         * addresses of all members are returned instead. The address
         * of the cluster interface is always returned even when
         * @expand_cluster_interfaces_fully is false.
         */
        virtual void _expand_interface(libfwbuilder::Rule *rule,
                                       libfwbuilder::Interface *iface,
                                       std::list<libfwbuilder::FWObject*> &ol,
                                       bool expand_cluster_interfaces_fully);

	/**
	 * internal: like _expand_addr, but expands address range
	 * objects
	 */
	void _expandAddressRanges(libfwbuilder::Rule *rule,
                                  libfwbuilder::FWObject *s);

	/*
	 * normalizes port range (makes sure that niether range start
	 * nor end is <0 and so on
	 */
	void normalizePortRange(int &rs,int &re);


        /**
         * This method returns true if one of the following conditions is met:
         *
         * 1. obj1 is the same as obj2 (compares ID of both objects), or 
         * 2. obj1 is a child of obj2 on any depth, or
         * 3. address of obj1 matches that of any obj2's interfaces, or 
         * 4. address of obj1 is a broadcast address of one of 
         *    the interfaces of obj2
         * 5. address of obj1 is a broadcast (255.255.255.255)
         */
        bool complexMatch(libfwbuilder::Address *obj1,
                          libfwbuilder::Address *obj2,
                          bool recognize_broadcasts=true,
                          bool recognize_multicasts=true);

        libfwbuilder::Address* correctForCluster(libfwbuilder::Address *adr);
        
        /**
         * Compares given object with firewall or its parent cluster
         * (if any).  Compares only IDs of these objects. Relies on
         * class CompilerDriver to set integer variable
         * "parent_cluster_id" in the firewall object if it is a
         * member of a cluster.
         */
        bool isFirewallOrCluster(libfwbuilder::FWObject *obj);
        
        /**
         * This method finds interface of obj2 (which is usually
         * firewall object, but not necessarily so) which is connected
         * to the subnet on which obj1 is located. It also works if
         * obj1 is a network object, in this case it looks for the
         * interface that belongs to that network.
         */
        libfwbuilder::Interface* findInterfaceFor(
            const libfwbuilder::Address *o1, const libfwbuilder::Address *o2);
        
        /**
         * This method finds an interface of the firewall obj2 which
         * belongs to the subnet on which obj1 is located and returns
         * IPv4 address object of this interface. It also works if
         * obj1 is a network object, in this case it looks for the
         * interface that belongs to that network.
         * 
         * If obj1 is an Interface object, then corresponding Interface
         * object belonging to the firewall is returned (if found).
         */
        libfwbuilder::FWObject* findAddressFor(
            const libfwbuilder::Address *o1, const libfwbuilder::Address *o2);
        
	virtual ~Compiler();

	Compiler(libfwbuilder::FWObjectDatabase *_db,
		 libfwbuilder::Firewall *fw, bool ipv6_policy);

	Compiler(libfwbuilder::FWObjectDatabase *_db,
		 libfwbuilder::Firewall *fw, bool ipv6_policy,
		 fwcompiler::OSConfigurator *_oscnf);
/*
 * TODO: Refactor Compiler to not hide BaseCompiler
 */
	Compiler(libfwbuilder::FWObjectDatabase *_db, bool ipv6_policy);

        /**
         * overloaded methods: uses current firewall and ruleset objects
         */
        virtual void abort(const std::string &errstr);
        virtual void abort(libfwbuilder::FWObject *rule, const std::string &errstr);

        virtual void error(const std::string &errstr);
        virtual void error(libfwbuilder::FWObject *rule, const std::string &errstr);

        virtual void warning(const std::string &errstr);
        virtual void warning(libfwbuilder::FWObject *rule, const std::string &errstr);

	void setDebugLevel(int dl) { debug=dl;       }
	void setDebugRule(int dr)  { debug_rule = dr; rule_debug_on = true; }
	void setVerbose(bool v)    { verbose=v;      }
        void setSingleRuleCompileMode(const std::string &rule_id);
        bool inSingleRuleCompileMode() { return single_rule_mode; }

        void setSourceRuleSet(libfwbuilder::RuleSet *rs);
        libfwbuilder::RuleSet* getSourceRuleSet() { return source_ruleset; }

        void setRuleSetName(const std::string &name) { ruleSetName = name; }
        std::string getRuleSetName() { return ruleSetName; }

        void setPersistentObjects(libfwbuilder::Library* po);
        
	std::string getCompiledScript();
        int getCompiledScriptLength();

        void setGroupRegistry(GroupRegistry *gr) { group_registry = gr; }
        
        void expandGroup(libfwbuilder::FWObject *grp,
                         std::list<libfwbuilder::FWObject*> &ol);

	void expandGroupsInRuleElement(libfwbuilder::RuleElement *s);

        /**
	 * this method should return platform name. It is used 
	 * to construct proper error and warning messages.
	 */
	virtual std::string myPlatformName() { return ""; }

	std::string getUniqueRuleLabel();

	virtual std::string createRuleLabel(const std::string &prefix,
                                            const std::string &txt,
                                            int rule_num);
        virtual std::string printComment(libfwbuilder::Rule *rule,
                                         std::string &prev_rule_label,
                                         const std::string &prefix,
                                         bool suppress_comment=false);

	/**
	 * prolog return number of rules found
	 */
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

	/**
	 * prints rule marked for debugging (its number * is in
	 * debug_rule member variable)
	 */
        void debugRule();

    };
}

#endif
