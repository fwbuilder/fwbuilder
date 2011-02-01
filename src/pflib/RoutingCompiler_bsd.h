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

#ifndef __ROUTINGCOMPILER_BSD_HH__
#define __ROUTINGCOMPILER_BSD_HH__

#include <fwbuilder/libfwbuilder-config.h>

#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"
#include "config.h"


namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{

    class RoutingCompiler_bsd : public RoutingCompiler
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
        friend class RoutingCompiler_bsd::PrintRule;

	virtual std::string myPlatformName();

        // These buffers are needed to collect output generated from
        // the single ECMP rules belonging to one destination,
        // because all these routes have to be activated with a single
        // ip command. So ECMP ip commands are built up gradually
        // during compilation and inserted in the shell script after
        // all rules are processed.
        
       
        std::map< std::string, std::string> ecmp_rules_buffer; // sortedDstId+metric-->nexthops
        std::map< std::string, std::string> ecmp_comments_buffer; // sortedDstId+metric-->rule's info for the fw script
        bool have_default_route;
        bool defined_restore_script_output;
        
    public:

	RoutingCompiler_bsd(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler(_db, fw, ipv6_policy, _oscnf)
        {
            have_default_route = false;
            defined_restore_script_output = false;
        }

        virtual void verifyOS();
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();	

    };


}

#endif
