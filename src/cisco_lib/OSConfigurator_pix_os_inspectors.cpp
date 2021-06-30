/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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


#include "OSConfigurator_pix_os.h"
#include "inspectionProtocol.h"
#include "inspectionClassMap.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <QStringList>
#include <QtDebug>

#include <list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


/* ********************************************************************
 * 
 *     Generating class-map, class and match commands instead of fixups
 *     for PIX 7.0
 *
 * ********************************************************************/


string OSConfigurator_pix_os::_printMPFPolicyMap()
{
    ostringstream res;
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");
    string vers = "version_" + version;
    FWOptions *options = fw->getOptionsObject();
    assert(options!=nullptr);

    std::list<InspectionClassMap> defaultClassMaps;
    std::list<InspectionClassMap> customClassMaps;
    std::map<std::string,int>    DefaultInspectionInspectStatements;
    std::map<std::string,int>    CustomInspectionInspectStatements;


    QStringList allowed_fixups = 
        QString(Resources::platform_res[platform]->getResourceStr(
                    "/FWBuilderResources/Target/options/" + vers +
                    "/fixups/list").c_str()).split(",");

    defaultClassMaps.clear();
    customClassMaps.clear();
    DefaultInspectionInspectStatements.clear();
    CustomInspectionInspectStatements.clear();

    foreach (QString fixup_xml_element, allowed_fixups)
    {
        string f = options->getStr(fixup_xml_element.toLatin1().constData());

        if (!f.empty())
        {
            QString fixup_name = fixup_xml_element.replace("_fixup", "");

            int status;
            int p1,p2;
            string an;
            int av;
            istringstream str(f);

            str >> status >> p1 >> p2 >> an >> av;

/* We should really fix this in the GUI and pass max length parameter
 * as an/av rather than as port p1
 */
            if (fixup_name == "dns" && p1 != 0)
            {
                an = "maximum-length";
                av = p1;
                p1 = 53;
            }

            if (fixup_name.startsWith("ip_options"))
            {
                continue;
            }
            InspectionClassMap cm(fixup_name.toLatin1().constData(),
                                  status, p1, p2, an, av);
            if (cm.isDefault()) defaultClassMaps.push_back(cm);
            else                customClassMaps.push_back(cm);
        }
    }

    res << "class-map inspection_default" << endl;
    res << "  match default-inspection-traffic" << endl;
    res << endl;

    std::list<InspectionClassMap>::iterator i1;

    if (customClassMaps.size()>0)
    {
        for (i1=customClassMaps.begin(); i1!=customClassMaps.end(); i1++)
        {
            res << "class-map " << i1->class_map_name << endl;
            res << "  " << i1->getMatchCommand() << endl;
        }
        res << endl;
    }
    
    res << "policy-map global_policy" << endl;
    if (defaultClassMaps.size()>0)
    {
        res << "  class inspection_default" << endl;
        for (i1=defaultClassMaps.begin(); i1!=defaultClassMaps.end(); i1++)
        {
            string pn = i1->getPrintableName();
            if (i1->status!=FIXUP_SKIP && 
                DefaultInspectionInspectStatements[pn]!=1)
            {
                res << "    ";
                if (i1->status==FIXUP_DISABLE) res << "no ";
                res << "inspect " << pn << endl;
                DefaultInspectionInspectStatements[pn]=1;
            }
        }
    }
    if (customClassMaps.size()>0)
    {
        for (i1=customClassMaps.begin(); i1!=customClassMaps.end(); i1++)
        {
            string pn = i1->getPrintableName();
            if (i1->status!=FIXUP_SKIP &&
                CustomInspectionInspectStatements[pn]!=1)
            {
                res << "  class " << i1->class_map_name << endl;
                res << "    ";
                if (i1->status==FIXUP_DISABLE) res << "no ";
                res << "inspect " << i1->getPrintableName() << endl;
                CustomInspectionInspectStatements[pn]=1;
            }
        }
    }

    res << endl;
    res << "service-policy global_policy global" << endl;
    res << endl;

    return res.str();
}


