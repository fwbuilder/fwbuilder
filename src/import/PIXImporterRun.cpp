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

#include "PIXImporter.h"

#include <QString>
#include <QStringList>

#include <ios>
#include <iostream>
#include <algorithm>

#include <antlr/ANTLRException.hpp>

#include "../parsers/PIXCfgLexer.hpp"
#include "../parsers/PIXCfgParser.hpp"

extern int fwbdebug;


/*
 * Only this module depends on PIXCfgLexer and PIXCfgParser,
 * so only this file is recompiled when we change grammar
 */

void PIXImporter::run()
{
// it is probably safer to create an empty firewall if we do not have
// ANTLR on the system rather than try to #ifdef out chunks of code
// here and there in this module
//
// Obviously we should disable GUI elements that activate this importer
// if ANTLR runtime is not available.
//

    QStringList err;
    QString parser_err = QObject::tr("Parser error:\n");
    std::ostringstream parser_debug;

    PIXCfgLexer lexer(input);
    PIXCfgParser parser(lexer);
    parser.importer = this;
    if (fwbdebug)   parser.dbg = &std::cerr;
    else            parser.dbg = &parser_debug;

    try
    {
        parser.cfgfile();
    } catch(ANTLR_USE_NAMESPACE(antlr)ANTLRException &e)
    {
        err << parser_err;
        err << e.toString().c_str();
    } catch(std::exception& e)
    {
        err << parser_err;
        err << e.what();
    }

    if (haveFirewallObject())
    {
        if (countInterfaces()==0) err << noInterfacesErrorMessage();
//        if (countRules()==0) err << noRulesErrorMessage();
        if (countRules()==0)
        {
            // temporary error message, replace with the standard one when
            // done
            err << QObject::tr(
                "\n\n"
                "Import of PIX and ASA policy and NAT rules is not "
                "implemented yet but is planned for fwbuilder v4.2.0 release. "
                "Please watch nightly builds on "
                "http://www.fwbuilder.org/nightly_builds/fwbuilder-4.2/current_build/ "
                "if you are interested in this feature."
                "\n\n"
            );

        }
    } else
    {
        err << parser_err;
        err << noFirewallErrorMessage();
        err << commonFailureErrorMessage();
    }

    if (!err.isEmpty())
        throw ImporterException(err.join("\n"));
}

