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

#ifndef __ROUTINGCOMPILER_IPT_HH__
#define __ROUTINGCOMPILER_IPT_HH__


#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"

namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler {


    class RoutingCompiler_ipt : public RoutingCompiler {

    protected:
     
        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls RoutingCompiler::debugPrintRule
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

	/**
	 * processes rules with negation in Dst if it holds only one object
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(singleDstNegation);

	/**
	 * processes rules with negation in Dst
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(DstNegation);

	/**
	 * check if we have to install default route
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(FindDefaultRoute);

        /**
	 * expand address range objects in destination
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(addressRangesInDst);
        
        /**
         * remove duplicate rules
         */
	class PrintRule;
        class optimize3 : public RoutingRuleProcessor
        {
            std::map<std::string, bool> rules_seen_so_far;
            RoutingCompiler_ipt::PrintRule *printRule;
            
        public:
                
            optimize3(const std::string &name) : RoutingRuleProcessor(name){
                printRule=nullptr;
            }
            virtual bool processNext();
        };
        friend class RoutingCompiler_ipt::optimize3;

        /**
         *  eliminates duplicate objects in DST. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInDST : public eliminateDuplicatesInRE
        {
            
        public:
            
            eliminateDuplicatesInDST(const std::string &n) :
                eliminateDuplicatesInRE(n,libfwbuilder::RuleElementRDst::TYPENAME) {}
        };
        
        /**
         *  eliminates duplicate rules
         */
        class eliminateDuplicateRules : public RoutingRuleProcessor
        {
            std::map<std::string, std::string> rules_seen_so_far;
            std::map<std::string, std::string>::iterator rules_it;
            RoutingCompiler_ipt::PrintRule *printRule;
            
        public:
                
            eliminateDuplicateRules(const std::string &name) : RoutingRuleProcessor(name){
                printRule=nullptr;
            }
            virtual bool processNext();
        };


	/**
	 *  prints single policy rule, assuming all groups have been
	 *  expanded, destination holds exactly one object, and this
	 *  object is not a group.  Negation should also have been taken
	 *  care of before this method is called.
         *
         *  This processor is not necessarily the last in the
         *  conveyor, so it should push rules back to tmp_queue (for
         *  example there could be progress indicator processor after
         *  this one)
	 */
        class PrintRule : public RoutingRuleProcessor
        {
            bool        print_once_on_top;
            std::string current_rule_label;

            virtual std::string _printAddr(libfwbuilder::Address  *o);
            
        public:

            PrintRule(const std::string &name);
            virtual bool processNext();

            std::string RoutingRuleToString(libfwbuilder::RoutingRule *r);
            std::string _printRGtw(libfwbuilder::RoutingRule *r);
            std::string _printRItf(libfwbuilder::RoutingRule *r);
            std::string _printRDst(libfwbuilder::RoutingRule *r);

        };
        friend class RoutingCompiler_ipt::PrintRule;

	virtual std::string myPlatformName();

        // These buffers are needed to collect output generated from the single ECMP rules belonging to one destination,
        // because all these routes have to be activated with a single ip command. So ECMP ip commands are built up gradually
        // during compilation and inserted in the shell script after all rules are processed.
        
        
        std::map< std::string, std::string> ecmp_rules_buffer; // sortedDstId+metric-->nexthops
        std::map< std::string, std::string> ecmp_comments_buffer; // sortedDstId+metric-->rule's info for the fw script
        bool have_default_route;
        bool defined_restore_script_output;
        
    public:

	RoutingCompiler_ipt(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler(_db, fw, ipv6_policy, _oscnf)
        {
            have_default_route = false;
            defined_restore_script_output = false;
        }


        virtual void verifyPlatform();
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();	

    };


}

#endif
