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

#include <fwbuilder/FWObject.h>
#include <fwbuilder/XMLTools.h>

namespace libfwbuilder
{

class FWOptions : public FWObject 
{
    public:
    FWOptions();
    FWOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(FWOptions);
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class HostOptions : public FWOptions 
{
    public:
    HostOptions();
    HostOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(HostOptions);
};

class FirewallOptions : public FWOptions 
{
    public:
    FirewallOptions();
    FirewallOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(FirewallOptions);
};

class PolicyRuleOptions : public FWOptions 
{
    public:
    
    PolicyRuleOptions();
    PolicyRuleOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(PolicyRuleOptions);
};

class NATRuleOptions : public FWOptions 
{
    public:

    NATRuleOptions();
    NATRuleOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(NATRuleOptions);
};

class RoutingRuleOptions : public FWOptions 
{
    public:

    RoutingRuleOptions();
    RoutingRuleOptions(const FWObject *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(RoutingRuleOptions);
};

}

#endif // __FWOPTINS_HH_FLAG
