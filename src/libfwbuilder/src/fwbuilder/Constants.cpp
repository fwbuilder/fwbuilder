/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Zaliva     lord@crocodile.org

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


#include "fwbuilder/Constants.h"

#if defined (__linux__) || defined (__FreeBSD_kernel__) || defined (__MINGW64__) || defined (__MINGW32__) || defined (__APPLE__)
#   include <sys/stat.h>
#endif

using namespace std;
using namespace libfwbuilder;

string Constants::res_dir;

void Constants::init(const std::string &app_root_dir)
{
    if (app_root_dir.empty()) res_dir = string(RES_DIR);
    else res_dir = app_root_dir + FS_SEPARATOR + string(RES_DIR);
}

const string Constants::getLibraryDescription()
{
    return "Firewall Builder API ";
}

const string Constants::getLibraryVersion() 
{ 
    return string(VERSION); 
}

const string Constants::getDataFormatVersion()
{
    return string(FWBUILDER_XML_VERSION); 
}

const string Constants::getTemplateDirectory() 
{ 
    return string(RES_DIR); 
}

string Constants::getDTDDirectory()
{
    if (res_dir.empty()) init("");
    return res_dir;
}

string Constants::getResourcesDirectory()
{
    if (res_dir.empty()) init("");
    return res_dir;
}

string Constants::getResourcesFilePath()
{
    return getResourcesDirectory() + FS_SEPARATOR + "resources.xml";
}

string Constants::getStandardObjectsFilePath()
{
    return getResourcesDirectory() + FS_SEPARATOR  + "objects_init.xml";
}

string Constants::getTemplatesObjectsFilePath()
{
    return getResourcesDirectory() + FS_SEPARATOR  + "templates.xml";
}

string Constants::getLocaleDirectory()
{
    return getResourcesDirectory() + "/locale";
}

string Constants::getDistro()
{
#if defined (__linux__)
    if (fileExists("/etc/debian_version"))
        return "Debian";
    if (fileExists("/etc/mandrake-release"))
        return "Mandrake";
    if (fileExists("/etc/slackware-version"))
        return "Slackware";
    if (fileExists("/etc/SuSE-release"))
        return "SuSE";
    if (fileExists("/etc/redhat-release"))
        return "RedHat";
    return "Unknown";
#elif defined (__FreeBSD_kernel__) && defined (__GLIBC__)
    if (fileExists("/etc/debian_version"))
        return "Debian";
    return "Unknown";
#else
    return "";
#endif
}

bool Constants::fileExists(const string &file)
{
    struct stat sb;
    return (stat(file.c_str(), &sb) == 0);
}

