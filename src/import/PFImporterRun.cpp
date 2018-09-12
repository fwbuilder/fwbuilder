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


#include "fwbuilder/InetAddr.h"

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
using namespace libfwbuilder;


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

    QString whole_input = whole_input_tmp.join("\n") + "\n";
    QRegExp line_continuation("\\\\\\s*\n");
    whole_input.replace(line_continuation, "");

    QRegExp inline_comment("#.*$");
    QRegExp macro_definition("^\\s*(\\S+)\\s*=\\s*(.*)$");
    QRegExp list_of_items("^\\{\\s*((\\S+,?\\s*)+)\\s*\\}$");

    QMap<QString, QString> macros;
    QMap<QString, QString> macros_source_lines;

    foreach(QString str, whole_input.split("\n"))
    {
        QString work_str = str;
        work_str.replace(inline_comment, "");
        work_str = work_str.trimmed();

        if (macro_definition.indexIn(work_str) != -1)
        {
            QString macro_name = macro_definition.cap(1);
            QString value = macro_definition.cap(2);
            value.replace('\"', "");
            value = value.simplified();

            macros[macro_name] = value;
            macros_source_lines[macro_name] = macro_definition.cap(0);
        }
    }

    QMapIterator<QString, QString> it(macros);
    while (it.hasNext())
    {
        it.next();
        QString macro_name = it.key();
        QString value = it.value();
        substituteMacros(macros, value);
        macros[macro_name] = value;
    }

    it = macros;
    while (it.hasNext())
    {
        it.next();
        QString macro_name = it.key();
        QString value = it.value();

        qDebug() << "Macro: name=" << macro_name << "value=" << value;

        /*
         * Special case: if this macro defines list of addresses
         * in '{' '}', we convert it to a table with the same name
         * so that importer later on can create object group for
         * it.
         *
         * RegExp list_of_items assumes the string has been
         * stripped of any quotes and trimmed.
         */
        if (list_of_items.indexIn(value) != -1)
        {
            qDebug() << "This macro defines a list";

            /*
             * we only convert to table if the list contains at
             * least one ip address. We assume that if there is an
             * address there, then all items in the list must
             * represent addresses, host names or interface names
             * because pf does not allow mixed address/service
             * lists anywhere.
             */
            QString list_str = list_of_items.cap(1);
            list_str.replace(",", "");
            QStringList items = list_str.split(QRegExp("\\s"),
                                               QString::SkipEmptyParts);
            qDebug() << items;

            bool has_address = false;
            foreach(QString item, items)
            {
                qDebug() << "Item:" << item;
                if (!item.isEmpty() && (item.contains(':') || item.contains('.')))
                {
                    try
                    {
                        InetAddr(item.toStdString());
                        // stop the loop if string successfully
                        // converts to an ip address
                        has_address = true;
                        break;
                    } catch(FWException &ex)
                    {
                        ;
                    }
                }
            }

            if (has_address)
            {
                /*
                 * Convert as follows:
                 * Macro:
                 * name = "{ 1.1.1.1  2.2.2.2 }"
                 * to a table:
                 * table <name> "{ 1.1.1.1  2.2.2.2 }"
                 */
                QString table_def("table <%1> %2");
                whole_input.replace(macros_source_lines[macro_name],
                                    table_def.arg(macro_name).arg(value));
                /*
                 * And add a macro to the dictionary to map macro_name to
                 * the table
                 */
                macros[macro_name] = "<" + macro_name + ">";

                qDebug() << "Replacing macro definition with table:";
                qDebug() << table_def.arg(macro_name).arg(value);
            }
        }
    }

    if (fwbdebug)
        qDebug() << "Macros defined in this file: " << macros;

    substituteMacros(macros, whole_input);

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

    if (fwbdebug)
    {
        qDebug() << "haveFirewallObject()=" << haveFirewallObject()
                 << "countInterfaces()=" << countInterfaces()
                 << "countRules()=" << countRules();
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

void PFImporter::substituteMacros(const QMap<QString,QString> &macros,
                                  QString &buffer)
{
    // make several passes: sometimes macros can use other macros
    QRegExp any_macro_instance("\\$(\\w+)\\W");
    QSet<QString> undefined_macros;
    for (;;)
    {
        QMapIterator<QString, QString> it(macros);
        while (it.hasNext())
        {
            it.next();
            QString macro_name = it.key();
            QString macro_value = it.value();
            QRegExp macro_instance(QString("\\$%1(?=\\W)").arg(macro_name));

            buffer.replace(macro_instance, macro_value);
        }

        bool has_known_macros = false;
        int idx = 0;
        while ((idx = buffer.indexOf(any_macro_instance, idx)) != -1)
        {
            QString macro_name = any_macro_instance.cap(1);
            if (macros.contains(macro_name)) has_known_macros = true;
            else undefined_macros.insert(macro_name);
            idx++;
        }
        if (!has_known_macros) break;
    }

    foreach(QString macro_name, undefined_macros)
    {
        QString err;
        err = QObject::tr("Warning: Macro %1 is undefined").arg(macro_name);
        *logger << err.toUtf8().constData();
    }
}

