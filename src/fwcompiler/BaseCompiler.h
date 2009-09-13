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

#ifndef __BASE_COMPILER_HH__
#define __BASE_COMPILER_HH__

#include <fwbuilder/libfwbuilder-config.h>
#include "fwbuilder/FWException.h"
#include "fwbuilder/Rule.h"

#include <sstream>

namespace fwcompiler {

    class FWCompilerException : public libfwbuilder::FWException {
	libfwbuilder::Rule *rule;
	public:
	FWCompilerException(libfwbuilder::Rule *r,const std::string &err);
	libfwbuilder::Rule *getRule() const { return rule; }
    };

    class BaseCompiler {

	std::stringstream errors_buffer;
        bool test_mode;

public:

        void setTestMode() { test_mode = true; }
        bool inTestMode() { return test_mode; }

        /**
         * prints error message and aborts the program. If compiler is
         * in testing mode (flag test_mode==true), then just prints
         * the error message and returns.
         */
	void abort(const std::string &errstr) throw(libfwbuilder::FWException);

        /**
         * prints an error message and returns
         */
	void error(const std::string &warnstr);

        /**
         * prints warning message
         */
	void warning(const std::string &warnstr);

	virtual ~BaseCompiler() {};

	BaseCompiler() {test_mode = false;};

	std::string getErrors(const std::string &comment_sep);
	bool haveErrorsAndWarnings();
    };
}

#endif
