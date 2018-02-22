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

#ifndef  __FWOPTINS_HH_FLAG__
#define  __FWOPTINS_HH_FLAG__

#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWObject.h"

namespace libfwbuilder
{

class FWOptions : public FWObject 
{
    public:
    FWOptions();

    DECLARE_FWOBJECT_SUBTYPE(FWOptions);
    DECLARE_DISPATCH_METHODS(FWOptions);
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class HostOptions : public FWOptions 
{
    public:
    HostOptions();

    DECLARE_FWOBJECT_SUBTYPE(HostOptions);
    DECLARE_DISPATCH_METHODS(HostOptions);
};

class FirewallOptions : public FWOptions 
{
    public:
    FirewallOptions();

    DECLARE_FWOBJECT_SUBTYPE(FirewallOptions);
    DECLARE_DISPATCH_METHODS(FirewallOptions);
};

class InterfaceOptions : public FWOptions
{
    public:
    InterfaceOptions();

    DECLARE_FWOBJECT_SUBTYPE(InterfaceOptions);
    DECLARE_DISPATCH_METHODS(InterfaceOptions);
};

class RuleSetOptions : public FWOptions 
{
    public:
    
    RuleSetOptions();

    DECLARE_FWOBJECT_SUBTYPE(RuleSetOptions);
    DECLARE_DISPATCH_METHODS(RuleSetOptions);
};

class PolicyRuleOptions : public FWOptions 
{
    public:
    
    PolicyRuleOptions();

    DECLARE_FWOBJECT_SUBTYPE(PolicyRuleOptions);
    DECLARE_DISPATCH_METHODS(PolicyRuleOptions);
};

class NATRuleOptions : public FWOptions 
{
    public:

    NATRuleOptions();

    DECLARE_FWOBJECT_SUBTYPE(NATRuleOptions);
    DECLARE_DISPATCH_METHODS(NATRuleOptions);
};

class RoutingRuleOptions : public FWOptions 
{
    public:

    RoutingRuleOptions();

    DECLARE_FWOBJECT_SUBTYPE(RoutingRuleOptions);
    DECLARE_DISPATCH_METHODS(RoutingRuleOptions);
};

class ClusterGroupOptions : public FWOptions
{
    public:
    ClusterGroupOptions();

    DECLARE_FWOBJECT_SUBTYPE(ClusterGroupOptions);
    DECLARE_DISPATCH_METHODS(ClusterGroupOptions);
};

}

#endif // __FWOPTINS_HH_FLAG
