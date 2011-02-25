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


#ifndef _FWB_POLICY_IMPORTER_PIX_H_
#define _FWB_POLICY_IMPORTER_PIX_H_

#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>

#include "IOSImporter.h"

#include "fwbuilder/libfwbuilder-config.h"
#include "fwbuilder/Logger.h"

class PIXImporter : public IOSImporter
{

    public:

    PIXImporter(libfwbuilder::FWObject *lib,
                std::istringstream &input,
                libfwbuilder::Logger *log,
                const std::string &fwname);
    ~PIXImporter();

    virtual void run();
    
    // this method actually adds interfaces to the firewall object
    // and does final clean up.
    virtual libfwbuilder::Firewall* finalize();
};

#endif
