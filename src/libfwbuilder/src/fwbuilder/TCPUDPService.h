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


#ifndef __TCPUDPSERVICE_HH_FLAG__
#define __TCPUDPSERVICE_HH_FLAG__

#include "fwbuilder/Service.h"

namespace libfwbuilder
{

class TCPUDPService : public Service
{
    
    protected:

    int src_range_start;
    int src_range_end;
    int dst_range_start;
    int dst_range_end;
    
    public:

    TCPUDPService();
    virtual ~TCPUDPService();
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);

    
    DECLARE_FWOBJECT_SUBTYPE(TCPUDPService);

    DECLARE_DISPATCH_METHODS(TCPUDPService);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

    int getSrcRangeStart() const { return src_range_start; }
    int getSrcRangeEnd()   const { return src_range_end; }
    int getDstRangeStart() const { return dst_range_start; }
    int getDstRangeEnd()   const { return dst_range_end; }

    void setSrcRangeStart(int p) { src_range_start = p; }
    void setSrcRangeEnd(int p) { src_range_end = p; }
    void setDstRangeStart(int p) { dst_range_start = p; }
    void setDstRangeEnd(int p) { dst_range_end = p; }
    
    virtual bool cmp(const FWObject *obj, bool recursive=false);
};
    
}

#endif




