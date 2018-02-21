/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Theron Tock

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

#ifndef __DynamicGroup_h__
#define __DynamicGroup_h__

#include "fwbuilder/MultiAddress.h"

namespace libfwbuilder
{

class DynamicGroup : public MultiAddress
{
    std::list<std::string> m_filter;

 public:
    DynamicGroup();
    virtual ~DynamicGroup();

    DECLARE_FWOBJECT_SUBTYPE(DynamicGroup);
    DECLARE_DISPATCH_METHODS(DynamicGroup);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    /* Each list entry is comma-separated list of matching criteria */
    const std::list<std::string> &getFilter() { return m_filter; }
    void setFilter(const std::list<std::string> &filter) { m_filter = filter; }

    static bool splitFilter(const std::string &str, std::string &type,
                            std::string &keyword);
    static bool makeFilter(std::string &filter, const std::string &type,
                           const std::string &keyword);

    virtual bool cmp(const FWObject *obj, bool recursive=false);
    virtual FWObject& shallowDuplicate(const FWObject *other, bool preserve_id);

    virtual bool isCompileTime() const;
    virtual void loadFromSource(bool ipv6, FWOptions *options,
                                bool test_mode=false);

    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool    validateChild(FWObject *o);

    bool isMemberOfGroup(FWObject *obj);

    static const char *TYPE_NONE;
    static const char *TYPE_ANY;
    static const char *KEYWORD_NONE;
    static const char *KEYWORD_ANY;
};

}

#endif  /* __DynamicGroup_h__ */
