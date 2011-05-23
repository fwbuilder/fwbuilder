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


#ifndef _FWB_POLICY_IMPORTER_PF_H_
#define _FWB_POLICY_IMPORTER_PF_H_

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include "IOSImporter.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Logger.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/NAT.h"

#include <QString>


class PFImporter : public Importer
{
    
public:

    std::string direction;
    std::string iface;
    std::string address_family;
    bool quick;
    bool src_neg;
    bool dst_neg;
    bool tmp_neg;
    
    std::list<std::string> proto_list;
    std::list<std::pair<std::string, std::string> > src_group;
    std::list<std::pair<std::string, std::string> > dst_group;
    std::list<std::pair<std::string, std::string> > tmp_group;

    // each item in the list is a vector of 2 or 3 strings
    // Unary operations are represented by 2 strings, binary operations
    // use 3 strings
    std::string tmp_port_def;
    std::list< std::vector<std::string> > src_port_group;
    std::list< std::vector<std::string> > dst_port_group;
    std::list< std::vector<std::string> > tmp_port_group;
    
    std::string queue;
    std::string state_op;
    
    libfwbuilder::NATRule::NATRuleTypes rule_type;
    
    PFImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~PFImporter();

    virtual void clear();

    void clearTempVars();
    
    virtual void run();

    void pushPolicyRule();
    void pushNATRule();
    void buildDNATRule();
    void buildSNATRule();
    virtual void pushRule();
    
    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();

    virtual libfwbuilder::FWObject* makeSrcObj();
    virtual libfwbuilder::FWObject* makeDstObj();
    virtual libfwbuilder::FWObject* makeSrvObj();

    virtual void addLogging();

    libfwbuilder::Interface* getInterfaceByName(const std::string &name);
};

#endif
