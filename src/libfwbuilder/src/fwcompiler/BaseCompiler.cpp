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
    rule_errors.clear();
}

string BaseCompiler::getErrorsForRule(Rule *rule, const std::string &comment_sep)
{
    string rule_label = rule->getLabel();
    rule_errors[rule_label].sort();
    ostringstream ostr;
    list<string>::iterator it;
    string prev;  // used to remove duplicate messages
    for (it=rule_errors[rule_label].begin(); it!=rule_errors[rule_label].end(); ++it)
    {
        if (*it != prev) ostr << comment_sep << *it << endl;
        prev = *it;
    }
    return ostr.str();
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
    }
    if (ruleset_owner.empty() && fw) ruleset_owner = fw->getName();

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
    Rule *cast_rule = Rule::cast(rule);
    if (cast_rule)
    {
        cast_rule->setCompilerMessage(str);
        rule_errors[cast_rule->getLabel()].push_back(str);
    }
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

/*
 * Note that when this code runs as part of the command line compiler
 * rather than as a single-rule compile function inside the GUI (that
 * is, it is not in embedded mode) and if test mode was activated
 * (usually via command line flag "-xt"), then abort() behaves as
 * error(), that is, it prints error message but does not terminate
 * the process but just returns. In embedded mode it always throws
 * exception to stop compiling
 */
void BaseCompiler::abort(const string &errstr)
{
    printError(errstr);
    if (inEmbeddedMode())
        throw FatalErrorInSingleRuleCompileMode(errors_buffer.str());
    status = FWCOMPILER_ERROR;
    if (test_mode) return;
    throw FWException("Fatal error");
}

void BaseCompiler::abort(FWObject *fw,
                         FWObject *ruleset,
                         FWObject *rule,
                         const string &errstr)
{
    message("error", fw, ruleset, rule, errstr);
    if (inEmbeddedMode())
        throw FatalErrorInSingleRuleCompileMode(errors_buffer.str());
    status = FWCOMPILER_ERROR;
    if (test_mode) return;
    throw FWException("Fatal error");
}

void BaseCompiler::error(const string &str)
{
    status = FWCOMPILER_ERROR;
    printError(str);
}

void BaseCompiler::error(FWObject *fw,
                         FWObject *ruleset,
                         FWObject *rule,
                         const string &errstr)
{
    status = FWCOMPILER_ERROR;
    message("error", fw, ruleset, rule, errstr);
}

void BaseCompiler::warning(const string &str)
{
    status = FWCOMPILER_WARNING;
    printError(str);
}

void BaseCompiler::warning(FWObject *fw,
                           FWObject *ruleset,
                           FWObject *rule,
                           const string &errstr)
{
    status = FWCOMPILER_WARNING;
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

