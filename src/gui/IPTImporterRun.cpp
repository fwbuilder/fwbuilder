/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"

#include "IPTImporter.h"

#include <qstring.h>

#include <ios>
#include <iostream>
#include <algorithm>

#ifdef HAVE_ANTLR_RUNTIME
#include <antlr/ANTLRException.hpp>

// parser and lexer for files produced by iptables-save
#include "../parsers/IPTCfgLexer.hpp"
#include "../parsers/IPTCfgParser.hpp"

#endif

/*
 * Only this module depends on IPTCfgLexer and IPTCfgParser,
 * so only this file is recompiled when we change grammar
 */

void IPTImporter::run()
{
#ifdef HAVE_ANTLR_RUNTIME
// it is probably safer to create an empty firewall if we do not have
// ANTLR on the system rather than try to #ifdef out chunks of code
// here and there in this module
//
// Obviously we should disable GUI elements that activate this importer
// if ANTLR runtime is not available.
//

    std::string err;
    std::ostringstream parser_debug;

    IPTCfgLexer lexer(input);
    IPTCfgParser parser(lexer);
    parser.importer = this;
    if (fwbdebug)   parser.dbg = &std::cerr;
    else            parser.dbg = &parser_debug;

    try
    {
        current_ruleset = getUnidirRuleSet("Policy");  // creates if new

        parser.cfgfile();
    } catch(ANTLR_USE_NAMESPACE(antlr)ANTLRException &e)
    {
        err = e.toString().c_str();
    } catch(std::exception& e)
    {
        err = e.what() ;
    }

    if (!err.empty())  throw ImporterException(err);

#endif
}

