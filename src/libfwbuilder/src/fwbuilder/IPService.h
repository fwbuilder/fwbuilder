/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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


#ifndef __IPSERVICE_HH_FLAG__
#define __IPSERVICE_HH_FLAG__

#include "fwbuilder/Service.h"

namespace libfwbuilder
{

class IPService : public Service
{
    static void initNamedProtocols();

    protected:

    static std::map<int, std::string> named_protocols;

    public:
    
    IPService();
    virtual ~IPService();

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    DECLARE_FWOBJECT_SUBTYPE(IPService);

    DECLARE_DISPATCH_METHODS(IPService);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

    void setProtocolNumber(int n);
    
    std::string getTOSCode() const;
    void setTOSCode(const std::string &c);

    std::string getDSCPCode() const;
    void setDSCPCode(const std::string &c);

    bool hasIpOptions() const;
    
    static void addNamedProtocol(int proto_num, const std::string &proto_name);
};

}



#endif




