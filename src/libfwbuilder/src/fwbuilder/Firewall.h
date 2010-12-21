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

#ifndef __FIREWALL_HH_FLAG__
#define __FIREWALL_HH_FLAG__

#include <fwbuilder/Host.h>
#include <time.h>   // for time_t

#include <list>

namespace libfwbuilder
{
    class Interface;
    class Policy;
    class NAT;
    class RuleSet;
    class Routing;

class Firewall : public Host 
{

    void duplicateInterfaces(FWObject *target,
                             const FWObject *source,
                             std::map<int,int> &id_mapping,
                             bool preserve_id = true);
    
public:
    
    Firewall();
    Firewall(const FWObjectDatabase *root, bool prepopulate);
    virtual ~Firewall();
    
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML      (xmlNodePtr parent) throw(FWException);
    
    
    DECLARE_FWOBJECT_SUBTYPE(Firewall);

    DECLARE_DISPATCH_METHODS(Firewall);
    
    /**
     * verify whether given object type is approppriate as a child
     */
    virtual bool    validateChild(FWObject *o);

    virtual FWOptions* getOptionsObject();

    /**
     * This method copies content of object 'x' in the object 'this'.
     * Depending on 'preserve_id' flag, Id's are either copied or new
     * ones are issued. Unlike FWObject::duplicate, this method also
     * replaces references to the old firewall in all policy and NAT
     * rules with references to 'this'
     */
    virtual FWObject& duplicate(const FWObject *obj,
                                bool preserve_id = true) throw(FWException);

    /**
     * This method copies all attributes of obj into this, plus
     * FWOptions and Management child objects but no other
     * children.
     */
    virtual FWObject& duplicateForUndo(const FWObject *obj) throw(FWException);

    Policy  *getPolicy();
    NAT     *getNAT();
    Routing *getRouting();

    /**
     * Return list of interfaces of given type. This walks all interfaces recursively,
     * including subinterfaces.
     */
    std::list<libfwbuilder::Interface*> getInterfacesByType(const std::string &iface_type);
    
    time_t getLastModified();
    time_t getLastInstalled();
    time_t getLastCompiled();
    void   updateLastInstalledTimestamp();
    void   updateLastModifiedTimestamp();
    void   updateLastCompiledTimestamp();
    bool   needsInstall();
    bool   needsCompile();
    bool   getInactive();
    void   setInactive(bool b);

};

}

#endif //__FIREWALL_HH_FLAG__
