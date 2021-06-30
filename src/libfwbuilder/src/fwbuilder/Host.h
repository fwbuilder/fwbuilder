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

#ifndef __HOST_HH_FLAG__
#define __HOST_HH_FLAG__

#include <vector>
#include <list>

#include "fwbuilder/Address.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/ObjectMatcher.h"

namespace libfwbuilder
{

    class Interface;
    class Management;

class Host : public Address
{
    protected:

    public:

    Host();
    virtual ~Host();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML      (xmlNodePtr parent);


    DECLARE_FWOBJECT_SUBTYPE(Host);

    DECLARE_DISPATCH_METHODS(Host);
    
    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool    validateChild(FWObject *o);

    bool Appropriate(Interface *i);
    void addInterface(Interface *i);
    void removeInterface(Interface *i);

    const InetAddr* getManagementAddress();

    /**
     *  This method returns reference to the object representing
     *  host's platform-specific options (class HostOptions)
     */
    virtual FWOptions* getOptionsObject();

    /**
     * Returns management object associated with this Host.
     * If none is present, new one will be created;
     */
    Management *getManagementObject();

    virtual const Address* getAddressObject() const;
    virtual unsigned int dimension()  const { return 1; }

    /**
     * similar to hasInetAddress() but counts addresses
     */
    virtual int countInetAddresses(bool skip_loopback) const;

    virtual bool isPrimaryObject() const { return true; }

    /**
     *  helper-function, needed when dealing with sub-interfaces:
     *  function returns the parent host (or firewall) of an interface
     *  or rule set. This is just a convenience function that performs
     *  operation we often need.
     */
    static FWObject* getParentHost(FWObject *obj);

    protected:
    
    Management *mgmt;



};

}

#endif
