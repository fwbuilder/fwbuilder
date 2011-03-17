/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

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


#ifndef _FWB_POLICY_IMPORTER_IOS_H_
#define _FWB_POLICY_IMPORTER_IOS_H_

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include "Importer.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Logger.h"
#include "fwbuilder/Policy.h"


class IOSImporter : public Importer
{

    std::map<std::string, std::pair<int,int> > cisco_icmp_specs;

protected:
    
    virtual libfwbuilder::FWObject* createIPService(bool deduplicate=true);
    virtual libfwbuilder::FWObject* createICMPService(bool deduplicate=true);
    virtual libfwbuilder::FWObject* createTCPService(bool deduplicate=true);
    virtual libfwbuilder::FWObject* createUDPService(bool deduplicate=true);

    int convertPort(const std::string &port, const std::string &proto);

    std::pair<int,int> convertPortSpec(const std::string &port_op,
                                       const std::string &port_spec,
                                       const std::string &proto);

public:

    IOSImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~IOSImporter();

    virtual void run();
    
    virtual void setInterfaceAndDirectionForRuleSet(const std::string &ruleset_name,
                                                    const std::string &interface_name,
                                                    const std::string &dir);

    virtual void ignoreCurrentInterface();

    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();


    class merge_rule : public std::unary_function<libfwbuilder::FWObject*, void>
    {
        std::string ruleset_name;
        libfwbuilder::FWObject *intf;
        libfwbuilder::PolicyRule::Direction dir;
        libfwbuilder::FWObject *target_ruleset;
      public:
        merge_rule(const std::string &_n,
                   libfwbuilder::FWObject *i,
                   libfwbuilder::PolicyRule::Direction d,
                   libfwbuilder::FWObject *_rs)
        { ruleset_name = _n; intf = i; dir = d; target_ruleset = _rs; }
        void operator()(libfwbuilder::FWObject* r);
    };


};

#endif
