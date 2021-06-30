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

#ifndef __ROUTINGCOMPILER_FREEBSD_HH__
#define __ROUTINGCOMPILER_FREEBSD_HH__


#include "RoutingCompiler_openbsd.h"

#include "fwbuilder/RuleElement.h"

#include <QMap>
#include <QString>


namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{

    class RoutingCompiler_freebsd : public RoutingCompiler_openbsd
    {
        int routing_rule_counter;
        QMap<int, QString> routing_rules_ids;
        
        QString getNextStaticRouteID();
        
    protected:
     
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
        class PrintRule : public RoutingCompiler_openbsd::PrintRule
        {
        public:

            PrintRule(const std::string &name);
            virtual bool processNext();

            virtual std::string RoutingRuleToString(libfwbuilder::RoutingRule *r,
                                                    bool add_decorations=true);

        };
        friend class RoutingCompiler_freebsd::PrintRule;

    public:

        RoutingCompiler_freebsd(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler_openbsd(_db, fw, ipv6_policy, _oscnf)
        {
            routing_rule_counter = 0;
        }

	virtual void compile();
    };


}

#endif
