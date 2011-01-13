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

    class ASA8Object;
    class ASA8ObjectGroup;
    
    class NATCompiler_asa8 : public NATCompiler_pix
    {
	public:

        void addASA8Object(const libfwbuilder::FWObject *obj);
        ASA8Object* getASA8Object(const libfwbuilder::FWObject *obj);
        
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

        DECLARE_NAT_RULE_PROCESSOR(PrintObjectsForNat);

        /*
         * Check that TSrc has right combination of objects after
         * object group has been created. Call after CreateObjectGroupsForTSrc
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyValidityOfTSrc);

        /**
         * TSrc may contain multiple objects, so we should group them
         * in order to put all addresses, address ranges and subnets
         * into an object-group and keep interfaces separate.
         */
        DECLARE_NAT_RULE_PROCESSOR(PrintObjectsForTSrc);
        
        /*
         * Check if "translate dns" option can be used with the rule
         */
        DECLARE_NAT_RULE_PROCESSOR(VerifyValidityOfDNSOption);

	/**
	 * this processor accumulates all rules fed to it by previous
	 * processors, then prints PIX commands to clear
	 * access-lists, then feeds all rules to the next
	 * processor. Usually this processor is in chain right
	 * before PrintRules.
         *
         * We use this processor to print "clear" commands because
         * they need to be generated when all access lists have been
         * created but before they are printed.
         *
         * "Clear" commands on ASA 8.3 are different from older PIX.
	 */
        class PrintClearCommands : public NATRuleProcessor
        {
            public:
            PrintClearCommands(const std::string &n) : NATRuleProcessor(n) {}
            virtual bool processNext();
        };
        friend class NATCompiler_pix::PrintClearCommands;

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

    };


}

#endif
