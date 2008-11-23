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
#include <QDir>
#include <QTextStream>
#include <QLocale>


using namespace std;


Help::Help(QWidget *parent, const QString &help_file, const QString &title) :
    SimpleTextView(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setModal(false);
    setName(title);
    resize(500, 600);
    raise();

    setText(getHelpFileContents(help_file));
};


void Help::scrollToAnchor(const QString &anchor)
{
    m_dialog->textview->scrollToAnchor(anchor);
}


QString Help::getHelpFileContents(const QString &help_file)
{
    QString locale = QLocale::system().name(); //"en_US";

    QString res;
    QFile f;
    QTextStream ts;

    if (!Help::getFile(help_file, locale, f))
    {
        // We do not have help file for this locale (including locale "C")
        // Show English one as a default
        locale = "en_US";
    }

    if (Help::getFile(help_file, locale, f) && f.open(QIODevice::ReadOnly ))
    {
        ts.setDevice(&f);
        res = ts.readAll();
        f.close();
    } else
    {
        res = QString("Help file %1 not found.").arg(f.fileName());
    }
    return res;
}

bool Help::getFile(const QString &help_file, const QString &locale, QFile &file)
{
    file.setFileName(QString(respath.c_str()) + "/help/" + help_file +
                     "_" + locale + ".html");

    return file.exists();
}

