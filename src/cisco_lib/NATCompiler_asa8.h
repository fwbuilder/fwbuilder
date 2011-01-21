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

#ifndef _NATCOMPILER_ASA8_HH
#define _NATCOMPILER_ASA8_HH

#include "NATCompiler_pix.h"

#include <QString>

#include <map>


namespace fwcompiler {

    class NamedObject;
    class ASA8ObjectGroup;
    
    class NATCompiler_asa8 : public NATCompiler_pix
    {
	public:

        QString sanitizeObjectName(const QString &name);
        std::string createNetworkObjectCommand(libfwbuilder::Address *addr);
        std::string createServiceObjectCommand(libfwbuilder::Service *addr);

        
        /**
         *  verifies correctness of the NAT rules. Some rule types
         *  that were not supported in PIX v <8.3 are supported now,
         *  so this rule processor is slightly different from
         *  NATCompiler_pix::VerifyRules
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyRules);

        /*
         * Check that TSrc has right combination of objects after
         * object group has been created. Call after CreateObjectGroupsForTSrc
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyValidityOfTSrc);

        /*
         * Check if "translate dns" option can be used with the rule
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyValidityOfDNSOption);

        /**
         * for #1954 Issue a warning when nat rule with the same interface
         * is used for the real and mapped interface, as in
         * nat (outside,outside)
         */
	DECLARE_NAT_RULE_PROCESSOR(verifyInterfacesInNatRule);
        
        /**
         * Split rule to make sure objects in OSrc match network zones
         * of interfaces. We only need to do this for ASA 8.3 where we
         * support object-groups in "nat" rules. Older versions did
         * not support groups and so required all nat rules to be
         * atomic which achieved the same effect.
         */
        class splitByNetworkZonesForOSrc : public splitByNetworkZonesForRE
        {
            public:
            splitByNetworkZonesForOSrc(const std::string &n) :
                splitByNetworkZonesForRE(n, libfwbuilder::RuleElementOSrc::TYPENAME)
                {}
        };

        /**
         *  prints single policy rule, assuming all groups have been
         *  expanded, so source, destination and service hold exactly
         *  one object each, and this object is not a group.  Negation
         *  should also have been taken care of before this method is
         *  called.
         */
        friend class PrintRule;
        class PrintRule : public NATCompiler_pix::PrintRule
        {
            QString printSingleObject(libfwbuilder::FWObject *obj);
    public:
            
            PrintRule(const std::string &n);
            virtual void printNONAT(libfwbuilder::NATRule *rule);
            virtual void printSNAT(libfwbuilder::NATRule *rule);
            virtual void printSDNAT(libfwbuilder::NATRule *rule);
            virtual void printDNAT(libfwbuilder::NATRule *rule);
        };
        friend class NATCompiler_asa8::PrintRule;



	NATCompiler_asa8(libfwbuilder::FWObjectDatabase *_db,
                         libfwbuilder::Firewall *fw,
                         bool ipv6_policy,
                         fwcompiler::OSConfigurator *_oscnf);
        virtual ~NATCompiler_asa8();
        
	virtual void compile();

        virtual std::string printClearCommands();

    };


}

#endif
