/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#include "TableFactory.h"

#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/AttachedNetworks.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <iterator>

#include <assert.h>
#include <sstream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


void print_string(const string &s)
{
    cerr << s << " ";
}


TableFactory::TableFactory(BaseCompiler *comp, Firewall *fwall,
                           Library *persistent_objects,
                           GroupRegistry *_group_registry)
{
    compiler = comp;
    firewall = fwall;
    ruleSetName = "";
    group_registry = _group_registry;
    dbroot = nullptr;
    persistent_tables = new ObjectGroup();
    persistent_tables->setName("PF Tables");
    persistent_objects->add(persistent_tables);
}

void TableFactory::init(FWObjectDatabase *_dbr)
{
    dbroot = _dbr;
    // dbroot->add(persistent_tables);
    // persistent_tables->fixTree();
}

void TableFactory::detach()
{
    // dbroot->remove(persistent_tables, false);
}

struct joinIDs : public unary_function<string, void>
{
    string    out, sep;
    joinIDs(const string& _sep) { sep=_sep; };
    void operator() (string x) { out += x + sep; }
};

string TableFactory::generateTblID(RuleElement *re)
{
    string res;
    list<string> lids;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o   = *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        lids.push_back(FWObjectDatabase::getStringId(o->getId()));
    }
    lids.sort();
    joinIDs R = for_each(lids.begin(), lids.end(), joinIDs("_"));
    return R.out;
}

void TableFactory::registerTable(const string& tblname, const string& tblid,
                                 FWObject* tbl)
{
// two different table objects should have different names
// 
    if (tables.count(tblid)!=0 && 
        tblnames.count(tblname)!=0 &&
        tables[tblid]->getName()!=tbl->getName()
    )
        compiler->abort("table object name must be unique: '"+tblname+"'");

    tblnames[tblname] = tblid;
    tables[tblid] = tbl;
    table_deduplicator[tblname] = set<int>();
}

FWObject* TableFactory::createTableObject(const string &tblname,
                                          const string &tblid)
{
    FWObject *tblgrp = dbroot->createObjectGroup();

    tblgrp->setName( tblname );
    tblgrp->setId(FWObjectDatabase::generateUniqueId()); //  "id_" + tblname );

    persistent_tables->add(tblgrp, false);
    dbroot->addToIndex(tblgrp);

    tblgrp->setBool("pf_table", true);
    tblgrp->setStr("pf_table_id", tblid);

    registerTable(tblname, tblid, tblgrp);

    return tblgrp;
}

/*
 * Add object to the table group, but perform a check to make sure we
 * do not add it again if it is already there. See #2671
 */
void TableFactory::addObjectToTable(FWObject *tblgrp, FWObject *obj)
{
    int obj_id = obj->getId();
    string tblname = tblgrp->getName();
    if (table_deduplicator[tblname].count(obj_id) == 0)
    {
        tblgrp->addRef(obj);
        table_deduplicator[tblname].insert(obj_id);
    }
}

void TableFactory::createTablesForRE(RuleElement *re, Rule *rule)
{
    // sanity checks
    assert(rule->getRoot()==re->getRoot());
    assert(dbroot==rule->getRoot());

    /*
     * get the list of groups that used to be in this rule element
     * when we started
     */
    set<string> original_groups = group_registry->getGroupsForRE(re);

    string tblID = generateTblID(re);

    FWObject *tblgrp = nullptr;

    list<FWObject*> objects_in_groups;
    list<FWObject*> objects;
    set<FWObject*> table_objects;

    /*
     * separate objects that should be in a user-defined groups
     * (tables) and those that dont
     */

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
        if ( ! group_registry->getGroupRegistryKey(o).empty())
            objects_in_groups.push_back(o);
        else
            objects.push_back(o);
    }

    re->clearChildren();

    if (original_groups.size() > 0)
    {
        for (FWObject::iterator i=objects_in_groups.begin(); i!=objects_in_groups.end(); i++)
        {
            FWObject *obj = *i;

            set<string> groups = group_registry->getGroupsForObject(obj);
            set<string> groups_in_this_re;

            /*
             * an object can be a member of multiple groups, but not all
             * of these groups belong to the given RE.
             *
             * set_intersection requires both sets to be sorted, but STL class set is
             * always sorted automatically.
             */

            std::set_intersection(
                original_groups.begin(), original_groups.end(),
                groups.begin(), groups.end(),

                std::insert_iterator< set<string> >(
                    groups_in_this_re, groups_in_this_re.begin() ));

            for (set<string>::iterator it=groups_in_this_re.begin(); it!=groups_in_this_re.end(); ++it)
            {
                string tblname = *it;

                if (tables.count(tblname)!=0)
                {
                    tblgrp = tables[tblname];
                } else
                {
                    tblgrp = createTableObject(tblname, tblname);
                }

                /*
                 * Add object to the table but first check if this object
                 * already belongs to the group. If RE had two groups and
                 * this object used to belong to both, set
                 * groups_in_this_re will have two items.
                 *
                 * See #2671
                 */
                addObjectToTable(tblgrp, obj);

                table_objects.insert(tblgrp);
            }
        }
    } else
    {
        // if RE never had any groups to begin with
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *obj = FWReference::getObject(*i);
            objects.push_back(obj);
        }
    }

    /*
     * Deal with objects that weren't part of any user-defined group
     */
    if (objects.size() > 0)
    {
        if (objects.size() == 1)
        {
            re->addRef(objects.front());
        } else
        {
            // objects.size() > 1
            if (tables.count(tblID)!=0)
            {
                tblgrp = tables[tblID];
            } else
            {
// TODO: can two rules yeild the same name for the group using this method?
                std::ostringstream tblname;
                if (!ruleSetName.empty()) tblname << ruleSetName << ":";
                int rp = rule->getPosition();
                tblname << "tbl.r";
                tblname << ((rp>0)?rp:0);

                //if (rule_iface) tblname << rule_iface->getName()+".";
                // tblname=tblname+rule->getId();
                if (RuleElementSrc::isA(re)) tblname << ".s";
                if (RuleElementDst::isA(re)) tblname << ".d";

                while (tblnames.count(tblname.str())>0)  tblname << "x";

                tblgrp = createTableObject(tblname.str(), tblID);

                for (FWObject::iterator i=objects.begin(); i!=objects.end(); i++)
                {
                    addObjectToTable(tblgrp, *i);
                    // tblgrp->addRef(*i);
                }
            }

            table_objects.insert(tblgrp);
        }
    }

    for (set<FWObject*>::iterator i=table_objects.begin(); i!=table_objects.end(); i++)
        re->addRef(*i);
}

string TableFactory::PrintTables()
{
    if (tables.size() == 0) return "";

    stringstream output;
    output << endl;
    output << "# Tables: (" << tables.size() << ")" << endl;

    for (map<string,string>::const_iterator i=tblnames.begin();
         i!=tblnames.end(); i++)
    {
        string tblID = i->second;
        FWObject *grp = tables[tblID];
        output << "table ";
        output << "<" << grp->getName() << "> ";
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(grp);
        if (atrt!=nullptr &&
            atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
        {
            output << "persist";
            if ( !atrt->getSourceName().empty() )
            {
                string path =
                    atrt->getSourceNameAsPath(firewall->getOptionsObject());
                if (path.empty()) {
                    compiler->abort("Error: Firewall's data directory not set for address table: " + atrt->getName());
                }
                
                output << " file \"" << path << "\"";
            }

            output << endl;
            continue;
        }
        output << "{ ";
        for (FWObject::iterator i=grp->begin(); i!=grp->end(); i++)
        {
            if (i!=grp->begin())  output << ", ";
            FWObject *o = FWReference::getObject(*i);
            if (o==nullptr) compiler->abort("broken table object ");

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=nullptr)
            {
                if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
                {
                    output <<  atrt->getSourceName() << " ";
                }
                if (atrt->getSubstitutionTypeName()==AttachedNetworks::TYPENAME)
                {
                    output << atrt->getSourceName() << ":network ";
                }
            } else
            {
                if (Interface::cast(o))
                {
                    output << o->getName();
                } else
                {
                    Address *A=Address::cast( o );
                    if (A==nullptr)
                        compiler->abort("table object must be an address: '" +
                                          o->getTypeName()+"'");

                    const InetAddr *addr = A->getAddressPtr();
                    InetAddr mask = *(A->getNetmaskPtr());

                    if (A->dimension()==1)
                    {
                        mask = InetAddr(InetAddr::getAllOnes());
                    }

                    output << addr->toString();
                    if (!mask.isHostMask())
                    {
                        output << "/" << mask.getLength();
                    }
                }
            }
            output << " ";
        }
        output << "} ";
        output << endl;
    }
    output << endl;
    return output.str();
}
