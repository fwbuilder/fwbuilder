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


#include "OSConfigurator_pix_os.h"
#include "Helper.h"
#include "fwbuilder/Resources.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

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
 *     Generating old school fixup commands for PIX 6.X
 *     and FWSM 2.3
 *
 * ********************************************************************/


/*
 * Copy this method into class PIXAdvancedFWDialog in fwbuilder/src/pix
 */
string OSConfigurator_pix_os::_printFixupCommand(const string &fixup_name,
                                                 const string &sw,
                                                 int   arg1, 
                                                 int   arg2,
                                                 bool  ov)
{
    ostringstream res;

    if (sw=="0")
    {
        if (fixup_name=="dns")
        {
            if (arg1)
            {
                res << "fixup protocol " << fixup_name;
                res << " maximum-length " << arg1;
                res << endl;
            }
        } else
        {
            if (fixup_name=="ftp")
            {
                if (arg1)
                {
                    res << "fixup protocol " << fixup_name << " ";
                    if (ov) res << "strict ";
                    res << arg1;
                    res << endl;
                }
            } else
            {
                if (fixup_name=="mgcp")
                {
                    if (arg1)
                    {
                        res << "fixup protocol " << fixup_name << " ";
                        res << arg1;
                        res << endl;
                    }
                    if (arg2) 
                    {
                        res << "fixup protocol " << fixup_name << " ";
                        res << arg2;
                        res << endl;
                    }
                } else 
                {
                    res << "fixup protocol " << fixup_name << " ";
                    if (arg1) res << arg1;
                    if (arg2 && arg1!=arg2) res << "-" << arg2;
                    res << endl;
                }
            }
        }
    }
    if (sw=="1")
    {
        res << "no fixup protocol " << fixup_name;
        res << endl;
    }

    return res.str();
}

string OSConfigurator_pix_os::_printFixups()
{
    ostringstream  res;
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");
    FWOptions *options = fw->getOptionsObject();
    assert(options!=nullptr);

    string lst = Resources::platform_res[platform]->getResourceStr(
            "/FWBuilderResources/Target/options/version_" + version +
            "/fixups/list");

    string::size_type i,j, k;
    i=0;
    while ( i<lst.size() )
    {
        j=lst.find(",",i);
        string fixup_xml_element=lst.substr(i,j-i);

        i=j+1;

        string f=options->getStr(fixup_xml_element);

        if (!f.empty())
        {
            string fixup_name=fixup_xml_element.substr(0, fixup_xml_element.find("_fixup") );

            while ( (k=fixup_name.find("_"))!=string::npos ) 
                fixup_name.replace(k,1,1,' ');

            string sw;
            int    arg1,arg2;
            string on;
            bool   ov;
            istringstream str(f);

            str >> sw >> arg1 >> arg2 >> on >> ov;

            res << _printFixupCommand(fixup_name, sw, arg1, arg2, ov );
        }
        if (j==string::npos) break;
    }

    return res.str();
}




string OSConfigurator_pix_os::getProtocolInspectionCommands()
{
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");

    ostringstream res;

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_" + version +
            "/fixups/use_fixup_commands"))
        res << _printFixups();

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_" + version +
            "/fixups/use_mpf_policy_map"))
        res << _printMPFPolicyMap();

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_" + version +
            "/fixups/use_policy_map_type_inspect"))
        res << _printPolicyMapTypeInspect();

    return res.str();
}
