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
#include "FWBSettings.h"

#include "IC_ProgressPage.h"

#include <QString>
#include <QFileDialog>
#include <QTextStream>


IC_ProgressPage::IC_ProgressPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::IC_ProgressPage_q;
    m_dialog->setupUi(this);

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
                qDebug("Saving import log to file: %d chars",
                       m_dialog->discoveryLog->toPlainText().length());
                qDebug("--------------------------------");
            }
            QTextStream strm(&f);
            QString txt = m_dialog->discoveryLog->toPlainText();
            strm << txt << endl;
            if (fwbdebug)
            {
                qDebug("%s",txt.toAscii().constData());
                qDebug("--------------------------------");
            }
            f.close();
        }
    }
}

