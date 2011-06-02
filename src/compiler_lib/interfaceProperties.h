/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#ifndef INTERFACE_PROPERTIES_HH
#define INTERFACE_PROPERTIES_HH

#include <QString>
#include <QList>

#include <map>

#include "fwbuilder/InterfaceData.h"


class interfaceProperties {

protected:
    typedef enum {BONDING_INTERFACE,
                  ETH_NO_DOT,
                  VLAN_INTERFACE,
                  BRIDGE_INTERFACE,
                  UNKNOWN} interface_type;
    bool vlan_checks;
    
    /*
     * collects all ip addresses of the interface (only direct
     * addresses, not addresses of subinterfaces) and fills the list
     * with string representation in the form "address/masklen"
     */
    void getListOfAddresses(libfwbuilder::Interface *intf,
                            QStringList &addr_list);
    
public:
    interfaceProperties() { vlan_checks = true; }
    virtual ~interfaceProperties() {}

    void setPerformVlanChecks(bool f) { vlan_checks = f; }
    
    // simple name validation: does not allow space and "-"
    // However some platform permit space (procurve).
    virtual bool basicValidateInterfaceName(libfwbuilder::Interface *intf,
                                            const QString &proposed_name,
                                            QString &err);
    
    virtual void rearrangeInterfaces(
        std::map<int,libfwbuilder::InterfaceData> &interfaces,
        std::list<libfwbuilder::InterfaceData*> &interface_tree)
    {
        // default rearranger: just copy the data
        std::map<int,libfwbuilder::InterfaceData>::iterator i;
        for (i=interfaces.begin(); i!=interfaces.end(); ++i)
        {
            interface_tree.push_back(&(i->second));
        }
    }

    virtual bool looksLikeInterface(const QString&);
    
    virtual bool parseVlan(const QString&, QString*, int*);
    virtual bool isValidVlanInterfaceName(const QString &,
                                          const QString &,
                                          QString&);
    virtual bool looksLikeVlanInterface(libfwbuilder::InterfaceData*);
    virtual bool looksLikeVlanInterface(const QString&);

    virtual bool validateInterface(libfwbuilder::FWObject *parent,
                                   const QString &inetrface_name,
                                   QString &err);
    virtual bool validateInterface(libfwbuilder::FWObject *parent,
                                   libfwbuilder::FWObject *intf,
                                   bool check_types,
                                   QString &err);

    virtual bool isEligibleForCluster(libfwbuilder::Interface *intf);

    virtual void guessSubInterfaceTypeAndAttributes(libfwbuilder::Interface *intf);

    /**
     * for the given interface return list of its ip addresses that we
     * should manage using update_addresses shell function and list of
     * addresses we should ignore (as in the case of hearbeat or openais
     * cluster interfaces). Returns true if we should manage ip addresses
     * of this interface and false otherwise. Note that it is possible to
     * return true even when there are no addresses to manage, in which
     * case lists update_addresses and ignore_addresses are empty.
     */
    virtual bool manageIpAddresses(libfwbuilder::Interface *intf,
                                   QStringList &update_addresses,
                                   QStringList &ignore_addresses);
};

#endif
