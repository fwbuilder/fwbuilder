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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"

#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"

#include <cstring>

using namespace std;
using namespace libfwbuilder;

const char *DynamicGroup::TYPE_NONE = "none";
const char *DynamicGroup::TYPE_ANY = "any";
const char *DynamicGroup::KEYWORD_NONE = ",";
const char *DynamicGroup::KEYWORD_ANY = "";

const char *DynamicGroup::TYPENAME={"DynamicGroup"};

DynamicGroup::DynamicGroup() : MultiAddress() {}

DynamicGroup::~DynamicGroup() {}

bool DynamicGroup::validateChild(FWObject *o)
{ 
    if (FWObjectReference::cast(o)!=NULL) return true;

    return FWObject::validateChild(o);
}

void DynamicGroup::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    for (xmlNodePtr child = root->xmlChildrenNode;
         child != 0; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) continue;
        assert(strcmp(FROMXMLCAST(child->name), "SelectionCriteria") == 0);

        const char *type = FROMXMLCAST(xmlGetProp(child, TOXMLCAST("type")));
        const char *keyword =
            FROMXMLCAST(xmlGetProp(child, TOXMLCAST("keyword")));
        
        string filter;
        if (makeFilter(filter, type, keyword)) {
            m_filter.push_back(filter);
        }

        FREEXMLBUFF(type);
        FREEXMLBUFF(keyword);
    }
}

xmlNodePtr DynamicGroup::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlRemoveProp(xmlHasProp(me, TOXMLCAST("run_time")));
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    list<string>::const_iterator iter;
    for (iter = m_filter.begin(); iter != m_filter.end(); ++iter) {
        string filter, type, keyword;
        if (!splitFilter(*iter, type, keyword)) continue;
        if (!makeFilter(filter, type, keyword)) continue;

        xmlNodePtr item = xmlNewChild(me, NULL,
                                      TOXMLCAST("SelectionCriteria"), NULL);
        xmlNewProp(item, TOXMLCAST("type"), STRTOXMLCAST(type));
        xmlNewProp(item, TOXMLCAST("keyword"), STRTOXMLCAST(keyword));
    }

    return me;
}

bool DynamicGroup::splitFilter(const std::string &filter, std::string &type,
                              std::string &keyword)
{
    size_t pos = filter.find(',');
    if (pos == string::npos) return false;
    type = filter.substr(0, pos);
    keyword = filter.substr(pos + 1);
    return true;
}


bool DynamicGroup::makeFilter(string &filter, const string &type,
                             const string &keyword)
{
    if (type == TYPE_NONE || keyword == KEYWORD_NONE) return false;
    filter = type;
    filter.append(",");
    filter.append(keyword);
    return true;
}


bool DynamicGroup::cmp(const FWObject *obj,
                      bool recursive) throw(FWException)
{
    if (!FWObject::cmp(obj, recursive)) return false;

    const DynamicGroup *group = DynamicGroup::constcast(obj);
    return group->m_filter == m_filter;
}


FWObject& DynamicGroup::shallowDuplicate(const FWObject *other,
                                        bool preserve_id) throw (FWException)
{
    const DynamicGroup *otherObj = DynamicGroup::constcast(other);
    m_filter = otherObj->m_filter;
    return FWObject::shallowDuplicate(otherObj, preserve_id);
}


bool DynamicGroup::isCompileTime() const
{
    return true;
}


void DynamicGroup::loadFromSource(bool ipv6, FWOptions *options, bool test_mode)
    throw (FWException)
{
    FWObjectDatabase *root = getRoot();

    FWObject::tree_iterator tree_iter;
    for (tree_iter = root->tree_begin();
         tree_iter != root->tree_end(); ++tree_iter) {
        FWObject *elem = (*tree_iter);
        if (elem == root) continue;

        if (!isMemberOfGroup(elem)) continue;
        addRef(elem);
    }
}


static bool isInDeletedObjs(FWObject *obj)
{
    FWObject *lib = obj->getLibrary();
    return lib == 0 || lib->getId() == FWObjectDatabase::DELETED_OBJECTS_ID;
}


bool DynamicGroup::isMemberOfGroup(FWObject *obj)
{
    if (obj == this) return false;
    if (ObjectGroup::cast(obj) == 0 && Address::cast(obj) == 0) return false;
    if (RuleElement::cast(obj) != 0) return false;
    if (isInDeletedObjs(obj)) return false;

    /* There's no way to figure out what are the "standard" object
       groups (like "address tables") from within the fwbuilder
       library, so we rely on counting how deep we are in the tree
       instead. */
    if (ObjectGroup::cast(obj) != 0 &&
        obj->getDistanceFromRoot() <= 3) return false;

    const set<string> &keywords = obj->getKeywords();

    list<string>::const_iterator iter;
    for (iter = m_filter.begin(); iter != m_filter.end(); ++iter) {
        string type, keyword;
        splitFilter(*iter, type, keyword);

        if ((type == TYPE_ANY || obj->getTypeName() == type) &&
            (keyword == KEYWORD_ANY || keywords.count(keyword) > 0)) {
            return true;
        }
    }

    return false;
}
