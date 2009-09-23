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

#include "../../config.h"
#include "global.h"
#include "utils.h"

#include "Help.h"

#include <QFile>
#include <QLocale>


using namespace std;

Help::Help(QWidget *parent, const QString &title) : SimpleTextView(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QString locale = QLocale::system().name(); //"en_US";

    // Set up path to help qtextBrowser find contents, such as files for <img>
    paths.append(QString(respath.c_str()) + "/help/" + locale);
    paths.append(QString(respath.c_str()) + "/help/" + "en_US");
    m_dialog->textview->setSearchPaths(paths);
    m_dialog->textview->setOpenLinks(true);
    m_dialog->textview->setOpenExternalLinks(true);

    setModal(false);
    setName(title);
    resize(500, 600);
    raise();
};

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

