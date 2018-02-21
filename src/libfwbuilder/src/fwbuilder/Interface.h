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

#ifndef __INTERFACE_HH_FLAG__
#define __INTERFACE_HH_FLAG__

#include <string>

#include "fwbuilder/Address.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/FWOptions.h"

namespace libfwbuilder
{
    class IPv4;
    class IPv6;
    
    class Interface : public Address
    {
private:

        /*
         * @param _bcast_bits The value of the least-significant bit in the IP
         *                   broadcast address used for sending datagrams on
         *                   the (logical) interface associated with the IP
         *                   address of this entry.  For example, when the
         *                   Internet standard all-ones broadcast address is
         *                   used, the value will be 1.  This value applies to
         *                   both the subnet and network broadcasts addresses
         *                   used by the entity on this (logical) interface."
         *
         * @param _snmp_type type of interface, as described in RFC-1213 under
         *                   'ifType' variable description.
         * @param _ostatus,  interface operational status. 'true' means up.
         *
         * These attrbites are not stored in XML yet.
         */
    
        int  bcast_bits;
        bool ostatus;
        int  snmp_type;

protected:

    virtual void replaceReferenceInternal(int oldfw_id, int newfw_id, int &counter);
        
public:
    
        Interface();
        Interface(const Interface &i);
        virtual ~Interface();

        /**
         * Removes reference to given object among
         * children of 'this'. In case of an Interface, we should check for
         * if the reference to @obj is used as a network zone.
         */
        virtual void removeRef(FWObject *obj);

        /**
         * Add reference to given object to 'this'. In case of an Interface,
         * add @obj as a network zone.
         */
        virtual void addRef(FWObject *obj);
    
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);

        virtual bool  validateChild(FWObject *o);

        virtual FWOptions* getOptionsObject();
        virtual FWOptions* getOptionsObjectConst() const;

        DECLARE_FWOBJECT_SUBTYPE(Interface);

        DECLARE_DISPATCH_METHODS(Interface);
        
        /**
         *   each interface must be associated with some security level. Level
         *   is described by interger number between 0 and 100, with 0 being
         *   least secure and 100 most secure levels. By default class Interface
         *   assigns value of 0 to its security level.
         */
        int  getSecurityLevel() const;
        void setSecurityLevel(int level);

        /**
         * set and check 'dyn' attribute - for interfaces that get their
         * IP address dynamically
         */
        void setDyn(bool value);
        bool isDyn() const ;

        /**
         * set and check 'unnum' attribute - for unnumbered interfaces
         */
        void setUnnumbered(bool value);
        bool isUnnumbered() const ;

        /**
         * Check if this is a bridge port. Bridge port is an
         * interfaces with type "ethernet" that is a child of an
         * interface with type "bridge". Type is defined in the
         * InterfaceOptions object.  This is different from how it was
         * in v3.0.x where bridge port was defined by an attribute
         * "bridgeport" of the interface object.  There is no
         * automatic conversion! User must conver their bridge port
         * interfaces manually by dragging them under the bridge
         * interface (typically br0) and setting the type
         * appropriately.
         *
         */
        bool isBridgePort() const;

        /**
         * Check if this is a slave of bonding interface. Slave is an
         * interfaces with type "ethernet" that is a child of an
         * interface with type "bonding". Type is defined in the
         * InterfaceOptions object. 
         */
        bool isSlave() const;

        /**
         * we often need to check if this is a regular interface
         * Equivalent to (!isDyn() && !isUnnumbered() && !isBridgePort())
         */
        bool isRegular() const
        { return (!isDyn() && !isUnnumbered() && !isBridgePort()); }
    
        /**
         * set and check 'unprotected' attribute - for interfaces that
         * should not have any ACLs or policy rules
         */
        void setUnprotected(bool value);
        bool isUnprotected() const ;

        void setDedicatedFailover(bool value);
        bool isDedicatedFailover() const;
        
        void setManagement(bool value);
        bool isManagement() const;

        bool isUp () const { return ostatus; }
        void setOStatus(bool value);

        bool isFailoverInterface() const;

        void setInterfaceType(int _snmp_type);
        int  getInterfaceType() const { return snmp_type; }

        void setBroadcastBits(int _val);

        /**
         * returns true if interface is loopback. Should work correctly
         * for both ipv4 and ipv6 (some day)
         */
        bool isLoopback() const;

        IPv4*  addIPv4();
        IPv6*  addIPv6();

        physAddress*  getPhysicalAddress () const;
        void  setPhysicalAddress(const std::string &pa);


        virtual FWObject& shallowDuplicate(const FWObject *obj,
                                           bool preserve_id = true);
        virtual FWObject& duplicate(const FWObject *obj,
                                    bool preserve_id = true);

        virtual bool cmp(const FWObject *obj, bool recursive=false);

        void duplicateWithIdMapping(const FWObject *src,
                                    std::map<int,int> &id_mapping, bool preserve_id);
        
        const std::string &getLabel() const;
        void setLabel(const std::string& n);

        virtual const Address* getAddressObject() const;
        
        virtual unsigned int dimension()  const { return 1; }

        /**
         * similar to hasInetAddress() but counts addresses
         */
        virtual int countInetAddresses(bool skip_loopback) const;
    
        virtual bool isPrimaryObject() const { return false; }

        /**
         * finds all interfaces of the host (or firewall, since class
         * Firewall inherits Host) without scanning whole tree rooted
         * at this. This is more efficient than calling
         * getByTypeDeep() when firewall has lots of rules.
         */
        static void findAllInterfaces(FWObject *obj, std::list<FWObject*> &interfaces);
    
    };

}

#endif


