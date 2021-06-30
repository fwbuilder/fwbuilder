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


#include "interfaceProperties.h"
#include "linux24Interfaces.h"

#include <string>
#include <iostream>

#include <QObject>
#include <QRegExp>


using namespace std;
using namespace libfwbuilder;


bool linux24Interfaces::parseVlan(const QString &name, QString *base_name, int *vlan_id)
{
    QList<QRegExp> vlan_name_patterns;
    vlan_name_patterns.append(QRegExp("([a-zA-Z0-9-]+\\d{1,})\\.(\\d{1,})"));
    vlan_name_patterns.append(QRegExp("(vlan)(\\d{1,})"));
    for (int idx=0; idx < vlan_name_patterns.size(); ++idx)
    {
        if (vlan_name_patterns[idx].indexIn(name) != -1)
        {
            if (base_name!=nullptr) *base_name = vlan_name_patterns[idx].cap(1);
            if (vlan_id!=nullptr) *vlan_id = vlan_name_patterns[idx].cap(2).toInt();
            return true;
        }
    }
    return false;
}

/*
 * per #1856, OpenWRT uses "-" in ppp interface names, such as
 * "ppp-dsl". Also bridge interfaces can have "-" in their names. It
 * seems we should just allow "-" in names instead of cherry-picking
 */
bool linux24Interfaces::basicValidateInterfaceName(Interface *intf,
                                                   const QString &obj_name,
                                                   QString &err)
{
    (void) intf; // Unused

    if (obj_name.indexOf(' ') != -1)
    {
        err = QObject::tr("Bridge interface name '%1' can not contain white space").arg(obj_name);
        return false;
    }
    return true;

    return true;
}
    
/*
 * Take original information about interfaces provided by the crawler
 * and try to arrange it into a tree or interfaces and
 * subinterfaces. Guess based on host OS, inetrface names and their
 * MAC addresses. Returns data in the argument @interface_tree
 */
void linux24Interfaces::rearrangeInterfaces(map<int,InterfaceData> &interfaces,
                                            list<InterfaceData*> &interface_tree)
{
    /*
      how to find vlan subinterfaces:

      if interface 1 has name with a dot and numbers after the
      dot, AND its MAC address is the same as MAC address of an
      interface 2 with the name matching the part before the dot,
      then interface 1 is subinterface of 1 and type of interface 2
      should be 8021q

      Example of mixed configuration :

      Bridge:  br0 -> eth4
      Bonding: bond0 -> eth2 eth3
      Vlans:   eth1 -> eth1.100 eth1.101 eth1.102
               bond0 -> bond0.200 bond0.201 bond0.202

eth0      Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:96  

bond0     Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  
bond0.200 Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  
bond0.201 Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  
bond0.202 Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  
eth2      Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  
eth3      Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:AA  

eth1      Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:A0  
eth1.100  Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:A0  
eth1.101  Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:A0  
eth1.102  Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:A0  

br0       Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:BE  
eth4      Link encap:Ethernet  HWaddr 00:0C:29:F6:BE:BE  

    */

    // map mac addresse to list of InterfaceData objects. Note that
    // there can be several interfaces with the same mac address
    map<string, list<InterfaceData*> > all_mac_addresses;

    // pass 1: group interfaces by mac address

    map<int,InterfaceData>::iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i)
    {
        // loopback, tunnels and p2p interfaces have no mac address
        if (i->second.mac_addr.empty())
            interface_tree.push_back(&(i->second));
        else
            all_mac_addresses[i->second.mac_addr].push_back(&(i->second));
    }

    // pass 2: for each unique mac address, create top level interface and 
    // subinterfaces
    map<string, list<InterfaceData*> >::iterator it;
    for (it=all_mac_addresses.begin(); it!=all_mac_addresses.end(); ++it)
    {
        if (it->second.size() > 1)
        {
            // several interfaces with the same mac address
            //
            // Can be:
            // eth0 and eth0.100, eth0.101
            // eth0 and vlan1, vlan2
            // bond0 and eth0, eth1
            // br0 and eth0, eth1

            list<InterfaceData*> vlan_subinterfaces;
            list<InterfaceData*> bond_subinterfaces;
            list<InterfaceData*> bridge_subinterfaces;
            InterfaceData *vlan_parent_interface = nullptr;
            InterfaceData *bond_parent_interface = nullptr;
            InterfaceData *bridge_parent_interface = nullptr;
            list<InterfaceData*>::iterator intf;

            bool bonding = false;
            bool bridge = false;
            bool vlans = false;
            bool have_dots = false;

            for (intf=it->second.begin(); intf!=it->second.end(); ++intf)
            {
                if ((*intf)->name.find("br") == 0) bridge = true;
                if ((*intf)->name.find("bond") == 0) bonding = true;
                if ((*intf)->name.find("vlan") == 0) vlans = true;
                if ((*intf)->name.find(".") != string::npos) have_dots = true;
            }

            // assume that interface with a dot in names are vlan
            // interfaces such as "eth0.100".
            // But vlan interfaces can be "bond0.100" as well.

            if (!vlans && have_dots) vlans = true;

            for (intf=it->second.begin(); intf!=it->second.end(); ++intf)
            {
                linux24Interfaces::interface_type itype{};

                if ((*intf)->name.find("bond") == 0 &&
                    (*intf)->name.find(".") == string::npos)
                    itype = BONDING_INTERFACE;

                if ((*intf)->name.find("eth") == 0 &&
                    (*intf)->name.find(".") == string::npos)
                    itype = ETH_NO_DOT;

                if ((*intf)->name.find(".") != string::npos ||
                    (*intf)->name.find("vlan") == 0)
                    itype = VLAN_INTERFACE;

                if ((*intf)->name.find("br") == 0 &&
                    (*intf)->name.find(".") == string::npos)
                    itype = BRIDGE_INTERFACE;

                
                if (bonding)
                {
                    switch (itype)
                    {
                    case BONDING_INTERFACE:
                        // interface name starts with "bond" and has no dot
                        // like "bond0"
                        (*intf)->interface_type = "bonding";
                        bond_parent_interface = *intf;
                        continue;

                    case ETH_NO_DOT:
                        // interface name starts with "eth" and has no dot
                        // like "eth0"
                        // This is physical interface that is a member of bonding group
                        (*intf)->interface_type = "ethernet";
                        bond_subinterfaces.push_back(*intf);
                        continue;

                    case VLAN_INTERFACE:
                    {
                        (*intf)->interface_type = "8021q";
                        parseVlan((*intf)->name.c_str(), nullptr, &((*intf)->vlan_id));
                        //parseVlan(*intf);
                        bond_subinterfaces.push_back(*intf);
                        continue;
                    }

                    default:
                        break;
                    }
                }

                if (bridge)
                {
                    switch (itype)
                    {
                    case BRIDGE_INTERFACE:
                        // interface name starts with "br" and has no dot
                        // like "br0"
                        (*intf)->interface_type = "bridge";
                        bridge_parent_interface = *intf;
                        continue;

                    case ETH_NO_DOT:
                        // interface name starts with "eth" and has no dot
                        // like "eth0"
                        // This is physical interface that is a member of a bridge
                        (*intf)->interface_type = "ethernet";
                        bridge_subinterfaces.push_back(*intf);
                        // special case: eth0 can be part of the bridge and
                        // vlan parent interface
                        // break from switch but continue the loop
//                        continue;
                        break;

                    case VLAN_INTERFACE:
                    {
                        (*intf)->interface_type = "8021q";
                        parseVlan((*intf)->name.c_str(), nullptr, &((*intf)->vlan_id));
                        //parseVlan(*intf);
                        bridge_subinterfaces.push_back(*intf);
                        // special case: vlan interface can be part of
                        // the bridge and part of the vlan configuration
                        // break from switch but continue the loop
//                        continue;
                        break;
                    }

                    default:
                        break;
                    }
                }

                if (vlans)
                {
                    switch (itype)
                    {
                    case ETH_NO_DOT:
                        // interface name starts with "eth" and has no dot
                        // like "eth0"
                        (*intf)->interface_type = "ethernet";
                        vlan_parent_interface = *intf;
                        continue;

                    case VLAN_INTERFACE:
                    {
                        (*intf)->interface_type = "8021q";
                        parseVlan((*intf)->name.c_str(), nullptr, &((*intf)->vlan_id));
                        //parseVlan(*intf);
                        vlan_subinterfaces.push_back(*intf);
                        continue;
                    }

                    default:
                        break;
                    }
                }

                // if we get here, then interface was not covered by either
                // of the cases above. Just create it as top-level
                interface_tree.push_back(*intf);
            }

            if (bond_parent_interface)
            {
                for (intf=bond_subinterfaces.begin(); intf!=bond_subinterfaces.end(); ++intf)
                    bond_parent_interface->subinterfaces.push_back(*intf);
                interface_tree.push_back(bond_parent_interface);
            }

            if (bridge_parent_interface)
            {
                for (intf=bridge_subinterfaces.begin(); intf!=bridge_subinterfaces.end(); ++intf)
                    bridge_parent_interface->subinterfaces.push_back(*intf);
                interface_tree.push_back(bridge_parent_interface);
            }

            if (vlan_parent_interface)
            {
                for (intf=vlan_subinterfaces.begin(); intf!=vlan_subinterfaces.end(); ++intf)
                    vlan_parent_interface->subinterfaces.push_back(*intf);
                interface_tree.push_back(vlan_parent_interface);
            }

        } else
        {
            // single interface with this mac, just create it as top-level
            interface_tree.push_back(it->second.front());
        }
    }
}

