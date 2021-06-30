/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#include <fstream>
#include <iostream>
#include <iomanip>

#include "CompilerDriver.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Rule.h"

#include "fwcompiler/Compiler.h"

#include <QString>
#include <QStringList>
#include <QMap>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


bool CompilerDriver::prepare(const QStringList &_args)
{
    args = _args;

    if (!configure(args)) return false;

    if (!single_rule_compile_on)
    {
        Firewall *fw = locateObject();
        if (fw == nullptr)
        {
            cerr << "Firewall or cluster object not found" << endl;
            return false;
        }
    }
    return true;
}

void CompilerDriver::compile()
{
    if (single_rule_compile_on)
    {
        QMapIterator<QString,QString> it(compileSingleRule(single_rule_id));
        while (it.hasNext())
        {
            it.next();
            info("Firewall " + it.key().toStdString());
            info(it.value().toStdString());
            info("\n");
        }
        return;
    }

    Firewall *fw = locateObject();
    if (Cluster::isA(fw))
    {
        commonChecks(fw);
        // compiling cluster. 
        list<Firewall*> members;
        Cluster::cast(fw)->getMembersList(members);
        for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
        {
            info("\n");
            info(" Firewall " + (*it)->getName() +
                 " member of cluster " + fw->getName());

            CompilerDriver *cl_driver = clone();
            cl_driver->configure(args);
            cl_driver->chDir();
            cl_driver->run(objdb->getStringId(fw->getId()),
                           objdb->getStringId((*it)->getId()),
                           "");
            // if (cl_driver->status == BaseCompiler::FWCOMPILER_ERROR)
            // We need to always copy the status to make sure
            // FWCOMPILER_WARNING is passed through
            status = cl_driver->status;
            delete cl_driver;
        }
    }
    else
    {
        chDir();
        commonChecks(fw);
        run("", objdb->getStringId(fw->getId()), "");
    }
}

/*
 * Compile single rule and return generated code. Rule is defined by
 * its ID, this is sufficient to locate the rule, ruleset and firewall
 * objects. If ruleset belongs to a cluster, compile all members and
 * return code generated for all of them. Returned code is placed in
 * QMap where the key is member firewall name and value is generated
 * script. If the rule belongs to a firewall rather than a cluster,
 * returned QMap contains one item.
 */
QMap<QString,QString> CompilerDriver::compileSingleRule(const string &rule_id)
{
    Cluster *cluster = nullptr;
    Firewall *fw = nullptr;

    Rule *rule = Rule::cast(
        objdb->findInIndex(FWObjectDatabase::getIntId(rule_id)));
    if (rule==nullptr)
        throw FWException(string("Rule with ID=") + rule_id + " not found");

    FWObject *p = rule;
    while (p && Firewall::cast(p)==nullptr) p = p->getParent();

    if (Cluster::isA(p)) cluster = Cluster::cast(p);
    if (Firewall::isA(p)) fw = Firewall::cast(p);

    QMap<QString,QString> result;

    if (cluster)
    {
        commonChecks(cluster);

        list<Firewall*> members;
        Cluster::cast(cluster)->getMembersList(members);

        // this copy of CompilerDriver is not going to do any useful work and
        // does not need these.
        objdb->remove(persistent_objects, false);
        objdb->remove(workspace, false);

        for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
        {
            CompilerDriver *cl_driver = clone();
            cl_driver->single_rule_compile_on = true;
            if (inTestMode()) cl_driver->setTestMode();
            if (inEmbeddedMode()) cl_driver->setEmbeddedMode();
            result[(*it)->getName().c_str()] = 
                cl_driver->run(
                    objdb->getStringId(cluster->getId()),
                    objdb->getStringId((*it)->getId()),
                    rule_id);
            delete cl_driver;
        }
    } else
    {
        commonChecks(fw);
        single_rule_compile_on = true;
        result[fw->getName().c_str()] = 
            run("", objdb->getStringId(fw->getId()), rule_id);
    }
    return result;
}

