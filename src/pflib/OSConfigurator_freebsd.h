/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#ifndef _OSCONFIGURATOR_FREEBSD_HH
#define _OSCONFIGURATOR_FREEBSD_HH


#include "OSConfigurator_bsd.h"
#include "OSData_pf.h"

#include <QMap>
#include <QStringList>


namespace libfwbuilder
{
    class Interface;
};


namespace fwcompiler
{

    class OSConfigurator_freebsd : public OSConfigurator_bsd
    {
        QMap<QString, QStringList> ifconfig_lines;
        QMap<QString, QStringList> ipv6_ifconfig_lines;
        

        virtual void printIfconfigLines(const QMap<QString, QStringList> &lines);
        
        virtual void setKernelVariable(libfwbuilder::Firewall *fw,
                                       const std::string &var_name,
                                       Configlet *configlet);

        virtual void summaryConfigLineIP(QStringList intf_names,
                                                 bool ipv6);
        virtual void interfaceConfigLineIP(
            libfwbuilder::Interface *iface,
            std::list<std::pair<libfwbuilder::InetAddr,libfwbuilder::InetAddr> >
            all_addresses);


        virtual void summaryConfigLineVlan(QStringList vlan_names);

        virtual void interfaceConfigLineVlan(
            libfwbuilder::Interface *iface,
            const std::list<libfwbuilder::Interface*> &vlan_subinterfaces);

        // functions that generate bridge configuration
        virtual void summaryConfigLineBridge(QStringList vlan_names);

        virtual void interfaceConfigLineBridge(
            libfwbuilder::Interface *iface, QStringList vlan_names);

        // functions that generate CARP interface configuration
        virtual void summaryConfigLineCARP(QStringList carp_names);
        virtual void interfaceConfigLineCARP(libfwbuilder::Interface *iface,
                                         libfwbuilder::FWObject *failover_group);

        // functions that generate pfsync interface configuration
        virtual void summaryConfigLinePfsync(bool have_pfsync);
        virtual void interfaceConfigLinePfsync(
            libfwbuilder::Interface *iface,
            libfwbuilder::StateSyncClusterGroup *sync_group);

        // this function generates additional ifconfig parameters
        virtual void interfaceIfconfigLine(libfwbuilder::Interface *iface);
        
        virtual QString printAllInterfaceConfigurationLines();

        
	public:

	virtual ~OSConfigurator_freebsd() {};
	OSConfigurator_freebsd(libfwbuilder::FWObjectDatabase *_db,
                               libfwbuilder::Firewall *fw,
                               bool ipv6_policy) : 
        OSConfigurator_bsd(_db, fw, ipv6_policy) {}

        virtual int prolog();

	virtual std::string myPlatformName();
	virtual std::string printKernelVarsCommands();
    };
};

#endif
