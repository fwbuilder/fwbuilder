/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: fwbedit.cpp 429 2008-07-31 07:03:39Z vadim $

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
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <deque>
#include <vector>
#include <string>

#include <assert.h>

// can't use 'DELETE' in this enum because it is degined somewhere on windows
typedef enum { NONE, ADDGRP, REMGRP, DELOBJECT, NEWOBJECT, MODOBJECT,
               LIST, STRUCT, UPGRADE, MERGE, IMPORT} command;

class OperandsError : public std::exception {};

// need to qualify deque even though we use "using namespace std;"
// to make it compile on windows
typedef std::deque<std::string> operands;

extern void listObject(libfwbuilder::FWObjectDatabase *objdb,
                       const std::string &path,
                       bool list_children,
                       bool recursive,
                       const std::string &list_format,
                       bool full_dump);

extern void newObject(libfwbuilder::FWObjectDatabase *objdb,
                      const std::string &objtype,
                      const std::string &name,
                      const std::string &comment_txt,
                      const std::string &parent,
                      operands &ops);

extern void delObject(libfwbuilder::FWObjectDatabase *objdb,
                      const std::string &object);

extern void modObject(libfwbuilder::FWObjectDatabase *objdb,
                      const std::string &object,
                      const std::string &comment_txt,
                      operands &ops);

extern void checkAndRepairTree(libfwbuilder::FWObjectDatabase *objdb);

extern void mergeTree(libfwbuilder::FWObjectDatabase *objdb,
                      const std::string &mergefile, int conflict_res);

extern void importConfig(const std::string &import_config,
                         libfwbuilder::FWObject *library,
                         const std::string &fw_name,
                         bool deduplicate);

extern int splitStr(char ch,std::string s, operands * ops);
extern std::string getNextOpt(operands &ops);
extern std::string fixPath(const std::string &obj_path);

extern void findObjects(const std::string &obj_path,
                        libfwbuilder::FWObject *obj,
                        std::list<libfwbuilder::FWObject*> &res);

extern bool getBool(std::string s);
extern void usage();


