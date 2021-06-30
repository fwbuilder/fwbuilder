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


#ifndef __TCPSERVICE_HH_FLAG__
#define __TCPSERVICE_HH_FLAG__

#include "fwbuilder/TCPUDPService.h"

namespace libfwbuilder
{

class TCPService : public TCPUDPService
{

    public:

    typedef enum {URG, ACK, PSH, RST, SYN, FIN } TCPFlag;

private:
    static std::map<TCPFlag,std::string> flags;
    static std::map<TCPFlag,std::string> flags_masks;
    
    void _init_flags();

public:

    TCPService();
    virtual ~TCPService();

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    DECLARE_FWOBJECT_SUBTYPE(TCPService);

    DECLARE_DISPATCH_METHODS(TCPService);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;

/**
 * this is a shortcut method, it returns true if TCPService object
 * inspects any TCP flags. This is so if it has non-empty set of TCP
 * flag masks. Note that configuration with non-empty set of the TCP
 * flags masks and empty set of TCP flags is completely legal and
 * sometimes desired (checking for null scans, for example).
 */
    bool    inspectFlags() const;

    bool    getTCPFlag(TCPFlag f) const;
    std::set<TCPFlag> getAllTCPFlags() const;
    void    setTCPFlag(TCPFlag f,bool v);

    bool    getTCPFlagMask(TCPFlag f) const;
    std::set<TCPFlag> getAllTCPFlagMasks() const;
    void    setTCPFlagMask(TCPFlag f,bool v);

    void    clearAllTCPFlags();
    void    clearAllTCPFlagMasks();
    void    setAllTCPFlagMasks();

    bool    getEstablished();
    void    setEstablished(bool f);
    
};

}

#endif




