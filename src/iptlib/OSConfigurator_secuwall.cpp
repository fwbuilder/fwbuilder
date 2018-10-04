/*
 * OSConfigurator_secuwall.cpp - secunet wall OS configurator implementation
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


#include "OSConfigurator_secuwall.h"

#include "fwbuilder/InetAddr.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Tools.h"

#include <errno.h>
#include <ctype.h>
#include <assert.h>

#include <iostream>
#include <cstring>

#include <QDir>
#include <QString>
#include <QTextStream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static string sysconfig_dir      = "sysconfig";
static string ssh_dir            = "ssh";
static string networkscripts_dir = sysconfig_dir + "/network-scripts";
static string hosts_filename     = "hosts";
static string dns_filename       = "resolv.conf";
static string nsswitch_filename  = "nsswitch.conf";
static string mgmt_filename      = sysconfig_dir + "/management";
static string network_filename   = sysconfig_dir + "/network";
static string iface_prefix       = "ifcfg-";
static string iface_filename     = networkscripts_dir+"/"+iface_prefix;

static string fwadmin_ssh_key       = "/.ssh/id_rsa.pub";
static string fwadmin_known_hosts   = "/.ssh/known_hosts2";

string OSConfigurator_secuwall::myPlatformName()
{
    return "Secuwall";
}

OSConfigurator_secuwall::OSConfigurator_secuwall(FWObjectDatabase *_db,
                                                 Firewall *fw,
                                                 bool ipv6_policy) :
    OSConfigurator_linux24(_db, fw, ipv6_policy)
{
    s_mapIfaceTypes["ethernet"] = ETHERNET;
    s_mapIfaceTypes["bridge"]   = BRIDGE;
    s_mapIfaceTypes["bonding"]  = BONDING;
    s_mapIfaceTypes["vrrp"]     = CLUSTER;
    s_mapIfaceTypes["8021q"]    = VLAN;

    s_mapIfaceStrings[ETHERNET] = "ethernet";
    s_mapIfaceStrings[ALIAS]    = "alias";
    s_mapIfaceStrings[BRIDGE]   = "bridge";
    s_mapIfaceStrings[BONDING]  = "bonding";
    s_mapIfaceStrings[CLUSTER]  = "vrrp";
    s_mapIfaceStrings[VLAN]     = "8021q";

    /* search all interfaces, this means here: IP endpoints in stack! */
    list<FWObject *> fw_ifaces = fw->getByTypeDeep(Interface::TYPENAME);

    for (list<FWObject *>::iterator it = fw_ifaces.begin(); it != fw_ifaces.end(); it++)
    {
        Interface *iface = Interface::cast(*it);
        assert(nullptr != iface);
        /* Check if it is a management interface */
        if (!iface->getName().empty() && (nullptr != iface->getAddressObject()))
        {
            m_ifaces.push_back(iface);
        }
    }
}

/*
 * in addition to kernel parameters and other standard things
 * OSConfigurator_linux24 does, this class also creates configuration
 * directories, generates ssh keys and files for sysconfig-style
 * configuration. This method is called from CompilerDriver_ipt::run()
 * and does not run in single rule compile mode.
 */
void OSConfigurator_secuwall::processFirewallOptions()
{
    OSConfigurator_linux24::processFirewallOptions();

    if (!createDirStructure())
        abort("Unable to create directory structure");

    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    /* Do ssh key generation if not disabled. */
    if (!options->getBool("secuwall_no_ssh_key_generation"))
    {
        generateSSHKeys();
    }

    generateHostsFile();
    generateDNSFile();
    generateNsswitchFile();
    generateManagementFile();
    generateNetworkFile();
    generateInterfaces();
}

bool OSConfigurator_secuwall::createDirStructure() const
{
    QDir directory;
    list<QString> dir_names;

    dir_names.push_back (QString (fw->getName().c_str()));

    QString tmp_name = fw->getName().c_str();
    tmp_name.append("/");
    tmp_name.append(ssh_dir.c_str());
    dir_names.push_back(tmp_name);

    tmp_name = fw->getName().c_str();
    tmp_name.append("/");
    tmp_name.append(sysconfig_dir.c_str());
    dir_names.push_back(tmp_name);

    tmp_name = fw->getName().c_str();
    tmp_name.append("/");
    tmp_name.append(networkscripts_dir.c_str());
    dir_names.push_back(tmp_name);

    list<QString>::const_iterator c_iter = dir_names.begin();
    for (; c_iter != dir_names.end(); c_iter++)
    {
        if (!directory.mkdir(*c_iter))
        {
            /* Check if directory already exists */
            if (!directory.exists())
            {
                cerr << "Error[" << errno << "]: " << strerror(errno);
                cerr << " '" << c_iter->toStdString() << "'"<< endl;
                return false;
            }
        }
    }

    cout << " created directory structure successfully" << endl << flush;

    return true;
}

int OSConfigurator_secuwall::generateManagementFile()
{
    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    QString s, mgm_ip, vrrp_secret, stream_string, snmp_ip;
    bool vrrp_master = false;
    vector <string> tmp_v, mgm_iface;

    /* Temporary storage for management file content */
    QTextStream stream (&stream_string);

    /* search Management Interfaces, note: this can be more than one */
    for (list<Interface *>::iterator it = m_ifaces.begin(); it != m_ifaces.end(); it++)
    {
        /* Check if it is a management interface */
        if ((*it)->isManagement())
        {
            mgm_iface.push_back((*it)->getName());
        }
    }

    stream << "MGM_DEV=\"";
    stream << stringify(mgm_iface, " ").c_str();
    stream << "\"" << endl;

    /* lookup Management IP address */
    mgm_ip = options->getStr("secuwall_mgmt_mgmtaddr").c_str();

    if (mgm_ip.isEmpty())
    {
        /* This is only a warning, if the system is not managed online */
        cout << " Warning: no Management IP address specified!" << endl;
    }
    else
    {
        if (mgm_iface.empty())
        {
            abort("At least one management interface is needed for Online Management!");
        }
        else
        {
            stream << "MGM_IP=\"";
            tmp_v.clear();
            tokenize(mgm_ip.toStdString(), tmp_v, ",");
            stream << stringify(tmp_v, " ").c_str();
            stream << s << "\"" << endl;
        }
    }

    /* Log-Server IP address */
    stream << "LOG_IP=\"";
    tmp_v.clear();
    tokenize(options->getStr("secuwall_mgmt_loggingaddr"), tmp_v, ",");
    stream << stringify(tmp_v, " ").c_str();
    stream << "\"" << endl;

    /* SNMP-Server IP address */
    snmp_ip = options->getStr("secuwall_mgmt_snmpaddr").c_str();
    if (!snmp_ip.isEmpty())
    {
        if (mgm_iface.empty())
        {
            abort("At least one management interface is needed for SNMP!");
        }
        else
        {
            stream << "SNMP_IP=\"";
            tmp_v.clear();
            tokenize(options->getStr("secuwall_mgmt_snmpaddr"), tmp_v, ",");
            stream << stringify(tmp_v, " ").c_str();
            stream << "\"" << endl;

            /* SNMP Community string */
            stream << "SNMP_COM=\"";
            stream << options->getStr("secuwall_mgmt_rosnmp").c_str();
            stream << "\"" << endl;
        }
    }

    /* NTP-Server IP address */
    stream << "NTP_IP=\"";
    tmp_v.clear();
    tokenize(options->getStr("secuwall_mgmt_ntpaddr"), tmp_v, ",");
    stream << stringify(tmp_v, " ").c_str();
    stream << "\"" << endl;

    /* /var partition */
    stream << "VARPART=\"";
    stream << options->getStr("secuwall_mgmt_varpart").c_str();
    stream << "\"" << endl;

    /* Configuration partition */
    stream << "CFGPART=\"";
    stream << options->getStr("secuwall_mgmt_confpart").c_str();
    stream << "\"" << endl;

    /* Activate Nagios */
    stream << "NRPE=";
    s.clear();
    s = options->getStr("secuwall_mgmt_nagiosaddr").c_str();
    if (!s.isEmpty())
    {
        stream << "yes" << endl;

        /* Nagios-Server IP-Address */
        stream << "NRPE_IP=\"";
        tmp_v.clear();
        tokenize(s.toStdString(), tmp_v, ",");
        stream << stringify(tmp_v, " ").c_str();
        stream << "\"" << endl;
    }
    else
    {
        stream << "no" << endl;
    }

    /* VRRP interfaces */

    for (FWObjectTypedChildIterator fw_ifaces = fw->findByType(Interface::TYPENAME);
             fw_ifaces != fw_ifaces.end(); ++fw_ifaces)
    {
        Interface *iface = Interface::cast(*fw_ifaces);
        /* Check if it is a VRRP interface */
        FWObject *failover_group =
            iface->getFirstByType(FailoverClusterGroup::TYPENAME);
        if (failover_group)
        {
            FWOptions *failover_opts =
                FailoverClusterGroup::cast(failover_group)->getOptionsObject();
            if (failover_group->getStr("type") == "vrrp" && failover_opts != nullptr)
            {
                vrrp_secret = failover_opts->getStr("vrrp_secret").c_str();
                vrrp_master = iface->getOptionsObject()->getBool("failover_master");;
            }
        }
    }

    /* Activate VRRP */
    stream << "VRRPD=";
    if (options->getBool("cluster_member"))
    {
        stream << "yes" << endl;
        /* VRRP secret */
        stream << "VRRPSECRET=\"";
        stream << vrrp_secret;
        stream << "\"" << endl;

        /* VRRP Master/Slave */
        stream << "MASTER=";
        stream << (vrrp_master ? "yes" : "no");
        stream << endl;
    }
    else
    {
        stream << "no" << endl;
    }

    /* conntrackd */
    s.clear();
    s = options->getStr("state_sync_interface").c_str();
    stream << "CONNTRACKD=";
    if (!s.isEmpty())
    {
        stream << "yes" << endl;
        /* conntrack device */
        stream << "CONN_DEV=\"";
        stream << s;
        stream << "\"" << endl;
    }
    else
    {
        stream << "no" << endl;
    }

    /* Write actual management file */
    string filename = fw->getName() + "/" + mgmt_filename;
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << mgmt_filename << " successfully" << endl << flush;

    return 0;
}

/* Routes are expected to be verified & valid since this step is executed after Route policy compilation */
int OSConfigurator_secuwall::generateNetworkFile()
{
    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    FWObject *routes = fw->getFirstByType(Routing::TYPENAME);
    assert(routes);

    QString s, ifName, gwAddress, stream_string;

    /* Temporary storage for file content */
    QTextStream stream (&stream_string);

    /* Default route */
    RoutingRule* defaultRoute = nullptr;

    /* Prepend static content */
    stream << "NETWORKING=yes" << endl;

    /* Find default route */
    FWObjectTypedChildIterator routing_rules = routes->findByType(RoutingRule::TYPENAME);
    for (; routing_rules != routing_rules.end(); ++routing_rules)
    {
        RoutingRule* route = RoutingRule::cast(*routing_rules);
        if (!route->isEmpty() && !route->isDisabled() && route->getRDst()->isAny())
        {
            defaultRoute = route;
            /* There can only be one default route, so we are done */
            break;
        }

    }

    if (defaultRoute != nullptr)
    {
        RuleElementRItf* itfrel = defaultRoute->getRItf();
        RuleElementRGtw* gtwrel = defaultRoute->getRGtw();

        FWObject *oRGtw = FWReference::cast(gtwrel->front())->getPointer();
        assert(oRGtw != nullptr);
        FWObject *oRItf = FWReference::cast(itfrel->front())->getPointer();
        assert(oRItf != nullptr);

        /* Extract Gateway IP address */
        if (Host::cast(oRGtw) != nullptr)
        {
            Host *host=Host::cast(oRGtw);
            gwAddress = host->getAddressPtr()->toString().c_str();
        }
        else if (Interface::cast(oRGtw) != nullptr)
        {
            Interface *intf=Interface::cast(oRGtw);
            gwAddress = intf->getAddressPtr()->toString().c_str();
        }
        else if (Address::cast(oRGtw)->dimension()==1)
        {
            Address *ipv4 = Address::cast(oRGtw);
            gwAddress = ipv4->getAddressPtr()->toString().c_str();
        }
        /* Extract Interface name */
        ifName = oRItf->getName().c_str();
    }

    /* XXX: not setting gateway since default route will be set by routing rules */
    /* Default Gateway */
    stream << "GATEWAY=\"";
    s = gwAddress;
    {
    // stream << s;
    }
    stream << "\"" << endl;

    /* Gateway interface */
    stream << "GATEWAYDEV=\"";
    s = ifName;
    {
    // stream << s;
    }
    stream << "\"" << endl;

    /* Hostname */
    stream << "HOSTNAME=\"";
    stream << fw->getName().c_str();
    stream << "\"" << endl;

    /* Routing */
    stream << "FORWARD_IPV4=\"";
    if (options->getBool("linux24_ip_forward"))
    {
        stream << "yes";
    }
    else
    {
        stream << "no";
    }
    stream << "\"" << endl;

    /* Write actual network file */
    string filename = fw->getName() + "/" + network_filename;
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << network_filename << " successfully" << endl << flush;

    return 0;
}

int OSConfigurator_secuwall::generateHostsFile()
{
    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    QString s, stream_string;

    /* Temporary storage for file content */
    QTextStream stream (&stream_string);

    /* Prepend static content */
    stream << "127.0.0.1\tlocalhost\n\n# Secuwall hosts" << endl;

    /* TODO: Should entries of every fw interface address be appended?  */

    stream << options->getStr("secuwall_dns_hosts").c_str();
    stream << endl;

    /* Write actual hosts file */
    string filename = fw->getName() + "/" + hosts_filename;
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << hosts_filename << " successfully" << endl << flush;

    return 0;
}

int OSConfigurator_secuwall::generateDNSFile()
{
    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    QString s, stream_string;

    /* Temporary storage for file content */
    QTextStream stream (&stream_string);

    /* Search domains */
    s = options->getStr("secuwall_dns_domains").c_str();
    if (!s.isEmpty())
    {
        /* Replace \n with " " */
        s.replace(QString("\n"), QString(" "));
        stream << "search\t\t" << s << endl;
    }

    /* DNS-Server entries */
    s = options->getStr("secuwall_dns_srv1").c_str();
    if (!s.isEmpty())
        stream << "nameserver\t" << s << endl;

    s = options->getStr("secuwall_dns_srv2").c_str();
    if (!s.isEmpty())
        stream << "nameserver\t" << s << endl;

    s = options->getStr("secuwall_dns_srv3").c_str();
    if (!s.isEmpty())
        stream << "nameserver\t" << s << endl;

    /* Write actual DNS file */
    string filename = fw->getName() + "/" + dns_filename;
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << dns_filename << " successfully" << endl << flush;

    return 0;
}

int OSConfigurator_secuwall::generateNsswitchFile()
{
    FWOptions* options = fw->getOptionsObject();
    assert(options != nullptr);

    QString s, stream_string;

    /* Temporary storage for file content */
    QTextStream stream(&stream_string);

    /* Prepend static content */
    stream << "passwd:\t\tfiles\nshadow:\t\tfiles\ngroup:\t\tfiles\n" << endl;

    /* hosts entries */
    stream << "hosts:\t\t";
    s = options->getStr("secuwall_dns_reso1").c_str();
    if (!s.isEmpty() && s != "none")
    {
        stream << s;
    }

    s = options->getStr("secuwall_dns_reso2").c_str();
    if (!s.isEmpty() && s != "none")
    {
        stream << " " << s;
    }

    s = options->getStr("secuwall_dns_reso3").c_str();
    if (!s.isEmpty() && s != "none")
    {
        stream << " " << s;
    }

    s = options->getStr("secuwall_dns_reso4").c_str();
    if (!s.isEmpty() && s != "none")
    {
        stream << " " << s;
    }

    s = options->getStr("secuwall_dns_reso5").c_str();
    if (!s.isEmpty() && s != "none")
    {
        stream << " " << s << endl;
    }

    stream << endl;

    /* Append static content */
    stream << "ethers:\t\tfiles\nnetmasks:\tfiles\nnetworks:\tfiles\nprotocols:\tfiles\nrpc:\t\tfiles\nservices:\tfiles\n";

    /* Write actual nsswitch file */
    string filename = fw->getName() + "/" + nsswitch_filename;
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << nsswitch_filename << " successfully" << endl << flush;

    return 0;
}

int OSConfigurator_secuwall::generateInterfaceFile (Interface * iface, string name, IPv4 * ip_address, int iface_number)
{
    FWOptions* options = nullptr;
    ifaceType itype = ifNotDefined;
    QString s;

    /* Temporary storage for file content */
    QString stream_string;
    QTextStream stream(&stream_string);

    assert(iface != nullptr);

    /* fallback for name of the interface */
    if (name.empty())
        name = iface->getName();

    if (name.empty())
        abort("cannot get name for interface");

    /* determine the type of the interface */
    if (iface->getName().find("*") == string::npos)
        options = iface->getOptionsObject();

    if (iface_number > 0)
    {
        itype = ALIAS;
    }
    else if (options == nullptr || options->getStr("type").empty())
    {
        itype = ETHERNET;
    }
    else
    {
        itype = s_mapIfaceTypes[options->getStr("type")];
    }

    /* shortcut: unconfigured ethernet devices just exist, they don't need a config file */
    if ((itype == ETHERNET) && (ip_address == nullptr) && (iface->getAddressObject() == nullptr))
        return 0;

    /* Interface name */
    stream << "DEVICE=\"";
    stream << name.c_str();
    if (iface_number > 0)
        stream << ":" << iface_number;
    stream << "\"" << endl;

    /* Boot-Protocol */
    stream << "BOOTPROTO=\"";
    if (iface->isDyn())
    {
        stream << "dhcp";
    }
    else
    {
        stream << "none";
    }
    stream << "\"" << endl;

    /* Address object contains host, network and broadcast address plus netmask */
    const Address* ipAddr = nullptr;
    if (ip_address != nullptr)
        ipAddr = ip_address->getAddressObject();

    if (ipAddr != nullptr)
    {
        /* Interface IP Address */
        stream << "IPADDR=\"";
        stream << ipAddr->getAddressPtr()->toString().c_str();
        stream << "\"" << endl;

        /* Netmask */
        stream << "NETMASK=\"";
        stream << ipAddr->getNetmaskPtr()->toString().c_str();
        stream << "\"" << endl;

        /* Network IP Address */
        stream << "NETWORK=\"";
        stream << ipAddr->getNetworkAddressPtr()->toString().c_str();
        stream << "\"" << endl;

        /* Broadcast IP Address */
        stream << "BROADCAST=\"";
        stream << ipAddr->getBroadcastAddressPtr()->toString().c_str();
        stream << "\"" << endl;
    }

    /* Activate on bootup */
    stream << "ONBOOT=\"";
    if (options != nullptr && options->getBool("iface_disableboot"))
    {
        stream << "no";
    }
    else
    {
        stream << "yes";
    }
    stream << "\"" << endl;

    /* Link */
    stream << "LINK=\"";
    if (options != nullptr)
    {
        stream << options->getStr("iface_options").c_str();
    }
    stream << "\"" << endl;

    /* MAC-Address */
    stream << "MACADDR=\"";
    physAddress* macAddr = iface->getPhysicalAddress();
    if (macAddr != nullptr)
    {
        stream << macAddr->getPhysAddress().c_str();
    }
    stream << "\"" << endl;

    /* MTU */
    s.clear();
    stream << "MTU=\"";
    if (options == nullptr || (s = options->getStr("iface_mtu").c_str()).isEmpty())
    {
        /* TODO: Extract magic value */
        /* Set to "sane" default: "1500" */
        s = "1500";
    }
    stream << s;
    stream << "\"" << endl;

    /* Activate ARP */
    stream << "ARP=\"";
    if (options != nullptr && options->getBool("iface_disablearp"))
    {
        stream << "no";
    }
    else
    {
        stream << "yes";
    }
    stream << "\"" << endl;

    /* Interface type */
    stream << "TYPE=\"";
    stream << s_mapIfaceStrings[itype].c_str();
    stream << "\"" << endl;

    /* get all direct children of type interface */
    list<FWObject *> basedevs = iface->getByType(Interface::TYPENAME);

    /* Type-specific parameter handling */
    switch (itype)
    {
    case BRIDGE:
        /* Fall-through */
    case BONDING:
        /* Iterate over all child interfaces */
        if (basedevs.empty())
        {
            abort("No base device specified for " + name);
        }
        else
        {
            vector<string> devs;
            for (list<FWObject *>::iterator it = basedevs.begin(); it != basedevs.end(); it++)
            {
                Interface *iface = Interface::cast(*it);
                assert(nullptr != iface);
                if (!(iface->getName().empty()))
                {
                    devs.push_back(iface->getName());
                    generateInterfaceFile(iface);
                }
            }

            /* Base Device */
            stream << "BASEDEV=\"";
            stream << stringify(devs," ").c_str();
            stream << "\"" << endl;
        }
        break;

    case VLAN:
        if (options == nullptr || options->getStr("vlan_id").empty())
        {
            abort("No VLAN id specified for " + name);
        }

        stream << "VLANID=\"";
        stream << options->getStr("vlan_id").c_str();
        stream << "\"" << endl;

        if (iface->getParent() == nullptr || iface->getParent()->getName().empty())
        {
            /* No base device provided */
            abort("No base device specified for " + name);
        }

        stream << "BASEDEV=\"";
        stream << iface->getParent()->getName().c_str();
        stream << "\"" << endl;

        generateInterfaceFile(Interface::cast(iface->getParent()));
        break;

    case CLUSTER:
        if (options->getStr("base_device").empty())
        {
            /* No base device provided */
            abort("No base device specified for " + name);
        }

        stream << "BASEDEV=\"";
        stream << options->getStr("base_device").c_str();
        stream << "\"" << endl;
        break;

    case ALIAS:
        /* Base Device for secondary interfaces*/
        stream << "BASEDEV=\"";
        stream << name.c_str();
        stream << "\"" << endl;
        break;

    default:
        /* Don't define BASEDEV */
        break;
    }


    /* Write actual interface file */
    string filename = fw->getName() + "/" + iface_filename + name;
    if (iface_number > 0)
    {
        stringstream tmp;
        tmp << ":" << iface_number;
        filename += tmp.str();
    }
    stringToFile(stream.string()->toStdString(), filename);

    cout << " wrote " << filename << " successfully" << endl << flush;

    return 0;
}

template <class T>
inline std::string toString (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

int OSConfigurator_secuwall::generateInterfaces()
{
    /* clean up possibly stale interface files */
    string nwdir = fw->getName() + "/" + networkscripts_dir;

    QDir d(nwdir.c_str());
    QStringList entries = d.entryList();

    for (QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry)
    {
        if (*entry != "." && *entry != "..")
        {
            d.remove(*entry);
        }
    }

    int vrrp_count = 0;
    /* Iterate over all top-level interfaces */
    for (list<Interface *>::iterator it = m_ifaces.begin(); it != m_ifaces.end(); it++)
    {
        string ifname = (*it)->getName();
        FWOptions *options = (*it)->getOptionsObject();

        /* rename handling for our vrrp "devices" */
        if ((options != nullptr) && options->getBool("cluster_interface"))
        {
            ifname = "vrrp" + ::toString(vrrp_count++);
        }

        /* Iterate over all addresses */
        FWObjectTypedChildIterator j = (*it)->findByType(IPv4::TYPENAME);
        int count = 0;
        for (; j != j.end(); ++j, ++count)
        {
            IPv4 *address = IPv4::cast(*j);
            generateInterfaceFile (*it, ifname, address, count);
        }
    }

    return 0;
}

int OSConfigurator_secuwall::stringToFile(const std::string data,
        const std::string filename,
        const QIODevice::OpenMode mode) const
{
    QFile file (QString (filename.c_str()));

    if (!file.open (mode))
    {
        cerr << "Unable to open file " << filename << endl;
    }

    qint64 byte_count = file.write (data.c_str());
    if (byte_count == -1)
    {
        cerr << "Unable to write data to file " << filename << endl;
    }
    if (data.length() != (unsigned int) byte_count)
    {
        cerr << "Unable to write all data (" << byte_count << " of " << data.length() << " bytes) to file " << filename << endl;
    }
    file.close();
    file.setPermissions (QFile::ReadOwner|QFile::WriteOwner|QFile::ReadGroup|QFile::ReadOther);

    return 0;
}

int OSConfigurator_secuwall::generateSSHKeys()
{
    int i;
    string cmd;
    QString pwd = QDir::currentPath();
    string filename;
    string hostKey_file;
    string fwadmin_keyfilename;
    QFile file (filename.c_str());

    /* TODO: Rewrite with popen for error handling */
    /* Generate RSA Keys */
    filename = fw->getName() + "/" + ssh_dir + "/ssh_host_rsa_key";
    if (!QFile::exists(filename.c_str()))
    {
        cmd = "ssh-keygen -t rsa -b 2048 -f " + pwd.toStdString() + "/" + filename + " -C root@" + fw->getName() + " -P \"\" 2>&1";
        i = system(cmd.c_str());
    }
    else
    {
        cout << " Found existing RSA key: skipping key generation." << endl;
    }

    /* Generate DSA Keys */
    filename =  fw->getName() + "/" + ssh_dir + "/ssh_host_dsa_key";
    if (!QFile::exists(filename.c_str()))
    {
        cmd = "ssh-keygen -t dsa -f " + pwd.toStdString() + "/" + filename + " -C root@" + fw->getName() + " -P \"\" 2>&1";
        i = system(cmd.c_str());
    }
    else
    {
        cout << " Found existing DSA key: skipping key generation." << endl;
    }

    Q_UNUSED(i);

    /* Add RSA pub key of fwadmin to the firewall's known hosts file */
    fwadmin_keyfilename = getenv("HOME");
    fwadmin_keyfilename += fwadmin_ssh_key;
    QFile fwadmin_ssh_keyfile(fwadmin_keyfilename.c_str());
    filename = pwd.toStdString() + "/" + fw->getName() + "/" + ssh_dir + "/authorized_keys2";
    if (!QFile::exists(filename.c_str()))
    {
        if (fwadmin_ssh_keyfile.open(QIODevice::ReadOnly))
        {
            /* Temporary storage for file content */
            QString stream_string;
            QTextStream stream(&stream_string);
            stream << fwadmin_ssh_keyfile.readAll();
            fwadmin_ssh_keyfile.close();
            /* Write actual authorized_keys2 file */
            stringToFile(stream.string()->toStdString(),
                         fw->getName() + "/" + ssh_dir + "/authorized_keys2");

            QFile::setPermissions (filename.c_str(), QFile::ReadOwner|QFile::ReadGroup);
        }
        else
        {
            cout << " Unable to open " << fwadmin_keyfilename << endl;
        }
    }
    else
    {
        cout << " Found existing key authorization file: skipping addition of management key." << endl;
    }

    /* Add RSA host key of firewall to the fwadmin's known hosts file */
    string hostKey_filename = getenv("HOME");
    hostKey_filename += fwadmin_known_hosts;
    QFile host_key_file(hostKey_filename.c_str());
    bool keyPresent = false;
    /* Check if hosts key file exists */
    if (host_key_file.exists())
    {
        /* Check if key entry is present */
        host_key_file.open(QIODevice::ReadOnly);
        QTextStream known_hosts(&host_key_file);
        QString tmp;
        while(!known_hosts.atEnd())
        {
            known_hosts >> tmp;
            if (containsFirewallKey(tmp.toStdString()))
                keyPresent = true;
        }
        host_key_file.close();
    }

    if (!keyPresent)
    {
        filename = pwd.toStdString() + "/" + fw->getName() + "/" + ssh_dir + "/ssh_host_rsa_key.pub";
        QFile ssh_keyfile(filename.c_str());
        if (ssh_keyfile.open(QIODevice::ReadOnly))
        {
            /* Temporary storage for file content */
            QString stream_string;
            QTextStream stream(&stream_string);
            stream << fw->getName().c_str() << " ";
            stream << ssh_keyfile.readAll();
            ssh_keyfile.close();

            /* Append entry to authorized_keys2 file */
            stringToFile(stream.string()->toStdString(), hostKey_filename, QIODevice::Append);
        }
        else
        {
            cout << " Unable to open firewall public key" << endl;
        }
    }
    else
    {
        cout << " Found existing authorization entry: skipping addition of firewall key." << endl;
    }

    cout << " generated SSH keys successfully" << endl << flush;

    return 0;
}

/* TODO: Put in utility library */
bool OSConfigurator_secuwall::containsFirewallKey(string in) const
{
    string match = "root@"+fw->getName();
    if (match == in)
        return true;
    else
        return false;
}

string OSConfigurator_secuwall::printPathForAllTools(const string &)
{
    return OSConfigurator_linux24::printPathForAllTools("secuwall");
}

map<string, string> OSConfigurator_secuwall::getGeneratedFiles() const
{
    map<string, string> files;
    return files;
}
