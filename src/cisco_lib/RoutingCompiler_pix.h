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

#ifndef __ROUTINGCOMPILER_PIX_HH__
#define __ROUTINGCOMPILER_PIX_HH__


#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"

#include "RoutingCompiler_cisco.h"

namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler {

    class RoutingCompiler_pix : public RoutingCompiler_cisco {

    protected:

	virtual std::string myPlatformName();

        class PrintRule : public RoutingCompiler_cisco::PrintRule
        {
            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
            virtual std::string RoutingRuleToString(libfwbuilder::RoutingRule *r);
        };
        friend class RoutingCompiler_pix::PrintRule;
        
    public:

	RoutingCompiler_pix(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler_cisco(_db, fw, ipv6_policy, _oscnf) {};

        /**
         * checks if the gateway or Interface rule element is empty
         * (both are mandatory on PIX)
         */
        DECLARE_ROUTING_RULE_PROCESSOR(emptyRDstOrRItf);
        
	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

    };

}

#endif
