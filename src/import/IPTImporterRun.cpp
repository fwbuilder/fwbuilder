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

#include "IPTImporter.h"

#include <QString>
#include <QStringList>

#include <ios>
#include <iostream>
#include <algorithm>

#include <antlr/ANTLRException.hpp>

// parser and lexer for files produced by iptables-save
#include "../parsers/IPTCfgLexer.hpp"
#include "../parsers/IPTCfgParser.hpp"

extern int fwbdebug;

using namespace std;

/*
 * Only this module depends on IPTCfgLexer and IPTCfgParser,
 * so only this file is recompiled when we change grammar
 */

void IPTImporter::run()
{
// it is probably safer to create an empty firewall if we do not have
// ANTLR on the system rather than try to #ifdef out chunks of code
// here and there in this module
//
// Obviously we should disable GUI elements that activate this importer
// if ANTLR runtime is not available.
//

    QStringList err;
    ostringstream parser_debug;

/* Do a bit of preprocessing of the input to simplify crazy grammar. String
 * operations are easier to do with Qt QString class.
 *
 * Do the following (will add more stuff here in the future):
 *
 *  - normalize parameters for the multiport module. Multiport accepts
 *    parameters --source-ports and --sport and aparently in the older versions
 *    --sports. Unfortunayely the same parameter "--sport" is used
 *    to do port match with module tcp, udp and several others. Even though the
 *    name of the parameter is the same, port specification can follow different
 *    rules. For example, for multiport we can have port1[,port2], while for
 *    modules tcp and udp it is port1[:port2]. This makes grammar difficult
 *    to write. Need to convert parameters to the unique long form before
 *    passing script to antlr
 */
    input.seekg (0, ios::end);
    size_t input_size = input.tellg();
    string normalized_input_buffer;
    normalized_input_buffer.reserve(input_size);

    input.seekg (0, ios::beg);
    char buf[8192];
    while (!input.eof())
    {
        input.getline(buf, sizeof(buf)-1);

        QString str(buf);
        if (str.contains("-m multiport"))
        {
            str.replace("--sports", "--source-ports");
            str.replace("--sport", "--source-ports");
            str.replace("--dports", "--destination-ports");
            str.replace("--dport", "--destination-ports");
        }

        normalized_input_buffer.append(str.toStdString());
        normalized_input_buffer.append("\n");
    }

    assert(normalized_input_buffer.length() > 0);

    istringstream  normalized_input(normalized_input_buffer);

    IPTCfgLexer lexer(normalized_input);
    IPTCfgParser parser(lexer);
    parser.importer = this;
    if (fwbdebug)   parser.dbg = &std::cerr;
    else            parser.dbg = &parser_debug;

    QString parser_err = QObject::tr("Parser error:");
    QString gen_err = QObject::tr("Error:");

    try
    {
        parser.cfgfile();
    } catch(ANTLR_USE_NAMESPACE(antlr)ANTLRException &e)
    {
        err << parser_err;
        err << e.toString().c_str();
    } catch(ObjectMakerException &e)
    {
        err << gen_err;
        err << e.toString();
    } catch(ImporterException &e)
    {
        err << gen_err;
        err << e.toString();
    } catch(std::exception& e)
    {
        err << parser_err;
        err << e.what();
    }

    if (haveFirewallObject())
    {
        if (countInterfaces()==0) err << noInterfacesErrorMessage();
        if (countRules()==0) err << noRulesErrorMessage();
    } else
    {
        err << parser_err;
        err << noFirewallErrorMessage();
        err << commonFailureErrorMessage();
    }

    if (!err.isEmpty())
        throw ImporterException(err.join("\n"));
}

