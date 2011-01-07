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
#include "ASA8Object.h"

#include <QString>

#include <map>


namespace fwcompiler {

    class NATCompiler_asa8 : public NATCompiler_pix
    {
	public:

        std::map<int, ASA8Object*> asa8_object_registry;

        void addASA8Object(const libfwbuilder::FWObject *obj);
        ASA8Object* getASA8Object(const libfwbuilder::FWObject *obj);
        
        QString sanitizeObjectName(const QString &name);
        std::string createNetworkObjectCommand(libfwbuilder::Address *addr);
        std::string createServiceObjectCommand(libfwbuilder::Service *addr);

        
        DECLARE_NAT_RULE_PROCESSOR(PrintObjectsForNat);
            
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
        public:
            
            PrintRule(const std::string &n);
            virtual void printNONAT(libfwbuilder::NATRule *rule);
            virtual void printSNAT(libfwbuilder::NATRule *rule);
            virtual void printDNAT(libfwbuilder::NATRule *rule);
        };
        friend class NATCompiler_asa8::PrintRule;



	NATCompiler_asa8(libfwbuilder::FWObjectDatabase *_db,
                         libfwbuilder::Firewall *fw,
                         bool ipv6_policy,
                         fwcompiler::OSConfigurator *_oscnf);

	virtual void compile();

    };


}

#endif
