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


#ifndef  __PRINTINGCONTROLLER_HH_
#define  __PRINTINGCONTROLLER_HH_

#include "fwbuilder/FWObject.h"
#include "printerStream.h"
#include "ProjectPanel.h"

#include <QTableWidget>

#include <list>



class PrintingController
{
    printerStream *pr;

public:
    PrintingController(printerStream *_pr) { pr = _pr; }

    std::list<libfwbuilder::FWObject*> findAllUsedByType(
        std::list<libfwbuilder::FWObject*> &result, libfwbuilder::FWObject *obj,
        const std::string &type_name);
    int addObjectsToTable(std::list<libfwbuilder::FWObject*> &objects,
                          QTableWidget *tbl, int &row, int &col);
    bool addObjectsByTypeToTable(libfwbuilder::FWObject *parent,
                                 const std::string &type_name,
                                 QTableWidget *tbl, int &row, int &col);
    void findAllGroups(std::list<libfwbuilder::FWObject*> &objects,
                       std::list<libfwbuilder::FWObject*> &groups);
    void printRuleSet(libfwbuilder::FWObject *fw,
                      const std::string &ruleset_type_name,
                      ProjectPanel *project);
    void printFirewall(libfwbuilder::FWObject *fw, ProjectPanel *project);
    void printLegend(bool newPageForSection);
    void printObjects(libfwbuilder::FWObject *firewall_to_print,
                      bool newPageForSection);
    void configureQTableForPrint(QTableWidget *tbl);
};

#endif
