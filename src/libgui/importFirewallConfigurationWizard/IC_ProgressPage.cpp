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
#include "events.h"
#include "FWBSettings.h"

#include "IC_ProgressPage.h"
#include "ImporterThread.h"
#include "ImportFirewallConfigurationWizard.h"

#include "fwbuilder/Library.h"

#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;

bool fwbdebug_ic = fwbdebug;


IC_ProgressPage::IC_ProgressPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::IC_ProgressPage_q;
    m_dialog->setupUi(this);
    importer = nullptr;

    errors_count = 0;
    warnings_count = 0;

    QTextCursor cursor(m_dialog->importLog->textCursor());
    normal_format = cursor.charFormat();

    error_format = normal_format;
    error_format.setForeground(QBrush(Qt::red));
    error_format.setAnchorHref("http://somewhere.com");
    error_format.setAnchor(true);
    // weight must be between 0 and 99. Qt 4.4.1 does not seem to mind if
    // it is >99 (just caps it) but older versions assert
    error_format.setProperty(QTextFormat::FontWeight, 99);

    warning_format = normal_format;
    warning_format.setForeground(QBrush(Qt::blue));
    warning_format.setProperty(QTextFormat::FontWeight, 99);
    warning_format.setAnchor(true);
    warning_format.setAnchorHref("http://somewhere.com");

}

IC_ProgressPage::~IC_ProgressPage()
{
    disconnect(this, SLOT(logLine(QString)));
    disconnect(this, SLOT(importerFinished()));
    if (importer != nullptr && importer->isRunning()) importer->stop();
}

int IC_ProgressPage::nextId() const
{
    if (fwbdebug_ic) qDebug() << "IC_ProgressPage::nextId()";

    ImportFirewallConfigurationWizard* wz = dynamic_cast<ImportFirewallConfigurationWizard*>(wizard());

    QString platform = wz->platform;
    Firewall *fw = wz->getFirewall();

    // Move on to the next page only if firewall object has been created
    // and the next page only makes sense for pix and fwsm
    if (fw && (platform == "pix" || platform == "fwsm"))
        return ImportFirewallConfigurationWizard::Page_NetworkZones;

    return -1;
}

bool IC_ProgressPage::validatePage()
{
    if (fwbdebug_ic)
        qDebug() << "IC_ProgressPage::validatePage()"
                 << "importer=" << importer
                 << "isRunning=" << ((importer) ? importer->isRunning() : 0);

    if (importer != nullptr && importer->isRunning()) return false;
    return true;
}

bool IC_ProgressPage::isComplete() const
{
    if (importer != nullptr && importer->isRunning()) return false;
    return true;
}

void IC_ProgressPage::importerDestroyed(QObject *obj)
{
    if (fwbdebug_ic) qDebug() << "IC_ProgressPage::importerDestroyed() obj=" << obj;
    if (obj == importer) importer = nullptr;
}

void IC_ProgressPage::initializePage()
{
    if (fwbdebug_ic)
        qDebug() << "IC_ProgressPage::initializePage()";

    if (importer != nullptr && importer->isRunning())
    {
        if (fwbdebug_ic) qDebug() << "importer is still runnig; stopping";
        importer->stop();
        importer->wait();
        delete importer;
    }

    m_dialog->importLog->clear();
    m_dialog->errors_count_display->setText("0");
    m_dialog->warnings_count_display->setText("0");

    ImportFirewallConfigurationWizard* wz = dynamic_cast<ImportFirewallConfigurationWizard*>(wizard());

    QString platform = wz->platform;
    QString firewallName = field("firewallName").toString();
    bool deduplicate = field("deduplicate").toBool();

    QStringList *buffer = &(wz->buffer);
    QString fileName = field("fileName").toString();

    Library *lib = wz->currentLib();
    importer = new ImporterThread(this,
                                  lib,
                                  *buffer, platform, firewallName, fileName,
                                  deduplicate);

    // lists host_os_list and version_list are used-chosen host os and version.
    // We ask user to choose these only for PF, so for other platforms 
    // these lists are going to be empty.
    if ( wz->host_os_list.size() > 0 && wz->version_list.size() > 0)
    {
        int host_os_idx = field("hostOS").toInt();
        int version_idx = field("version").toInt();
        bool add_standard_comments = field("addStandardComments").toBool();

        importer->setUserChoiceHostOS( wz->host_os_list.at( host_os_idx ));
        importer->setUserChoiceVersion( wz->version_list.at( version_idx ));
        importer->setAddStandardCommentsFlag(add_standard_comments);
    }
    
    connect(importer, SIGNAL(destroyed(QObject*)),
            this, SLOT(importerDestroyed(QObject*)));
    connect(importer, SIGNAL(finished()),
            this, SLOT(importerFinished()));

    importer->start();
}

void IC_ProgressPage::cleanupPage()
{
    if (fwbdebug_ic) qDebug() << "IC_ProgressPage::cleanupPage()";
    disconnect(this, SLOT(logLine(QString)));
    disconnect(this, SLOT(importerFinished()));
    if (importer != nullptr && importer->isRunning()) importer->stop();
//    if (importer != nullptr && importer->isRunning()) importer->wait();
}

void IC_ProgressPage::importerFinished()
{
    if (fwbdebug_ic) qDebug() << "IC_ProgressPage::importerFinished()";

    Firewall *fw = importer->getFirewallObject();
    qApp->processEvents(); // to flush the log

    ImportFirewallConfigurationWizard* wz = dynamic_cast<ImportFirewallConfigurationWizard*>(wizard());

    wz->setFirewall(fw);

    QString platform = wz->platform;

    if (fw) // fw can be nullptr if import was uncussessful
    {
        QString fwName = field("firewallName").toString();
        fw->setName(fwName.toUtf8().constData());

        // lists host_os_list and version_list are used-chosen host os and version.
        // We ask user to choose these only for PF, so for other platforms 
        // these lists are going to be empty.
        if ( wz->host_os_list.size() > 0 && wz->version_list.size() > 0)
        {
            int host_os_idx = field("hostOS").toInt();
            int version_idx = field("version").toInt();

            QString hostOS = wz->host_os_list.at( host_os_idx ).toLower();
            QString version = wz->version_list.at( version_idx ).toLower();

            if ( ! hostOS.isEmpty()) fw->setStr("host_OS", hostOS.toStdString());
            if ( ! version.isEmpty()) fw->setStr("version", version.toStdString());
        }

        setFinalPage(false); // this triggers call to nextId()

    } else
    {
        // fw == nullptr
        // normally, wizard would have one more page, but since fw was not
        // created, this page should be the last
        setFinalPage(true);
    }

    emit completeChanged();
}

void IC_ProgressPage::logLine(const QString &buf)
{
    if (buf.isEmpty()) return;

    foreach(QString line, buf.trimmed().split("\n"))
    {
        QTextCharFormat format = normal_format;

        if (line.contains("Parser error"))
        {
            format = error_format;
            errors_count++;
        }

        if (line.contains("Error:"))
        {
            format = error_format;
            errors_count++;
        }

        if (line.contains("Parser warning"))
        {
            format = warning_format;
            warnings_count++;
        }

        if (line.contains("Warning:"))
        {
            format = warning_format;
            warnings_count++;
        }

        QString txt = line;
        while (!txt.isEmpty() && (txt.endsWith("\n") || txt.endsWith("\r")))
            txt.chop(1);

        if (format == error_format || format == warning_format)
            format.setAnchorHref(txt);

        QTextCursor cursor = m_dialog->importLog->textCursor();
        cursor.insertBlock();
        cursor.insertText(txt, format);

        QString s;
        m_dialog->errors_count_display->setText(s.setNum(errors_count));
        m_dialog->warnings_count_display->setText(s.setNum(warnings_count));
    }

    m_dialog->importLog->ensureCursorVisible();
}

void IC_ProgressPage::saveLog()
{
    QString dir;
    dir = st->getWDir();
    if (dir.isEmpty())  dir = st->getOpenFileDir();
    if (dir.isEmpty())  dir = "~";

    QString s = QFileDialog::getSaveFileName(
        this,
        "Choose a file",
        dir,
        "Text file (*.txt)");

    if (!s.isEmpty())
    {
        if (s.endsWith(".txt"))
        {
            s += ".txt";
        }
        QFile f(s);
        if (f.open(QIODevice::WriteOnly))
        {
            if (fwbdebug)
            {
                qDebug("Saving crawler log to file: %d chars",
                       m_dialog->importLog->toPlainText().length());
                qDebug("--------------------------------");
            }
            QTextStream strm(&f);
            QString txt = m_dialog->importLog->toPlainText();
            strm << txt << endl;
            if (fwbdebug)
            {
                qDebug("%s",txt.toLatin1().constData());
                qDebug("--------------------------------");
            }
            f.close();
        }
    }
}

