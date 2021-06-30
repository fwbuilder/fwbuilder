/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "interfacePropertiesObjectFactory.h"
#include "interfaceProperties.h"
#include "linux24Interfaces.h"
#include "nxosInterfaces.h"
#include "iosInterfaces.h"
#include "junosInterfaces.h"
#include "procurveInterfaces.h"
#include "openbsdInterfaces.h"
#include "freebsdInterfaces.h"
#include "pixInterfaces.h"
#include "procurveInterfaces.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Resources.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;

interfaceProperties* interfacePropertiesObjectFactory::getInterfacePropertiesObject(FWObject *fw)
{
    Resources* os_res = Resources::os_res[fw->getStr("host_OS")];
    string os_family = fw->getStr("host_OS");
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");
    return getInterfacePropertiesObject(os_family);
}

interfaceProperties* interfacePropertiesObjectFactory::getInterfacePropertiesObject(
    const std::string &os_family)
{
    if (os_family == "linux24" ||
        os_family == "openwrt" ||
        os_family == "dd-wrt-nvram" ||
        os_family == "dd-wrt-jffs" ||
        os_family == "secuwall") return new linux24Interfaces();

    if (os_family == "nxos") return new nxosInterfaces();

    if (os_family == "ios") return new iosInterfaces();

    if (os_family == "junos") return new junosInterfaces();

    if (os_family == "pix_os" || os_family == "ios") return new pixInterfaces();

    if (os_family == "openbsd") return new openbsdInterfaces();
    if (os_family == "freebsd") return new freebsdInterfaces();

    if (os_family == "procurve") return new procurveInterfaces();

    // by default return object of the base class. It performs some
    // reasonable default actions.
    return new interfaceProperties();
}
 
