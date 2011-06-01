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

#include "IfconfigImporter.h"

#include "../parsers/IfconfigCfgLexer.hpp"
#include "../parsers/IfconfigCfgParser.hpp"


#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include <QtDebug>

extern int fwbdebug;

using namespace std;
using namespace libfwbuilder;


IfconfigImporter::IfconfigImporter(FWObject *lib,
                                   std::istringstream &input,
                                   Logger *log,
                                   const std::string &fwname) :
    Importer(lib, "", input, log, fwname)
{
}

IfconfigImporter::~IfconfigImporter()
{
}

void IfconfigImporter::newInterface(InterfaceSpec &is)
{
    interfaces.push_back(is);
}

void IfconfigImporter::HwAddressForCurrentInterface(const std::string &str)
{
    list<InterfaceSpec>::reverse_iterator last = interfaces.rbegin();
    last->hwaddr = str;
}

void IfconfigImporter::inetConfigurationForCurrentInterface(AddressSpec &as)
{
    list<InterfaceSpec>::reverse_iterator last = interfaces.rbegin();
    last->as.push_back(as);
}

void IfconfigImporter::inet6ConfigurationForCurrentInterface(AddressSpec &as)
{
    list<InterfaceSpec>::reverse_iterator last = interfaces.rbegin();
    last->as.push_back(as);
}
    
void IfconfigImporter::addGroupToCurrentInterface(const std::string &group_name)
{
    list<InterfaceSpec>::reverse_iterator last = interfaces.rbegin();
    last->groups.push_back(group_name);
}

Firewall* IfconfigImporter::finalize()
{
    return NULL;
}

void IfconfigImporter::run()
{
    QStringList err;
    QString parser_err = QObject::tr("Parser error:");
    QString gen_err = QObject::tr("Error:");
    std::ostringstream parser_debug;

    IfconfigCfgLexer lexer(input);
    IfconfigCfgParser parser(lexer);
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

    if (!err.isEmpty())
        *logger << err.join("\n").toUtf8().constData();
}


