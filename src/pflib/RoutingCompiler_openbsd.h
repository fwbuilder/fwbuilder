/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __ROUTINGCOMPILER_OPENBSD_HH__
#define __ROUTINGCOMPILER_OPENBSD_HH__


#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"


namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{

    class RoutingCompiler_openbsd : public RoutingCompiler
    {

    protected:
     
        /**
         * prints rule in some universal format (close to that visible
         * to user in the GUI). Used for debugging purposes. This method
         * calls RoutingCompiler::debugPrintRule
         */
        virtual std::string debugPrintRule(libfwbuilder::Rule *rule);

        /**
	 * expand address range objects in destination
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(addressRangesInDst);
        
	/**
	 * check if we have to install default route
	 */
        DECLARE_ROUTING_RULE_PROCESSOR(FindDefaultRoute);

        /**
         * remove duplicate rules
         */
	class PrintRule;
        class optimize3 : public RoutingRuleProcessor
        {
            std::map<std::string, bool> rules_seen_so_far;
            
        public:
                
            optimize3(const std::string &name) : RoutingRuleProcessor(name) {}
            virtual bool processNext();
        };

        /**
         *  eliminates duplicate rules
         */
        class eliminateDuplicateRules : public RoutingRuleProcessor
        {
            std::map<std::string, std::string> rules_seen_so_far;
            
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

            virtual std::string RoutingRuleToString(libfwbuilder::RoutingRule *r,
                                                    bool add_decorations=true);
            virtual std::string _printRGtw(libfwbuilder::RoutingRule *r);
            virtual std::string _printRItf(libfwbuilder::RoutingRule *r);
            virtual std::string _printRDst(libfwbuilder::RoutingRule *r);

        };
        friend class RoutingCompiler_openbsd::PrintRule;

	virtual std::string myPlatformName();

        bool have_default_route;
        bool defined_restore_script_output;

        PrintRule *printRule;
        
    public:

	RoutingCompiler_openbsd(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler(_db, fw, ipv6_policy, _oscnf)
        {
            have_default_route = false;
            defined_restore_script_output = false;
            printRule = nullptr;
        }

        virtual void verifyOS();
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();	

    };


}

#endif
