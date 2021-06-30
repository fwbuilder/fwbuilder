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


#include "PIXImporter.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QtDebug>

#include <ios>
#include <iostream>
#include <algorithm>

#include <antlr/ANTLRException.hpp>

#include "../parsers/PIXCfgLexer.hpp"
#include "../parsers/PIXCfgParser.hpp"

extern int fwbdebug;

using namespace std;


/*
 * Only this module depends on PIXCfgLexer and PIXCfgParser,
 * so only this file is recompiled when we change grammar
 */

void PIXImporter::run()
{
    QStringList err;
    QString parser_err = QObject::tr("Parser error:");
    QString gen_err = QObject::tr("Error:");
    std::ostringstream parser_debug;

/* Do a bit of preprocessing of the input to simplify crazy grammar. 
 *
 * Do the following (will add more stuff here in the future):
 *
 * - process "names" section: isolate "name" commands and build
 *   dictionary of names and addresses, then scan input file and
 *   replace names with addresses everywhere.
 */

    QMap<QString, QString> named_addresses;
    QStringList whole_input;

    input.seekg (0, ios::beg);
    char buf[8192];
    while (!input.eof())
    {
        input.getline(buf, sizeof(buf)-1);
        whole_input.append(QString(buf));
    }

    foreach(QString str, whole_input)
    {
        if (str.startsWith("name "))
        {
            QStringList items = str.split(" ");
            named_addresses[items[2]] = items[1];
        }
    }

    QStringList normalized_input_buffer;

    foreach(QString str, whole_input)
    {
        if ( ! str.startsWith("name "))
        {
            QMap<QString, QString>::iterator it;
            for (it=named_addresses.begin(); it!=named_addresses.end(); ++it)
            {
                QString re("\\b%1\\b");
                str.replace(QRegExp(re.arg(it.key())), it.value());
            }
        }

        normalized_input_buffer.append(str);
    }

    istringstream  normalized_input(
        normalized_input_buffer.join("\n").toStdString());

    PIXCfgLexer lexer(normalized_input);
    PIXCfgParser parser(lexer);
    parser.importer = this;
    if (fwbdebug)   parser.dbg = &std::cerr;
    else            parser.dbg = &parser_debug;

    try
    {
        parser.cfgfile();
    } catch(ANTLR_USE_NAMESPACE(antlr)ANTLRException &e)
    {
        err << parser_err + " " + e.toString().c_str();
    } catch(ObjectMakerException &e)
    {
        err << gen_err + " " + e.toString();
    } catch(ImporterException &e)
    {
        err << gen_err + " " + e.toString();
    } catch(std::exception& e)
    {
        err << parser_err + " " + e.what();
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
        *logger << err.join("\n").toUtf8().constData();
}

