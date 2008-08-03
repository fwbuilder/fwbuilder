/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

  $Id: SimpleTextView.cpp 332 2008-07-06 20:11:03Z vadim $

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
    setModal(false);
    setName(title);
    resize(500, 600);
    raise();

    QString locale = QLocale::system().name(); //"en_US";

    QFile f;
    QTextStream ts;
    f.setFileName(QString(respath.c_str()) + "/help/" + help_file +
                  "_" + locale + ".html");

    if (f.exists())
    {
        if (f.open(QIODevice::ReadOnly ))
        {
            ts.setDevice(&f);
            setText(ts.readAll());
            f.close();
        }
    }
    else
        setText(QString("Help file %1 not found.").arg(help_file));
};





