/*
 * Copyright (c) 2008 Steven Mestdagh
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __ROUTINGCOMPILER_CISCO_HH__
#define __ROUTINGCOMPILER_CISCO_HH__


#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"

#include "NamedObjectsAndGroupsSupport.h"

namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{
    class RoutingCompiler_cisco : public RoutingCompiler
    {

    protected:
     
        NamedObjectsManager *named_objects_manager;

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
         *  eliminates duplicate objects in DST. Uses default comparison
         *  in eliminateDuplicatesInRE which compares IDs
         */
        class eliminateDuplicatesInDST : public eliminateDuplicatesInRE
        {
        public:
            eliminateDuplicatesInDST(const std::string &name) :
                eliminateDuplicatesInRE(name,
                                        libfwbuilder::RuleElementRDst::TYPENAME)
                {}
        };
        
        /**
         *  eliminates duplicate rules
         */
        class eliminateDuplicateRules : public RoutingRuleProcessor
        {
            std::map<std::string, std::string> rules_seen_so_far;
            std::map<std::string, std::string>::iterator rules_it;
            
        public:
                
            eliminateDuplicateRules(const std::string &name) :
                RoutingRuleProcessor(name) {}
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
            protected:
            std::string current_rule_label;
            virtual std::string _printAddr(libfwbuilder::Address  *o);
            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
            virtual std::string RoutingRuleToString(libfwbuilder::RoutingRule *r);
            virtual std::string _printRGtw(libfwbuilder::RoutingRule *r);
            virtual std::string _printRItf(libfwbuilder::RoutingRule *r);
            virtual std::string _printRDst(libfwbuilder::RoutingRule *r);
        };
        friend class RoutingCompiler_cisco::PrintRule;

    public:
        RoutingCompiler_cisco::PrintRule *printRule;
        
	RoutingCompiler_cisco(libfwbuilder::FWObjectDatabase *_db,
                              libfwbuilder::Firewall *fw, bool ipv6_policy,
                              fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler(_db, fw, ipv6_policy, _oscnf) {}

	virtual int  prolog();
	virtual void compile();

        void setNamedObjectsManager(NamedObjectsManager *mgr);

};

}

#endif
