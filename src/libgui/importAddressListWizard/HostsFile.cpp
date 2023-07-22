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


#include "HostsFile.h"

#include <QFile>
#include <QRegularExpression>
#include <QtDebug>

extern int fwbdebug;

using namespace std;
using namespace libfwbuilder;


void HostsFile::parse()
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw FWException("Can't open file '" + file_name.toStdString() + "'");

    data.clear();

    QRegularExpression comment("^\\s*#");
    QRegularExpression hosts_line("^\\s*(\\S+)\\s+(\\S*)");
    QRegularExpressionMatch match;

    while ( ! file.atEnd())
    {
        QString line = QString::fromUtf8(file.readLine().trimmed());

        if (fwbdebug) qDebug() << "Line: " << line;

        if (line.indexOf(comment) > -1) continue;
        if (hosts_line.match(line).hasMatch())
        {
            QString addr_s = match.captured(1);
            QStringList names = match.captured(2).split(",");

            if (fwbdebug)
                qDebug() << "cap(1)=" << match.captured(1)
                         << "cap(2)=" << match.captured(2);

            try
            {
                InetAddr addr(AF_INET6, addr_s.toStdString());
                foreach(QString name, names) data[addr] << name.trimmed();
            } catch (FWException&)
            {
                try
                {
                    InetAddr addr(addr_s.toStdString());
                    foreach(QString name, names) data[addr] << name.trimmed();
                } catch (FWException &ex)
                {
                    string err = ex.toString() +
                        "\nIn line: " + line.toStdString();
                    throw FWException(err);
                }
            }
        }
    }
}

