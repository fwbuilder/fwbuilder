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

#include "fwbuilder/Logger.h"
#include "fwbuilder/Policy.h"


class IOSImporter : public Importer
{

protected:
    
    virtual libfwbuilder::FWObject* createTCPService(const QString &name="");
    virtual libfwbuilder::FWObject* createUDPService(const QString &name="");
    virtual libfwbuilder::FWObject* createTCPUDPServicePair(const QString &name="");
    virtual libfwbuilder::FWObject* createTCPUDPNeqObject(const QString &proto,
                                                          const QString &name="");
    
    virtual ObjectSignature packObjectSignatureTCPService();
    virtual ObjectSignature packObjectSignatureUDPService();

    
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

    virtual void pushRule();

    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();


    class MergeRules
    {
        std::string ruleset_name;
        libfwbuilder::FWObject *intf;
        libfwbuilder::PolicyRule::Direction dir;
        libfwbuilder::FWObject *target_ruleset;
        
public:
        MergeRules(const std::string &_n,
                   libfwbuilder::FWObject *i,
                   libfwbuilder::PolicyRule::Direction d,
                   libfwbuilder::FWObject *_rs)
        { ruleset_name = _n; intf = i; dir = d; target_ruleset = _rs; }
        void move(libfwbuilder::FWObject* r);
    };


};

#endif
