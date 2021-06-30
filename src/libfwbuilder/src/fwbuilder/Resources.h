/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef _RESOURCES_HH
#define _RESOURCES_HH


#include <map>
#include <string>
#include <list>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <vector>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"

namespace libfwbuilder {
    class Host;
    class Firewall;
    class Interface;
    class FWOptions;
};

class Resources
{
    xmlDocPtr   doc;
    xmlNodePtr  root;
    std::string resfile;

    static const std::string PLATFORM_RES_DIR_NAME;
    static const std::string OS_RES_DIR_NAME;

    std::string getXmlNodeContent(xmlNodePtr node);
    std::string getXmlNodeProp(xmlNodePtr node,std::string prop);

    void loadRes(const std::string &rfile );


public:

    Resources();
    Resources(const std::string &resF);
    ~Resources();
    
    void clear();
    
    void loadSystemResources();

    xmlNodePtr getXmlNode(const std::string& path);

    std::string  getIconPath(const char*   icon);
    std::string  getIconPath(const std::string& icon);

    std::string  getResourceStr(const std::string& resource_path);
    int          getResourceInt(const std::string& resource_path);
    bool         getResourceBool(const std::string& resource_path);
    void         getResourceStrList(const std::string& resource_path,
                                    std::list<std::string> &res);

    bool         getObjResourceBool(const libfwbuilder::FWObject *obj,
                                    const std::string& resource_name);

    std::string  getObjResourceStr(const libfwbuilder::FWObject *obj,
                                   const std::string& resource_name);

    std::string  getRuleElementResourceStr(const std::string &rel,
                                           const std::string &resource);

    std::string  getVersion();
    std::string  getCompiler();
    std::string  getInstaller();
    std::string  getTransferAgent();

    static std::vector<std::string>             getListOfPlatforms();
    static std::map<std::string,std::string>    getPlatforms();
    static std::map<std::string,std::string>    getOS();

    /**
     *  sets value of _one_ option using default value from resources
     */
    void    setDefaultOption(libfwbuilder::FWObject *o,
                             const std::string &xml_node);

    /**
     *  sets values of _all_ options using default values from
     *  resources
     */
    void    setDefaultOptionsAll(libfwbuilder::FWObject *o,
                                 const std::string &xml_node);

    /**
     *  This method sets default values to objects's properties (or
     *  attributes) defined in DTD. Values are found in global
     *  resources.
     */
    static void    setDefaultProperties(libfwbuilder::FWObject *o);

    /**
     *  This method sets default values to the platform-specific host
     *  or firewall options. Values are found in global resources. Only
     *  class Host and its descendants have child element Options, hence
     *  limitation on the parameter type.
     */
    static void    setDefaultOptions(libfwbuilder::Host *o);

    /**
     *  This method sets default values to the platform-specific
     *  firewall or OS-specific host_OS options. 
     */
    static void    setDefaultTargetOptions(const std::string &target,
                                           libfwbuilder::Firewall *o);

    /**
     *  This method sets default values to the platform-specific interface
     *  or OS-specific interface options.
     */
    static void    setDefaultIfaceOptions(const std::string &target,
                                          libfwbuilder::Interface *iface);

    /**
     * returns string value of target's capability
     * 'cap_name'. Capabilities are stored in platform-specific xml files
     * in subtree "/FWBuilderResources/Target/capabilities"
     */
    static std::string getTargetCapabilityStr(const std::string &target,
                                              const std::string &cap_name);
    static bool        getTargetCapabilityBool(const std::string &target,
                                               const std::string &cap_name);

    /**
     * returns string value of target's option
     * 'opt_name'. Options are stored in platform-specific xml files
     * in subtree "/FWBuilderResources/Target/options"
     */
    static std::string getTargetOptionStr(const std::string &target,
                                          const std::string &opt_name);
    static bool        getTargetOptionBool(const std::string &target,
                                           const std::string &opt_name);
    static bool    isTargetActionSupported(const std::string &target,
                                           const std::string &action);
    static std::string    getActionEditor (const std::string &target,
                                           const std::string &action);

    static bool isSystem(const libfwbuilder::FWObject *o);
    static std::string getIconFileName(const libfwbuilder::FWObject *o);
    static std::string getNegIconFileName(const libfwbuilder::FWObject *o);
    static std::string getRefIconFileName(const libfwbuilder::FWObject *o);
    static std::string getTreeIconFileName(const libfwbuilder::FWObject *o);

    static Resources                        *global_res;
    static std::map<std::string,Resources*>  platform_res;
    static std::map<std::string,Resources*>  os_res;
};


#endif


