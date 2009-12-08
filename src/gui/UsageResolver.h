#ifndef USAGERESOLVER_H
#define USAGERESOLVER_H

#include "global.h"

#include <set>
#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

class UsageResolver
{
public:
    //UsageResolver();
    static void findWhereUsedRecursively(libfwbuilder::FWObject *, libfwbuilder::FWObject *, std::set<libfwbuilder::FWObject*> &, libfwbuilder::FWObjectDatabase*);
    static std::list<libfwbuilder::Firewall*> findFirewallsForObject(libfwbuilder::FWObject*, libfwbuilder::FWObjectDatabase*);
};

#endif // USAGERESOLVER_H
