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


#ifndef  __LIBFWBUILDER_CONSTANTS_HH__
#define  __LIBFWBUILDER_CONSTANTS_HH__

#include <string>

namespace libfwbuilder
{

/**
 * This class represents various information 
 * about library as well as some public
 * constants.
 */
class Constants
{
    static std::string res_dir;

public:

    static void init(const std::string &app_root_dir);

    /**
     * Returns library's description
     */
    static const std::string getLibraryDescription();

    /**
     * Returns library version number
     */
    static const std::string getLibraryVersion();

    /**
     * Returns data format version number
     */
    static const std::string getDataFormatVersion();

    /**
     * Returns directory in which library stores it's data files.
     */
    static const std::string getTemplateDirectory();

    /**
     * Returns directory where we install XML DTD file
     */
    static std::string getDTDDirectory();

    /**
     * Returns directory where we install resources
     */
    static std::string getResourcesDirectory();

    /**
     * Returns full path to the resources file
     */
    static std::string getResourcesFilePath();

    /**
     * Returns full path to the standard objects library file
     */
    static std::string getStandardObjectsFilePath();

    /**
     * Returns full path to the prepackaged firewall template objects file
     */
    static std::string getTemplatesObjectsFilePath();

    /**
     * Returns full path to the directory where we install locale files
     */
    static std::string getLocaleDirectory();

    /**
      * Returns name of distro, determined dynamically
      */
    static std::string getDistro();

private:
    bool static fileExists(const std::string& file);
};

}

#endif
