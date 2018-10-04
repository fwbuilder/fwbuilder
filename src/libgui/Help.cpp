/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

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

#include "global.h"
#include "utils.h"

#include "fwbuilder/Constants.h"

#include "Help.h"
#include "FWWindow.h"

#include <QFile>
#include <QDir>
#include <QLocale>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


Help* Help::help_window = nullptr;

Help::Help(QWidget *, const QString &title) :
    QDialog(nullptr)
{
    m_dialog = new Ui::HelpView_q;
    m_dialog->setupUi(this);
    setWindowTitle("Firewall Builder Help");
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint);

    connect(m_dialog->comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showReleaseNotesSelected()));

    QString locale = QLocale::system().name(); //"en_US";

    // Set up path to help qtextBrowser find contents, such as files for <img>
    paths.append(QString(Constants::getResourcesDirectory().c_str()) +
                 "/help/" + locale);
    paths.append(QString(Constants::getResourcesDirectory().c_str()) +
                 "/help/" + "en_US");
    m_dialog->textview->setSearchPaths(paths);
    m_dialog->textview->setOpenLinks(true);
    m_dialog->textview->setOpenExternalLinks(true);

    setName(title);
    resize(600, 700);
    //raise();

    flags = windowFlags()| Qt::WindowMinimizeButtonHint;
    m_dialog->comboBox->hide();
};

Help::~Help()
{
    delete m_dialog;
}

Help* Help::getHelpWindow(QWidget* w)
{
    if (help_window == nullptr)
    {
        help_window = new Help(w, "Firewall Builder");
        help_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    }

    return help_window;
}

void Help::setName(const QString &name)
{
    m_dialog->objectname->setText(name);
}

void Help::setSource(const QUrl &url)
{
        m_dialog->textview->setSource(url);
}

QString Help::findHelpFile(const QString &file_base_name)
{
    QString locale = QLocale::system().name(); //"en_US";
    QFile f;

    foreach(QString p, paths)
    {
        QString try_file_path = p + "/" + file_base_name;
        if (fwbdebug) qDebug("Checking help file %s", try_file_path.toLatin1().constData());
        if (QFile::exists(try_file_path)) return try_file_path;
    }
    return "";
}

void Help::showAllReleaseNotes(const QString &path)
{
    m_dialog->comboBox->show();

    QStringList filters;
    filters << "release_notes_*.html";

    QDir dir;
    dir.setPath(QFileInfo(path).path());

    m_dialog->comboBox->addItems(dir.entryList(filters, QDir::NoFilter, QDir::Name | QDir::Reversed));
}

void Help::showReleaseNotesSelected()
{
    if (!m_dialog->comboBox->isVisible()) return;

    QString file_name = m_dialog->comboBox->currentText();

    setSource(QUrl("file:" + file_name));
}

void Help::closeEvent(QCloseEvent *event)
{
    m_dialog->comboBox->hide();
    window_geometry = QWidget::saveGeometry();
    QDialog::closeEvent(event);
}

void Help::hideEvent(QHideEvent *event)
{
    restoreGeometry(window_geometry);
    QDialog::hideEvent(event);
}

void Help::showEvent(QShowEvent *event)
{
    restoreGeometry(window_geometry);
    QDialog::showEvent(event);
}

void Help::show()
{
    Help::showNormal();
}
