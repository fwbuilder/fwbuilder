/*
 * PolicyCompiler_secuwall.cpp - secunet wall policy compiler implementation
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

#include <iostream>

#include "PolicyCompiler_secuwall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interface.h"

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_secuwall::myPlatformName() { return "secuwall"; }

vector<string> PolicyCompiler_secuwall::getMgmtInterfaces()
{
    vector<string> interfaces;

    /* search Management Interfaces, note: this can be more than one */
    for (FWObjectTypedChildIterator fw_ifaces = fw->findByType(Interface::TYPENAME);
            fw_ifaces != fw_ifaces.end(); ++fw_ifaces)
    {
        Interface *iface = Interface::cast(*fw_ifaces);
        /* Check if it is a management interface */
        if (iface->isManagement() && iface->getAddressObject() != NULL)
        {
            interfaces.push_back(iface->getName());
        }
    }

    return interfaces;
}

PolicyCompiler_secuwall::PolicyCompiler_secuwall(
    FWObjectDatabase *_db,
    Firewall *fw,
    bool ipv6_policy,
    fwcompiler::OSConfigurator *_oscnf,
    std::map<const std::string, bool> *m_n_commands_map) :
    PolicyCompiler_ipt(_db, fw, ipv6_policy, _oscnf, m_n_commands_map)
{
    /* Set destination filename of firewall script to 'sysconfig/$FWNAME.fw' */
    /* It must be done here since it's the only place where we have a pointer */
    /* to the original fw object used by the compiler driver */
    string remote_script_name = "sysconfig/" + fw->getName() + ".fw";
    FWOptions *opts = fw->getOptionsObject();
    if (opts != NULL)
        opts->setStr("script_name_on_firewall", remote_script_name);
}

void PolicyCompiler_secuwall::verifyPlatform()
{
    string family = Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/family");

    if ((family != myPlatformName()) && (fw->getStr("host_OS") != myPlatformName()))
        abort("Unsupported platform " + fw->getStr("platform") +
              " (family " + family + ")");
}

int PolicyCompiler_secuwall::prolog() {
    int return_value = PolicyCompiler_ipt::prolog();

    /* Set mgmt_addr according to secuwall_mgmt_mgmtaddr */
    /* so SSH rule gets set no matter what */
    FWOptions* options = getCachedFwOpt();

    string mgmt_addr = options->getStr("secuwall_mgmt_mgmtaddr");
    if (options != NULL && !mgmt_addr.empty()) {
        options->setBool("mgmt_ssh", true);
        options->setStr("mgmt_addr", mgmt_addr);
    }

    return return_value;
}

void PolicyCompiler_secuwall::addPredefinedPolicyRules()
{
    PolicyCompiler_ipt::addPredefinedPolicyRules();

    /* Add management rules if not disabled */
    FWOptions* options = getCachedFwOpt();
    if (!options->getBool("secuwall_mgmt_rules_disable"))
    {
        insertSyslogRule();
        insertNtpRule();
        insertSnmpRule();
        insertNrpeRule();
    }
}

void PolicyCompiler_secuwall::epilog()
{
    PolicyCompiler_ipt::epilog();
}

void PolicyCompiler_secuwall::insertSyslogRule()
 {
    FWOptions* options = fw->getOptionsObject();

   if (options->getStr("secuwall_mgmt_loggingaddr").empty())
   {
       /* No syslog server specified, nothing left to do */
       return;
   }

    vector<string> addresses;
    tokenize (options->getStr("secuwall_mgmt_loggingaddr"), addresses, ", ");

    for (vector<string>::iterator it = addresses.begin(); it != addresses.end(); ++it)
    {
        /* Add Syslog-Address to database */
        Address *log_dst = NULL;
        log_dst = Address::cast(dbcopy->create(IPv4::TYPENAME));
        log_dst->setName("Logging-Address");
        log_dst->setAddress(InetAddr(*it));
        log_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
        log_dst->setComment("Logging IP Address");
        dbcopy->add(log_dst);

        /* TODO: Handle Syslog via TCP */
        /* Add Syslog-Service to database for first address only */
        UDPService* log_service = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        log_service->setDstRangeStart(514);
        log_service->setDstRangeEnd(514);
        log_service->setComment("Logging service");
        dbcopy->add(log_service);

        /* Add secuwall-specific rules for syslog */
        addMgmtRule(NULL, log_dst, log_service, NULL,
                    PolicyRule::Outbound, PolicyRule::Accept, "Syslog");
    }
}

void PolicyCompiler_secuwall::insertNtpRule()
{
    FWOptions* options = fw->getOptionsObject();

   if (options->getStr("secuwall_mgmt_ntpaddr").empty())
   {
       /* No NTP server specified, nothing left to do */
       return;
   }

    vector<string> addresses;
    tokenize (options->getStr("secuwall_mgmt_ntpaddr"), addresses, ", ");

    for (vector<string>::iterator it = addresses.begin(); it != addresses.end(); ++it)
    {
        /* Add NTP-Address to database */
        Address *ntp_dst = NULL;
        ntp_dst = Address::cast(dbcopy->create(IPv4::TYPENAME));
        ntp_dst->setName("NTP-Address");
        ntp_dst->setAddress(InetAddr(*it));
        ntp_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
        ntp_dst->setComment("NTP IP Address");
        dbcopy->add(ntp_dst);

        /* Add NTP-Service to database (INPUT) */
        UDPService* ntp_input = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        ntp_input->setSrcRangeStart(123);
        ntp_input->setSrcRangeEnd(123);
        ntp_input->setComment("NTP service (INPUT)");
        dbcopy->add(ntp_input);

        /* Add NTP-Service to database (OUTPUT) */
        UDPService* ntp_output = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        ntp_output->setDstRangeStart(123);
        ntp_output->setDstRangeEnd(123);
        ntp_output->setComment("NTP service (OUTPUT)");
        dbcopy->add(ntp_output);

        /* Add secuwall-specific rules for NTP */
        addMgmtRule(ntp_dst, NULL, ntp_input, NULL,
                    PolicyRule::Inbound, PolicyRule::Accept, "NTP");
        addMgmtRule(NULL, ntp_dst, ntp_output, NULL,
                    PolicyRule::Outbound, PolicyRule::Accept, "NTP");
    }
}

void PolicyCompiler_secuwall::insertNrpeRule()
{
    FWOptions* options = fw->getOptionsObject();

   if (options->getStr("secuwall_mgmt_nagiosaddr").empty())
   {
       /* No NRPE server specified, nothing left to do */
       return;
   }

    vector<string> addresses;
    tokenize (options->getStr("secuwall_mgmt_nagiosaddr"), addresses, ", ");

    for (vector<string>::iterator it = addresses.begin(); it != addresses.end(); ++it)
    {
        /* Add NRPE-Address to database */
        Address* nrpe_dst = NULL;
        nrpe_dst = Address::cast(dbcopy->create(IPv4::TYPENAME));
        nrpe_dst->setName("NRPE-Address");
        nrpe_dst->setAddress(InetAddr(*it));
        nrpe_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
        nrpe_dst->setComment("NRPE IP Address");
        dbcopy->add(nrpe_dst);

        /* Add NRPE-Service for INPUT to database */
        CustomService* nrpe_input = CustomService::cast(dbcopy->create(CustomService::TYPENAME));
        nrpe_input->setComment("NRPE service (INPUT)");
        nrpe_input->setCodeForPlatform("secuwall", "-p tcp --dport 5666 -m state --state NEW,ESTABLISHED");
        dbcopy->add(nrpe_input);

        /* Add NRPE-Service for OUTPUT to database */
        CustomService* nrpe_output = CustomService::cast(dbcopy->create(CustomService::TYPENAME));
        nrpe_output->setComment("NRPE service (OUTPUT)");
        nrpe_output->setCodeForPlatform("secuwall", "-p tcp --sport 5666 -m state --state ESTABLISHED,RELATED");
        dbcopy->add(nrpe_output);

        /* Add secuwall-specific rules for NRPE */
        addMgmtRule(nrpe_dst, NULL, nrpe_input, NULL,
                    PolicyRule::Inbound, PolicyRule::Accept, "NRPE");
        addMgmtRule(NULL, nrpe_dst, nrpe_output, NULL,
                    PolicyRule::Outbound, PolicyRule::Accept, "NRPE");
    }
}

void PolicyCompiler_secuwall::insertSnmpRule()
{
    FWOptions* options = fw->getOptionsObject();

   if (options->getStr("secuwall_mgmt_snmpaddr").empty())
   {
       /* No SNMP server specified, nothing left to do */
       return;
   }

    vector<string> addresses;
    tokenize (options->getStr("secuwall_mgmt_snmpaddr"), addresses, ", ");

    for (vector<string>::iterator it = addresses.begin(); it != addresses.end(); ++it)
    {
        /* Add SNMP-Address to database */
        Address* snmp_dst = NULL;
        snmp_dst = Address::cast(dbcopy->create(IPv4::TYPENAME));
        snmp_dst->setName("SNMP-Address");
        snmp_dst->setAddress(InetAddr(*it));
        snmp_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
        snmp_dst->setComment("SNMP IP Address");
        dbcopy->add(snmp_dst);

        /* Add SNMP-Service to database (INPUT) */
        UDPService* snmp_input = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        snmp_input->setDstRangeStart(161);
        snmp_input->setDstRangeEnd(161);
        snmp_input->setComment("SNMP service (INPUT)");
        dbcopy->add(snmp_input);

        /* Add SNMP-Service to database (OUTPUT) */
        UDPService* snmp_output = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        snmp_output->setSrcRangeStart(161);
        snmp_output->setSrcRangeEnd(161);
        snmp_output->setComment("SNMP service (OUTPUT)");
        dbcopy->add(snmp_output);

        /* Add SNMP-Trap-Service to database (OUTPUT) */
        UDPService* snmp_traps = UDPService::cast(dbcopy->create(UDPService::TYPENAME));
        snmp_traps->setDstRangeStart(162);
        snmp_traps->setDstRangeEnd(162);
        snmp_traps->setComment("SNMP-Trap service (OUTPUT)");
        dbcopy->add(snmp_traps);

        /* Add secuwall-specific rules for SNMP & SNMP-Traps */
        addMgmtRule(snmp_dst, NULL, snmp_input, NULL,
                    PolicyRule::Inbound, PolicyRule::Accept, "SNMP");
        addMgmtRule(NULL, snmp_dst, snmp_output, NULL,
                    PolicyRule::Outbound, PolicyRule::Accept, "SNMP");
        addMgmtRule(NULL, snmp_dst, snmp_traps, NULL,
                    PolicyRule::Outbound, PolicyRule::Accept, "SNMP");
    }
}

/*
 * An interface is considered a valid interface when it is
 *     1. a management interface
 *     2. of type "ethernet"
 */
bool PolicyCompiler_secuwall::isValidMgmtRuleInterface(Interface *intf)
{
    if (intf == NULL)
        return false;

    if (Interface::isA(intf))
    {
        FWOptions* if_opts = intf->getOptionsObject();
        if (if_opts == NULL)
            return false;

        /* TODO: Extract magic value! */
        string iface_type = if_opts->getStr("type");
        if (intf->isManagement() && (iface_type.empty() || (iface_type == "ethernet")))
        {
            return true;
        }
    }
    return false;
}
