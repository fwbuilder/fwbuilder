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
#include "Helper.h"
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
 *     Generating policy-map type inspect commands for PIX 8.0
 *
 * ********************************************************************/



string OSConfigurator_pix_os::_printPolicyMapTypeInspect()
{
    ostringstream res;
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");
    string vers = "version_" + version;
    FWOptions *options = fw->getOptionsObject();
    assert(options!=nullptr);

    // first, generate commands for ip-options

    QStringList allowed_fixups = 
        QString(Resources::platform_res[platform]->getResourceStr(
                    "/FWBuilderResources/Target/options/" + vers +
                    "/fixups/list").c_str()).split(",");

    list<InspectionClassMap> ip_options_matches;

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

            if (fixup_name.startsWith("ip_options") && status != FIXUP_SKIP)
            {
                InspectionClassMap cm(fixup_name.toLatin1().constData(),
                                      status, p1, p2, an, av);
                ip_options_matches.push_back(cm);
            }
        }
    }

    if (ip_options_matches.size() > 0)
    {
        res << "policy-map type inspect ip-options ip-options-map" << endl;
        res << "parameters" << endl;
        
        for(list<InspectionClassMap>::iterator i=ip_options_matches.begin();
            i!=ip_options_matches.end(); ++i)
        {

            switch (i->status)
            {
            case FIXUP_ENABLE:
                res << "  " << i->getPrintableName() << " action ";
                res << "allow" << endl;
                break;
            case FIXUP_CLEAR:
                res << "  "  << i->getPrintableName() << " action ";
                res << "clear" << endl;
                break;
            default:
                break;
            }
        }
    }

    res << endl;

    return res.str();
}


