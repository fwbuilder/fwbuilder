/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

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

#include <assert.h>



#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Network.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace libfwbuilder;
using namespace std;

const char *MultiAddress::TYPENAME={"MultiAddress"};

MultiAddress::MultiAddress() : FWObject() 
{
    setRunTime(false);
}

MultiAddress::~MultiAddress()
{
}

string MultiAddress::getSourceName()
{
    return "";
}

void MultiAddress::setSourceName(const std::string&)
{
}

bool MultiAddress::isCompileTime() const
{
    return !getBool("run_time");
}

bool MultiAddress::isRunTime() const
{
    return getBool("run_time");
}

void MultiAddress::setCompileTime(const bool b)
{
    setBool("run_time",!b);
}

void MultiAddress::setRunTime(const bool b)
{
    setBool("run_time",b);
}

bool MultiAddress::validateChild(FWObject *o)
{ 
    return ObjectGroup::validateChild(o);
}

// ========================================================================

const char *MultiAddressRunTime::TYPENAME={"MultiAddressRunTime"};

MultiAddressRunTime::MultiAddressRunTime()
{
    setName("UnknownMultiAddressRunTime");
    source_name = "source";
    run_time = false;
    subst_type_name = "Unknown";
}

MultiAddressRunTime::MultiAddressRunTime(MultiAddress *maddr)
{
    setName(maddr->getName());
    source_name = maddr->getSourceName();
    run_time = maddr->isRunTime();
    subst_type_name = maddr->getTypeName();
}

string MultiAddressRunTime::getSourceNameAsPath(FWOptions *options) const
{
    string ret = source_name;
    size_t found = ret.find("%DATADIR%");
    if (found == string::npos) return ret;

    string dataDir = options->getStr("data_dir");
    if (dataDir.empty()) return dataDir;

    ret.replace(found, 9, dataDir);
    return ret;
}
