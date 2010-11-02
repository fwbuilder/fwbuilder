/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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

#ifndef  __FWOBJECTPROPERTIESFACTORY_H
#define  __FWOBJECTPROPERTIESFACTORY_H

#include <qstring.h>

namespace libfwbuilder {
    class FWObject;
    class PolicyRule;
    class Rule;
}

class FWObjectPropertiesFactory {

 public:

/**
 * returns a one line property of the object for the second column of
 * the tree view
 */
    static QString getObjectPropertiesBrief(libfwbuilder::FWObject *obj);

    static QString getObjectProperties(libfwbuilder::FWObject *obj);

    static QString getObjectPropertiesDetailed(libfwbuilder::FWObject *obj,
                                               bool showPath=false,
                                               bool tooltip=false,
                                               bool accentName=true,
                                               bool richText=true);
    static QString getRuleActionProperties(libfwbuilder::Rule *rule); 
    static QString getRuleActionPropertiesRich(libfwbuilder::Rule *rule); 
    static QString getPolicyRuleOptions(libfwbuilder::Rule *rule);
    static QString getNATRuleOptions(libfwbuilder::Rule *rule);

    static QString stripHTML(const QString &str);

    static QString getInterfaceNameExamplesForHostOS(const QString &host_os);
};

#endif

