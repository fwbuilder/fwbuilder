/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "PFImporter.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QtDebug>

#include <ios>
#include <iostream>
#include <algorithm>

#include <antlr/ANTLRException.hpp>

#include "../parsers/PFCfgLexer.hpp"
#include "../parsers/PFCfgParser.hpp"

extern int fwbdebug;

using namespace std;


/*
 * Only this module depends on PFCfgLexer and PFCfgParser,
 * so only this file is recompiled when we change grammar
 */

void PFImporter::run()
{
    QStringList err;
    QString parser_err = QObject::tr("Parser error:");
    QString gen_err = QObject::tr("Error:");
    std::ostringstream parser_debug;

/* Do a bit of preprocessing of the input to simplify crazy grammar. 
 *
 * Do the following (will add more stuff here in the future):
 *
 * - fold lines split with '\'
 * - find macro definitions and perform all macro sustitutions
 */

    QMap<QString, QString> named_addresses;
    QStringList whole_input_tmp;

    input.seekg (0, ios::beg);
    char buf[8192];
    while (!input.eof())
    {
        input.getline(buf, sizeof(buf)-1);
        whole_input_tmp.append(QString(buf));
    }

    QString whole_input = whole_input_tmp.join("\n");
    QRegExp line_continuation("\\\\\\s*\n");
    whole_input.replace(line_continuation, "");

    QRegExp macro_definition_1("^\\s*(\\S+)\\s*=\\s*\"(.*)\"$");
    QRegExp macro_definition_2("^\\s*(\\S+)\\s*=\\s*([^\"]*)$"); // no quotes
    QMap<QString, QString> macros;

    foreach(QString str, whole_input.split("\n"))
    {
        if (macro_definition_1.indexIn(str) != -1)
        {
            macros[macro_definition_1.cap(1)] = macro_definition_1.cap(2);
        }
        if (macro_definition_2.indexIn(str) != -1)
        {
            macros[macro_definition_2.cap(1)] = macro_definition_2.cap(2);
        }
    }

    if (fwbdebug)
        qDebug() << "Macros defined in this file: " << macros;

    // make several passes: sometimes macros can use other macros
    int pass = 0;
    while (1)
    {
        bool has_macros = false;
        QMapIterator<QString, QString> it(macros);
        while (it.hasNext())
        {
            it.next();
            QString macro_name = it.key();
            QString macro_value = it.value();
            if (whole_input.contains("$" + macro_name))
            {
                has_macros = true;
                whole_input.replace( "$" + macro_name, macro_value);
                if (fwbdebug)
                    qDebug() << "Pass " << pass
                             << "Macro substitution: "
                             << macro_name << ":" << macro_value;
            }
        }
        if (! has_macros) break;
        pass++;
    }

    if (fwbdebug)
    {
        qDebug() << "pf.conf file after line unfolding and macro substitution:";
        qDebug() << whole_input;
    }

    istringstream  normalized_input(whole_input.toStdString());

    PFCfgLexer lexer(normalized_input);
    PFCfgParser parser(lexer);
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

