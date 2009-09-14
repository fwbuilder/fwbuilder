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

#include <assert.h>

#include "BaseCompiler.h"

#include "fwbuilder/libfwbuilder-config.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>

 
using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


FWCompilerException::FWCompilerException(Rule *r, const string &err) : FWException(err)
{
    rule=r;
}


bool BaseCompiler::haveErrorsAndWarnings()
{
    return (int(errors_buffer.tellp()) > 0);
}

string BaseCompiler::getErrors(const string &comment_sep)
{
    ostringstream ostr;
    istringstream istr(errors_buffer.str());
    while (!istr.eof())
    {
        string tmpstr;
        getline(istr, tmpstr);
        if (tmpstr.length())
            ostr << comment_sep << tmpstr << endl;
    }
    return ostr.str();
}

void BaseCompiler::clearErrors()
{
    errors_buffer.str("");
}

/*
 * Error and warning format:
 *
 *  fw-object-name:ruleset-name:rule-number: message 
 */
string BaseCompiler::stdErrorMessage(FWObject *fw,
                                     FWObject *ruleset,
                                     FWObject *rule,
                                     const std::string &errstr)
{
    ostringstream tmpstr;

    if (fw) tmpstr << fw->getName();
    tmpstr << ":";
    if (ruleset) tmpstr << ruleset->getName();
    tmpstr << ":";
    if (rule && Rule::cast(rule)) tmpstr << Rule::cast(rule)->getPosition();
    tmpstr << ": ";
    tmpstr << level_macro << ": ";
    tmpstr << errstr;
    return tmpstr.str();
}

void BaseCompiler::printError(const string &level, const string &errstr)
{
    string str = errstr;
    while (str.at(str.length() - 1) == '\n') str = str.substr(0, str.length() - 1);
    size_t n = str.find(level_macro);
    if (n != string::npos) str.replace(n, level_macro.length(), level);
    cout << flush;
    cerr << str << endl;
    errors_buffer << str << endl;
}

void BaseCompiler::abort(const string &errstr) throw(FWException)
{
    printError("error", errstr);
    if (!test_mode) throw FWException("Fatal error");
}

void BaseCompiler::error(const string &str)
{
    printError("error", str);
}

void BaseCompiler::warning(const string &str)
{
    printError("warning", str);
}

