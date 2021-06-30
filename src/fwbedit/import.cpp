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

#include "fwbuilder/Library.h"

#include "fwbedit.h"

#include "PreImport.h"
#include "IOSImporter.h"
#include "IPTImporter.h"
#include "PIXImporter.h"

#include <iostream>

#include <QFile>
#include <QString>


using namespace libfwbuilder;
using namespace std;


Logger& operator<<(Logger &logger, const QString &str)
{
    logger << str.toUtf8().constData();
    return logger;
}

void importConfig(const string &import_config,
                  FWObject *library,
                  const string &fw_name,
                  bool deduplicate)
{
    QFile f(QString::fromUtf8(import_config.c_str()));
    f.open(QFile::ReadOnly);
    string buffer = QString(f.readAll()).toStdString();
    f.close();

    std::istringstream instream(buffer);
    QueueLogger *logger = new QueueLogger();
    logger->copyToStderr();
    Importer* imp = nullptr;

    QStringList sl_buf = QString::fromUtf8(buffer.c_str()).split("\n");
    PreImport pi(&sl_buf);
    pi.scan();

    switch (pi.getPlatform())
    {
    case PreImport::PIX:
    case PreImport::FWSM:
        imp = new PIXImporter(library, instream, logger, fw_name);
        break;

    case PreImport::IOSACL:
        imp = new IOSImporter(library, instream, logger, fw_name);
        break;

    case PreImport::IPTABLES:
        imp = new IPTImporter(library, instream, logger, fw_name);
        break;

    case PreImport::IPTABLES_WITH_COUNTERS:
        cerr << "This appears to be iptables configuration saved using "
            "command \"iptables-save -c\""
            "and it includes packet counters. Please save configuration "
            "using command \"iptables-save\" without option \"-c\" and "
            "try to import it again."
             << endl;
        exit(1);

    default:
        cerr << "Can not import configuration file: unrecognized firewall platform"
             << endl;
        exit(1);
    }

    if (deduplicate) imp->prepareForDeduplication();
    
    try
    {
        imp->run();
    } catch(ImporterException &e)
    {
        *logger << e.toString() << "\n";
    } catch(ObjectMakerException &e)
    {
        *logger << e.toString() << "\n";
    }

    imp->finalize();

}

