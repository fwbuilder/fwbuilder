/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#ifndef __COMPILER_DRIVER_IPF_HH__
#define __COMPILER_DRIVER_IPF_HH__

#include "CompilerDriver_pf.h"

#include "TableFactory.h"

#include <string>

#include <QString>
#include <QStringList>


namespace libfwbuilder {
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
};


namespace fwcompiler {

    class CompilerDriver_ipf : public CompilerDriver_pf
    {
        QStringList activation_commands;

        QString composeActivationCommand(libfwbuilder::Firewall *fw,
                                         bool filter,
                                         const std::string &debug,
                                         const std::string &version,
                                         const std::string &remote_file_name);
protected:

        QString printActivationCommandWithSubstitution(libfwbuilder::Firewall *fw);
    
        virtual QString assembleManifest(libfwbuilder::Cluster *cluster,
                                         libfwbuilder::Firewall* fw,
                                         bool cluster_member);
        virtual QString printActivationCommands(libfwbuilder::Firewall *fw);
        virtual QString assembleFwScript(libfwbuilder::Cluster *cluster,
                                         libfwbuilder::Firewall* fw,
                                         bool cluster_member,
                                         OSConfigurator *ocsnf);

public:

        CompilerDriver_ipf(libfwbuilder::FWObjectDatabase *db);

        // create a copy of itself, including objdb
        virtual CompilerDriver* clone();
    
        virtual QString run(const std::string &cluster_id,
                            const std::string &firewall_id,
                            const std::string &single_rule_id);

    };
};

#endif
