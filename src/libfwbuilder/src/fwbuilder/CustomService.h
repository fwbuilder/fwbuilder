/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

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




#ifndef __CUSTOMSERVICE_HH_FLAG__
#define __CUSTOMSERVICE_HH_FLAG__

#include "fwbuilder/Service.h"

#include <map>

// for AF_INET and AF_INET6
#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  include <winsock2.h>
#  include <wtypes.h>
#endif


namespace libfwbuilder
{

class CustomService : public Service
{
    private:
    
    std::map<std::string,std::string>   codes;
    std::string protocol;
    int address_family;
    
    public:
    
    CustomService();
    virtual ~CustomService();
    
    virtual void       fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool cmp(const FWObject *obj, bool recursive=false);
    
    virtual FWObject& shallowDuplicate(
        const FWObject *obj, bool preserve_id = true);
    
    DECLARE_FWOBJECT_SUBTYPE(CustomService);

    DECLARE_DISPATCH_METHODS(CustomService);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

    void setCodeForPlatform(const std::string& platform,
                            const std::string& code);
    const std::string getCodeForPlatform(const std::string& platform) const;
    std::list<std::string> getAllKnownPlatforms();
    
    void setProtocol(const std::string& proto);
    const std::string& getProtocol();

    void setAddressFamily(int af);
    int getAddressFamily();

    virtual bool isV4Only();
    virtual bool isV6Only();
};

}

#endif




