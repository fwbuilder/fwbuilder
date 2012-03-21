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

#include "config.h"
#include "global.h"
#include "utils.h"

#include "fwbuilder/Constants.h"

#include "Help.h"
#include "FWWindow.h"

#include <QFile>
#include <QLocale>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


Help* Help::help_window = NULL;

Help::Help(QWidget *, const QString &title, bool _load_links_in_browser) :
    QDialog(NULL)
{
    load_links_in_browser = _load_links_in_browser;
    m_dialog = new Ui::HelpView_q;
    m_dialog->setupUi(this);
    setWindowTitle("Firewall Builder Help");
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint);
    delayed_open = false;

    http_getter = new HttpGet();
    connect(http_getter, SIGNAL(done(const QString&)),
            this, SLOT(downloadComplete(const QString&)));

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
};

Help::~Help()
{
    delete m_dialog;
}

Help* Help::getHelpWindow(QWidget* w)
{
    if (help_window == NULL)
    {
        help_window = new Help(w, "Firewall Builder");
        help_window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint);
#if QT_VERSION >= 0x040500
        help_window->setWindowFlags(help_window->windowFlags() | Qt::WindowCloseButtonHint);
#endif
    }

    return help_window;
}

void Help::setName(const QString &name)
{
    m_dialog->objectname->setText(name);
}

void Help::setSource(const QUrl &url)
{
    if (url.toString().startsWith("http:"))
    {
        delayed_open = true;
        if (!http_getter->get(QUrl(url)) && fwbdebug)
        {
            qDebug() << "HttpGet error: " << http_getter->getLastError();
            qDebug() << "Url: " << url;
        }
    } else
    {
        delayed_open = false;
        m_dialog->textview->setSource(url);
    }
}

void Help::downloadComplete(const QString& server_response)
{
    if (fwbdebug)
        qDebug() << "Help::downloadComplete"
                 << "status=" << http_getter->getStatus();
    /*
     * getStatus() returns error status if server esponded with 302 or
     * 301 redirect. Only "200" is considered success.
     */
    if (http_getter->getStatus())
    {
        m_dialog->textview->setHtml(server_response);
        
        /* here is additional layer of protection: if I make a mistake
         * and feed empty page as an announcement, do not show it to
         * the user.  If the user is behind captive portal or dns
         * intercept that feeds them fancy page that consists of only
         * a chunk of javascript and empty body, do not show it
         * either. One example of such case is dnsadvantage.com
         */

        QString c = m_dialog->textview->toPlainText();

        if (fwbdebug) qDebug() << "Announcement in plain text:"
                               << c;

        if (!c.isEmpty() && delayed_open)
        {
            raise();
            show();
        }
    }
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

void Help::closeEvent(QCloseEvent *event)
{
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
