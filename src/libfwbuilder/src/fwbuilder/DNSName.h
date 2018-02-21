/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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


#ifndef __DNS_NAME_HH_FLAG__
#define __DNS_NAME_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/MultiAddress.h"

namespace libfwbuilder
{

class DNSName : public MultiAddress
{
    private:
    
    public:

    DECLARE_FWOBJECT_SUBTYPE(DNSName);

    DECLARE_DISPATCH_METHODS(DNSName);
    
    DNSName();

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);
    
    virtual std::string getSourceName();
    virtual void setSourceName(const std::string& source_name);

    std::string getDNSRecordType();
    void setDNSRecordType(const std::string& rectype);
    
    virtual void loadFromSource(bool ipv6, FWOptions *options,
                                bool test_mode=false);
};

}


#endif

