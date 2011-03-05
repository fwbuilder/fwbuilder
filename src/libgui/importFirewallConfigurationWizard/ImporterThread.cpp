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

#include "global.h"

#include "ImporterThread.h"

#include "utils.h"
#include "QThreadLogger.h"
#include "IOSImporter.h"
#include "IPTImporter.h"
#include "PIXImporter.h"

#include <QWidget>
#include <QtDebug>

#include <sstream>


using namespace std;
using namespace libfwbuilder;


ImporterThread::ImporterThread(QWidget *ui,
                               FWObject *lib,
                               const QStringList &buffer,
                               const QString &platform,
                               const QString &firewallName)
{
    this->lib = lib;
    this->ui = ui;
    this->buffer = buffer;
    this->platform = platform;
    this->firewallName = firewallName;
    importer = NULL;
}

ImporterThread::~ImporterThread()
{
    if (fwbdebug) qDebug() << "ImporterThread::~ImporterThread()";
}

void ImporterThread::run()
{
    QThreadLogger *logger = new QThreadLogger();
    connect(logger, SIGNAL(lineReady(QString)),
            this->ui, SLOT(logLine(QString)),
            Qt::QueuedConnection);

    std::istringstream instream(buffer.join("\n").toStdString());

    importer = NULL;

    if (platform == "iosacl") importer = new IOSImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (platform == "iptables") importer = new IPTImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (platform == "pix") importer = new PIXImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (importer)
    {
        try
        {
            importer->run();
        } catch(ImporterException &e)
        {
            *logger << e.toString() << "\n";
        }

    } else
    {
        *logger << "Can not import configuration for platform "
                << platform.toStdString() << "\n";
    }

    fw = importer->finalize();

    emit finished();

    deleteLater(); // mark this object for destruction on the next run of event loop
}

void ImporterThread::stop()
{
}

