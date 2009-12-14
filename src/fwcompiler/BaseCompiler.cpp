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

    // TODO: (some day) get rid of the argument @fw and use attribute
    // ".ruleset_owner" instead. Set this attribute in the place where
    // we prepare rules for processing and copy them to the
    // queue. This way, when CompilerDriver prepares rules from the
    // cluster, it can override the same attribute to make error and
    // warning messges refer to correct object that really owns rule
    // sets.
    string ruleset_owner;
    if (ruleset)
    {
        ruleset_owner = ruleset->getStr(".ruleset_owner");
        if (ruleset_owner.empty() && fw) ruleset_owner = fw->getName();
    }

    tmpstr << ruleset_owner << ":";
    if (ruleset) tmpstr << ruleset->getName();
    tmpstr << ":";
    if (rule && Rule::cast(rule)) tmpstr << Rule::cast(rule)->getPosition();
    tmpstr << ": ";
    tmpstr << level_macro << ": ";
    tmpstr << errstr;
    return tmpstr.str();
}

string BaseCompiler::setLevel(const string &level, const string &errstr)
{
    string str = errstr;
    while (str.at(str.length() - 1) == '\n') str = str.substr(0, str.length() - 1);
    size_t n = str.find(level_macro);
    if (n != string::npos) str.replace(n, level_macro.length(), level);
    return str;
}

void BaseCompiler::message(const std::string &level,
                           FWObject *fw,
                           FWObject *ruleset,
                           FWObject *rule,
                           const string &errstr)
{
    string str = setLevel(level, stdErrorMessage(fw, ruleset, rule, errstr));
    printError(str);
    if (rule && Rule::cast(rule)) rule->setStr(".error_msg", str);
}

void BaseCompiler::printError(const string &errstr)
{
    if (!inEmbeddedMode())
    {
        cout << flush;
        cerr << errstr << endl;
    }
    errors_buffer << errstr << endl;
}

void BaseCompiler::abort(const string &errstr) throw(FWException)
{
    printError(errstr);
    if (!test_mode) throw FWException("Fatal error");
}

void BaseCompiler::abort(FWObject *fw,
                         FWObject *ruleset,
                         FWObject *rule,
                         const string &errstr) throw(FWException)
{
    message("error", fw, ruleset, rule, errstr);
    if (!test_mode) throw FWException("Fatal error");
}

void BaseCompiler::error(const string &str)
{
    printError(str);
}

void BaseCompiler::error(FWObject *fw,
                         FWObject *ruleset,
                         FWObject *rule,
                         const string &errstr)
{
    message("error", fw, ruleset, rule, errstr);
}

void BaseCompiler::warning(const string &str)
{
    printError(str);
}

void BaseCompiler::warning(FWObject *fw,
                           FWObject *ruleset,
                           FWObject *rule,
                           const string &errstr)
{
    message("warning", fw, ruleset, rule, errstr);
}

void BaseCompiler::info(const string &str)
{
    if (!inEmbeddedMode())
    {
        cout << str << endl << flush;
    }
}

void BaseCompiler::errorRegExp(std::list<std::string> *err_regexp)
{
    err_regexp->clear();
    err_regexp->push_back("([^:]*):([^:]*):.*[Ee]rror:");
    err_regexp->push_back("(Error(:| )[^\n]*)");
}

void BaseCompiler::warningRegExp(std::list<std::string> *warn_regexp)
{
    warn_regexp->clear();
    warn_regexp->push_back("([^:]*):([^:]*):.*[Ww]arning:");
    warn_regexp->push_back("(Warning(:| )[^\n]*)");
}

