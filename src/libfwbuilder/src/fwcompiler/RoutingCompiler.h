/* 

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>
  
  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#ifndef __ROUTING_COMPILER_HH__
#define __ROUTING_COMPILER_HH__

#include "fwcompiler/Compiler.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include <string>

namespace fwcompiler
{
    using namespace std;

    #define DECLARE_ROUTING_RULE_PROCESSOR(_Name) \
        friend class _Name; \
	class _Name : public RoutingRuleProcessor { \
		public: \
		_Name(const std::string &n) :  \
			RoutingRuleProcessor(n) {}; \
                virtual ~_Name() {}; \
		virtual bool processNext(); \
	};


    class RoutingCompiler : public Compiler
    {
	public:

	RoutingCompiler(libfwbuilder::FWObjectDatabase *_db,
                        libfwbuilder::Firewall *fw,
                        bool ipv6_policy,
                        fwcompiler::OSConfigurator *_oscnf)   :
        Compiler(_db, fw, ipv6_policy, _oscnf) {}
 
        /**
         *  deals with empty groups in RDst. See description for
         *  Compiler::emptyGroupsInRE
         */
        class emptyGroupsInRDst : public emptyGroupsInRE
        {
            public:
            emptyGroupsInRDst(const std::string &n) :
                emptyGroupsInRE(n,libfwbuilder::RuleElementRDst::TYPENAME) {}
        };
        
        /**
         *  deals with recursive groups in RDst. See description for
         *  Compiler::recursiveGroupsInRE
         */
        class recursiveGroupsInRDst : public recursiveGroupsInRE
        {
            public:
            recursiveGroupsInRDst(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementRDst::TYPENAME) {}
        };
                
        /**
         * creates a label with a sorted dst-id-list, to find identical destinations even if the order 
         * of the dst objects differs within one rule
         */
        DECLARE_ROUTING_RULE_PROCESSOR(createSortedDstIdsLabel);
        
        /**
         * checks if the Destination and the Interface are both empty
         */
        DECLARE_ROUTING_RULE_PROCESSOR(emptyRDstAndRItf);
        
        /**
         * If the Gateway is a host or a network interface, it must lead to
         * only one IP adress.
         * e.g.: if a host has two interfaces, you must specify which interface
         * has to be used as gateway. If the inteface has two or more IP addresses,
         * only an IP Adress Object can be used as Gateway.
         */
        DECLARE_ROUTING_RULE_PROCESSOR(singleAdressInRGtw);
        
        /**
         * Invalid routing destination network: 
         * network address and netmask mismatch.
         */
        friend class validateNetwork;
        class validateNetwork : public RoutingRuleProcessor { 
            public: 
                validateNetwork(const std::string &n) : RoutingRuleProcessor(n) {};
                virtual ~validateNetwork() {};
                virtual bool processNext();
                bool checkValidNetwork(libfwbuilder::FWObject*);
        };
        
        /**
         * the IP address of the gateway has to be 
         * in the same local network as the firewall
         */
        friend class reachableAddressInRGtw;
        class reachableAddressInRGtw : public RoutingRuleProcessor { 
            public: 
                reachableAddressInRGtw(const std::string &n) : RoutingRuleProcessor(n) {};
                virtual ~reachableAddressInRGtw() {};
                virtual bool processNext();
                bool checkReachableIPAddress(libfwbuilder::FWObject*);
        };
        
        /**
         * the IP address of the gateway has to be
         * in the same network as the interface
         */
        DECLARE_ROUTING_RULE_PROCESSOR(contradictionRGtwAndRItf);
        
        /**
         * checks if the Interface (RItf) is a child of the current firewall
         */
        DECLARE_ROUTING_RULE_PROCESSOR(rItfChildOfFw);

        /**
         * some OS (e.g. BSD) allow me to set up static route via
         * gateway or via interface, but not both in one rule.
         */
        DECLARE_ROUTING_RULE_PROCESSOR(interfaceOrGateway);
        
        /**
         * for OS where we do not support ECMP, detect rules that
         * define routes for the same destination via different
         * gateways and abort.
         */
        DECLARE_ROUTING_RULE_PROCESSOR(sameDestinationDifferentGateways);
        
        /**
         * checks for competing rules
         */
	class PrintRule;
        class competingRules : public RoutingRuleProcessor
        {
            /**
             * 'rules_seen_so_far' is a nested map with the following structure 
             * MAP MAP        PAIR
             * |-Dst1
             * |   |-Gtw&Itf1-<metric,label>
             * |   |-Gtw&Itf2-<metric,label>
             * |
             * |-Dst2
             *     |-Gtw&Itf1-<metric,label>
             *     |-Gtw&Itf2-<metric,label>
             *     |-Gtw&Itf3-<metric,label>
             *
             *   dest         gtw+itf       metric  label     
             */
            map< string, map< string, pair< string, string> > > rules_seen_so_far;
            map< string, map< string, pair< string, string> > >::iterator dest_it;
            map< string, pair< string, string> >::iterator gtwitf_it;
            
            public:
            competingRules(const std::string &name) : RoutingRuleProcessor(name){}
            virtual bool processNext();
        };
        friend class RoutingCompiler::competingRules;

        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes
         * TODO: implement
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);
 
        /**
         *  this inspector replaces references to hosts and firewalls
         *  in dst with references to their interfaces
         *
         * TODO: move to class Compiler because this might be useful
         * for PolicyCompiler, NATCompiler and also RoutingCompiler
         */
        DECLARE_ROUTING_RULE_PROCESSOR(ExpandMultipleAddresses);
           
        /**
         * this processor converts to atomic rules using all objects
         * contained in Routing Destination
         */
        DECLARE_ROUTING_RULE_PROCESSOR(ConvertToAtomicForDST);
        
        /**
	 * this class expands groups in dst. It creates
	 * references to new objects "in place" (that is, it does not
	 * create new rules but rather uses rule elements of the old
	 * ones)
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(ExpandGroups);
        
        /**
	 * this processor classifies routing rules into single and
         * multi path rules. Needs slurp().
	 */
        class classifyRoutingRules : public RoutingRuleProcessor
        {
            map< string, map< string, pair< string, libfwbuilder::RoutingRule*> > > rules_seen_so_far;
            map< string, map< string, pair< string, libfwbuilder::RoutingRule*> > >::iterator dest_it;
            map< string, pair< string, libfwbuilder::RoutingRule*> >::iterator gtwitf_it;
            
            public:
            classifyRoutingRules(const std::string &name) : RoutingRuleProcessor(name) {}
            virtual bool processNext();
        };
        friend class RoutingCompiler::classifyRoutingRules;

        /**
         * Placeholders for MultiAddressRunTime objects
         */
        class processMultiAddressObjectsInRE : public RoutingRuleProcessor
        {
            std::string re_type;
            public:
            processMultiAddressObjectsInRE(const std::string &name,
                      const std::string &t) : RoutingRuleProcessor(name) { re_type=t; }
            virtual bool processNext();
        };

        class processMultiAddressObjectsInRDst : public processMultiAddressObjectsInRE
        {
            public:
            processMultiAddressObjectsInRDst(const std::string &n) :
                processMultiAddressObjectsInRE(
                    n, libfwbuilder::RuleElementRDst::TYPENAME) {}
        };
        
        /**
         * detects if rules r1 and r2 are identical (that is, have the
         * same effect, rather than use the same objects)
         *
         * returns:
         *
         *  true if r1 is identical to r2
         */
        bool cmpRules(const libfwbuilder::RoutingRule &r1,
                      const libfwbuilder::RoutingRule &r2);


        /**
         *  deals with recursive groups in Dst. See description for
         *  Compiler::recursiveGroupsInRE
         
        class recursiveGroupsInDst : public recursiveGroupsInRE
        {
            public:
            recursiveGroupsInSrc(const std::string &n) :
                recursiveGroupsInRE(n,libfwbuilder::RuleElementSrc::TYPENAME) {}
        };*/

	virtual int prolog();
    };


};

#endif
