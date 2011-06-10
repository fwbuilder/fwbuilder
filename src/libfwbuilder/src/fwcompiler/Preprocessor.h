/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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

  ######################################################################

  Class Preprocessor performs various operations on the original copy
  of the object database before other compilers are called to process
  rules. All other cmpilers create a local copy copy of the database
  and work with it, but Preprocessor works with the original
  database. Currently Preprocessor expands MultiAddress
  objects. Preprocessor can be overloaded as any other Compiler with
  customizations added to its prolog() or compile() methods.

*/

#ifndef __PREPROCESSOR_HH__
#define __PREPROCESSOR_HH__

#include "fwcompiler/Compiler.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <string>

namespace fwcompiler {

    class Preprocessor : public Compiler {

        void findMultiAddressObjectsUsedInRules(libfwbuilder::FWObject *top);

public:
	virtual std::string myPlatformName();

	virtual ~Preprocessor();
	Preprocessor(libfwbuilder::FWObjectDatabase *_db,
                     libfwbuilder::Firewall *fw, bool ipv6_policy);

        /**
         * converts a single object. convertObjects calls this method for every
         * object that may need to be converted
         */
        virtual void convertObject(libfwbuilder::FWObject *obj);

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();
    };
}

#endif
