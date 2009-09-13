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
        ostr << comment_sep << tmpstr << endl;
    }
    errors_buffer.str("");
    return ostr.str();
}


void BaseCompiler::abort(const string &errstr) throw(FWException)
{
    if (test_mode)
        error(errstr);
    else
        throw FWException(errstr);
}

void BaseCompiler::error(const string &errstr)
{
    cout << flush;
    cerr << "Error: ";
    cerr << errstr << endl;
    errors_buffer << "Error: ";
    errors_buffer << errstr << endl;
}

void BaseCompiler::warning(const string &warnstr)
{
    cout << flush;
    cerr << "Warning: ";
    cerr << warnstr << endl;
    errors_buffer << "Warning: ";
    errors_buffer << warnstr << endl;
}

