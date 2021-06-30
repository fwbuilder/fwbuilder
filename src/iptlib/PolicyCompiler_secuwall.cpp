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
#include "fwbuilder/Network.h"

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
        if (iface->isManagement() && iface->getAddressObject() != nullptr)
        {
            interfaces.push_back(iface->getName());
        }
    }

    return interfaces;
}

void PolicyCompiler_secuwall::verifyPlatform()
{
    string family = Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/family");

    if ((family != myPlatformName()) && (fw->getStr("host_OS") != myPlatformName()))
        abort("Unsupported platform " + fw->getStr("platform") +
              " (family " + family + ")");
}

std::string PolicyCompiler_secuwall::printAutomaticRules()
{
    string res = PolicyCompiler_ipt::printAutomaticRules();

    FWOptions* options = getCachedFwOpt();
    if (options->getBool("secuwall_mgmt_rules_disable"))
    {
        return res;
    }

    Configlet configlet(fw, "secuwall", "management_rules");
    configlet.removeComments();
    configlet.collapseEmptyStrings(false);

    PolicyCompiler_ipt::PrintRule *print_rule = createPrintRuleProcessor();

    configlet.setVariable("begin_rule", print_rule->_startRuleLine().c_str());
    configlet.setVariable("end_rule", print_rule->_endRuleLine().c_str());

    // export these variables to the configlet
    std::vector<std::string> vars;
    vars.push_back("secuwall_mgmt_mgmtaddr");
    vars.push_back("secuwall_mgmt_loggingaddr");
    vars.push_back("secuwall_mgmt_ntpaddr");
    vars.push_back("secuwall_mgmt_nagiosaddr");
    vars.push_back("secuwall_mgmt_snmpaddr");
    vars.push_back("secuwall_dns_srv1");
    vars.push_back("secuwall_dns_srv2");
    vars.push_back("secuwall_dns_srv3");

    vars.push_back("secuwall_mgmt_confpart");

    for (vector<string>::iterator it = vars.begin(); it != vars.end(); ++it)
    {
        std::vector<std::string> tmp;
        tokenize(options->getStr(*it), tmp, ",");
        if (!tmp.empty())
        {
            configlet.setVariable(it->c_str(), stringify(tmp, " ").c_str());
            configlet.setVariable((string("has_")+*it).c_str(), true);
        }
    }

    configlet.setVariable("secuwall_mgmt_ifaces", stringify(getMgmtInterfaces(), " ").c_str());

    res += configlet.expand().toStdString();

    delete print_rule;
    return res;
}

/*
 * An interface is considered a valid interface when it is
 *     1. a management interface
 *     2. of type "ethernet"
 */
bool PolicyCompiler_secuwall::isValidMgmtRuleInterface(Interface *intf)
{
    if (intf == nullptr)
        return false;

    if (Interface::isA(intf))
    {
        FWOptions* if_opts = intf->getOptionsObject();
        if (if_opts == nullptr)
            return false;

        /* TODO: Extract magic value! */
        string iface_type = if_opts->getStr("type");
        if (intf->isManagement() && (iface_type != "cluster_member") &&
            (intf->getAddressObject() != nullptr))
        {
            return true;
        }
    }
    return false;
}
