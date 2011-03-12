/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include "config.h"

#include "TableFactory.h"

#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>

#include <assert.h>
#include <sstream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

TableFactory::TableFactory(BaseCompiler *comp, Library *persistent_objects)
{
    compiler = comp;
    ruleSetName = "";
    dbroot = NULL;
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
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        lids.push_back(FWObjectDatabase::getStringId(o->getId()));
    }
    lids.sort();
    joinIDs R = for_each(lids.begin(), lids.end(), joinIDs("_"));
    return R.out;
}

void TableFactory::registerTable(const string& tblname, const string& tblid,
                                 FWObject* tbl) throw(FWException)
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
}

void TableFactory::createTablesForRE(RuleElement *re,Rule *rule)
{
    // sanity checks
    assert(rule->getRoot()==re->getRoot());
    assert(dbroot==rule->getRoot());

    /*
     * Before we create a new table, we scan tables and try to find
     * the one that already exists and contains the same objects.
     */
    string tblID = generateTblID(re);
    FWObject *tblgrp = NULL;

    if (tables.count(tblID)!=0)
    {
        tblgrp = tables[tblID];
    } else
    {
        tblgrp = dbroot->createObjectGroup();
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

        tblgrp->setName( tblname.str() );
        tblgrp->setId(FWObjectDatabase::generateUniqueId()); //  "id_" + tblname.str() );

        persistent_tables->add(tblgrp,false);
        dbroot->addToIndex(tblgrp);

        tblgrp->setBool("pf_table",true);
        tblgrp->setStr("pf_table_id",tblID);

        registerTable(tblname.str(),tblID,tblgrp);

        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o = FWReference::getObject(*i);
            tblgrp->addRef( o );
        }
    }
    re->clearChildren();
    re->addRef(tblgrp);
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
        if (atrt!=NULL &&
            atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
        {
            output << "persist";
            if ( !atrt->getSourceName().empty() )
                output << " file \""
                       << atrt->getSourceName()
                       << "\"";

            output << endl;
            continue;
        }
        output << "{ ";
        for (FWObject::iterator i=grp->begin(); i!=grp->end(); i++)
        {
            if (i!=grp->begin())  output << ", ";
            FWObject *o = FWReference::getObject(*i);
            if (o==NULL) compiler->abort("broken table object ");

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=NULL)
            {
                if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
                {
                    output <<  atrt->getSourceName() << " ";
                }
            } else
            {
                if (Interface::cast(o))
                {
                    output << o->getName();
                } else
                {
                    Address *A=Address::cast( o );
                    if (A==NULL)
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
