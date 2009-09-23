/*
 * OSConfigurator_secuwall.h - OS configurator for secunet wall host OS
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#ifndef __OSNETWORKCONFIGURATOR_SECUWALL_H_
#define __OSNETWORKCONFIGURATOR_SECUWALL_H_

#include "OSConfigurator_linux24.h"
#include <map>
#include <QIODevice>

namespace fwcompiler
{

class OSConfigurator_secuwall : public OSConfigurator_linux24
{

public:
    virtual ~OSConfigurator_secuwall() {};
    OSConfigurator_secuwall(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw, bool ipv6_policy);

    virtual std::string myPlatformName();

    virtual int  prolog();
    virtual void epilog();

    virtual void processFirewallOptions();

    virtual std::string printPathForAllTools(const std::string &);

    virtual std::list<std::string> getGeneratedFiles() const;

    /* TODO: Merge this mapping with gui interface types mapping */
    /* Value-Defintions of the different String values */
    enum ifaceType { ifNotDefined,
                     ETHERNET,
                     BRIDGE,
                     BONDING,
                     VRRP,
                     VLAN,
                     ifEnd
                   };

    /* Map to associate the strings with the enum values */
    std::map<std::string, ifaceType> s_mapIfaceTypes;

private:
    bool createDirStructure() const;
    int generateManagementFile();
    int generateNetworkFile();
    int generateHostsFile();
    int generateDNSFile();
    int generateNsswitchFile();
    int generateInterfaces();
    int generateInterfaceFile(libfwbuilder::Interface * iface,
                              libfwbuilder::IPv4 * ip_address = NULL,
                              int iface_number = 0);

    int generateSSHKeys();

    int stringToFile(const std::string data,
                     const std::string filename,
                     const QIODevice::OpenMode mode = QIODevice::ReadWrite) const;

    bool containsFirewallKey(std::string in) const;

    std::list<std::string> generated_Files;
};
};

#endif
