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
    if (FWObjectReference::cast(o)!=nullptr) return true;

    return FWObject::validateChild(o);
}

void DynamicGroup::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    for (xmlNodePtr child = root->xmlChildrenNode;
         child != nullptr; child = child->next) {
        if (child->type != XML_ELEMENT_NODE) continue;
        assert(strcmp(XMLTools::FromXmlCast(child->name), "SelectionCriteria") == 0);

        const char *type = XMLTools::FromXmlCast(xmlGetProp(child, XMLTools::ToXmlCast("type")));
        const char *keyword =
            XMLTools::FromXmlCast(xmlGetProp(child, XMLTools::ToXmlCast("keyword")));
        
        string filter;
        if (makeFilter(filter, type, keyword)) {
            m_filter.push_back(filter);
        }

        XMLTools::FreeXmlBuff(type);
        XMLTools::FreeXmlBuff(keyword);
    }
}

xmlNodePtr DynamicGroup::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlRemoveProp(xmlHasProp(me, XMLTools::ToXmlCast("run_time")));
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    list<string>::const_iterator iter;
    for (iter = m_filter.begin(); iter != m_filter.end(); ++iter) {
        string filter, type, keyword;
        if (!splitFilter(*iter, type, keyword)) continue;
        if (!makeFilter(filter, type, keyword)) continue;

        xmlNodePtr item = xmlNewChild(me, nullptr,
                                      XMLTools::ToXmlCast("SelectionCriteria"), nullptr);
        xmlNewProp(item, XMLTools::ToXmlCast("type"), XMLTools::StrToXmlCast(type));
        xmlNewProp(item, XMLTools::ToXmlCast("keyword"), XMLTools::StrToXmlCast(keyword));
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
                      bool recursive)
{
    if (!FWObject::cmp(obj, recursive)) return false;

    const DynamicGroup *group = DynamicGroup::constcast(obj);
    return group->m_filter == m_filter;
}


FWObject& DynamicGroup::shallowDuplicate(const FWObject *other,
                                        bool preserve_id)
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
{
    (void) ipv6; (void) options; (void) test_mode; // Unused

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
    return lib == nullptr || lib->getId() == FWObjectDatabase::DELETED_OBJECTS_ID;
}


bool DynamicGroup::isMemberOfGroup(FWObject *obj)
{
    if (obj == this) return false;
    if (ObjectGroup::cast(obj) == nullptr && Address::cast(obj) == nullptr) return false;
    if (RuleElement::cast(obj) != nullptr) return false;
    if (isInDeletedObjs(obj)) return false;

    /* There's no way to figure out what are the "standard" object
       groups (like "address tables") from within the fwbuilder
       library, so we rely on counting how deep we are in the tree
       instead. */
    if (ObjectGroup::cast(obj) != nullptr &&
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
