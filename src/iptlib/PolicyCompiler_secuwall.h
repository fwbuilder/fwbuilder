/*
 * PolicyCompiler_secuwall.h - Policy compiler for secunet wall host OS
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

#ifndef __POLICYCOMPILER_SECUWALL_H_
#define __POLICYCOMPILER_SECUWALL_H_

#include <string>
#include <map>

#include "PolicyCompiler_ipt.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Tools.h"

namespace libfwbuilder
{
    class Address;
    class Service;
    class Interface;
    class Address;
    class InetAddr;
}

namespace fwcompiler
{

    class PolicyCompiler_secuwall : public PolicyCompiler_ipt
    {

protected:

    virtual std::string myPlatformName();

public:

    PolicyCompiler_secuwall(libfwbuilder::FWObjectDatabase *_db,
                            libfwbuilder::Firewall *fw,
                            bool ipv6_policy,
                            fwcompiler::OSConfigurator *_oscnf,
                            std::map<const std::string, bool> *m_n_commands_map) :
        PolicyCompiler_ipt(_db, fw, ipv6_policy, _oscnf, m_n_commands_map) {};

    /**
     * Add some predefined rules controlled by checkboxes in
     * firewall settings dialog
     */
    virtual std::string printAutomaticRules();
    virtual void verifyPlatform();

private:

    std::vector<std::string> getMgmtInterfaces();
    static bool isValidMgmtRuleInterface(libfwbuilder::Interface* intf);
    };
}

#endif
