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
#include "PFImporter.h"
#include "objectMaker.h"

#include <QWidget>
#include <QtDebug>

#include <sstream>


using namespace std;
using namespace libfwbuilder;


Logger& operator<<(Logger &logger, const QString &str)
{
    logger << str.toUtf8().constData();
    return logger;
}

ImporterThread::ImporterThread(QWidget *ui,
                               FWObject *lib,
                               const QStringList &buffer,
                               const QString &platform,
                               const QString &firewallName,
                               const QString &fileName,
                               bool deduplicate)
{
    this->lib = lib;
    this->ui = ui;
    this->buffer = buffer;
    this->platform = platform;
    this->firewallName = firewallName;
    this->fileName = fileName;
    this->deduplicate = deduplicate;
    importer = nullptr;
    stopFlag = false;
    addStandardComments = false;
}

ImporterThread::~ImporterThread()
{
    if (fwbdebug) qDebug() << "ImporterThread::~ImporterThread()";
}

void ImporterThread::setUserChoiceHostOS(const QString &s)
{
    userChoiceHostOS = s;
}

void ImporterThread::setUserChoiceVersion(const QString &s)
{
    userChoiceVersion = s;
}

void ImporterThread::setAddStandardCommentsFlag(bool f)
{
    addStandardComments = f;
}

void ImporterThread::run()
{
    QThreadLogger *logger = new QThreadLogger();
    connect(logger, SIGNAL(lineReady(QString)),
            this->ui, SLOT(logLine(QString)),
            Qt::QueuedConnection);

    std::istringstream instream(buffer.join("\n").toStdString());

    importer = nullptr;

    if (platform == "iosacl") importer = new IOSImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (platform == "iptables") importer = new IPTImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (platform == "pix" || platform == "fwsm") importer = new PIXImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (platform == "pf") importer = new PFImporter(
        lib, instream, logger, firewallName.toUtf8().constData());

    if (importer)
    {
        if ( ! userChoiceHostOS.isEmpty())
            importer->setUserChoiceHostOS(userChoiceHostOS.toStdString());
        
        if ( ! userChoiceVersion.isEmpty())
            importer->setUserChoiceVersion(userChoiceVersion.toStdString());

        importer->setAddStandardCommentsFlag(addStandardComments);

        importer->setFileName(fileName.toUtf8().constData());
        if (deduplicate) importer->prepareForDeduplication();

        try
        {
            importer->run();
        } catch(ImporterException &e)
        {
            *logger << e.toString() << "\n";
        } catch(ObjectMakerException &e)
        {
            *logger << e.toString() << "\n";
        }

    } else
    {
        *logger << "Can not import configuration for platform "
                << platform.toStdString() << "\n";
    }

    if ( ! stopFlag)
    {
        fw = importer->finalize();
        emit finished();
    }

    deleteLater(); // mark this object for destruction on the next run of event loop
}

void ImporterThread::stop()
{
    stopFlag = true;
}

