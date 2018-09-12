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

#include "StartTipDialog.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "Help.h"

#include "fwbuilder/Constants.h"

#include <QCheckBox>
#include <QLocale>
#include <QtDebug>
#include <QDesktopServices>

#include <stdlib.h>


using namespace std;
using namespace libfwbuilder;

StartTipDialog::StartTipDialog(QWidget *parent): QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    m_dialog = new Ui::StartTipDialog_q;
    m_dialog->setupUi(this);

    QString pgm = m_dialog->program_name->text();
    m_dialog->program_name->setText(pgm.arg(GENERATION));
    m_dialog->program_version->setText(VERSION);

    QString locale = QLocale::system().name(); //"en_US";
    QStringList paths;
    paths.append(QString(Constants::getResourcesDirectory().c_str()) +
                 "/help/" + locale);
    paths.append(QString(Constants::getResourcesDirectory().c_str()) +
                 "/help/" + "en_US");

    m_dialog->textview->setSearchPaths(paths);
    m_dialog->textview->setOpenLinks(true);
    m_dialog->textview->setOpenExternalLinks(true);

    current_tip = -1;

    // preload tips that come with the package
    
    // we use separate Help() object for the tip of the day becayse it should
    // have different size and should not be persistent
    Help *h = new Help(nullptr, "");
    int tip_no = 1;
    while (true)
    {
        QString tip_file;
        tip_file.sprintf("tip%02d.html", tip_no);
        QString contents;
        if (fwbdebug)
            qDebug("Trying tip file %s", tip_file.toLatin1().constData());

        QString help_file = h->findHelpFile(tip_file);
        if (!help_file.isEmpty())
        {
            tips.append("file:" + tip_file);
            tip_no++;
        } else
            break;
    }
    delete h;
    current_tip = tips.size() - 1;

    if (fwbdebug) qDebug("Have %d tips", tips.size());

    first_run = st->getBool("UI/FirstRun");
}

StartTipDialog::~StartTipDialog()
{
    delete m_dialog;
}

/*
 * Returns file name for a random tip
 */
QString StartTipDialog::getRandomTip()
{
    if (tips.size())
    {
        int n = rand() % tips.size();
        if (fwbdebug) qDebug("Showing tip %d", n);
        return tips[n];
    }
    return "";
}

void StartTipDialog::run()
{
    if (first_run)
    {
        showTip(0);
        st->setBool("UI/FirstRun", false);
    } else
    {
        showTip(getRandomTip(), false);
    }
}

void StartTipDialog::showTip(const QString &txt, bool new_tip)
{
    if (fwbdebug) qDebug("Show tip  %s", txt.toStdString().c_str());
    if (new_tip)
    {
        tips.append(txt);
        current_tip = tips.size() - 1;
    }

    QUrl url(txt);
    if (url.isValid() && url.scheme() == "file")
        m_dialog->textview->setSource(url);
    else
        m_dialog->textview->setText(txt);

    show();
    raise();
}

void StartTipDialog::showTip(int tip_idx)
{
    if (fwbdebug) qDebug("Show tip #%d", tip_idx);
    showTip(tips[tip_idx], false);
}

void StartTipDialog::close()
{
    if (m_dialog->donotshow->isChecked()) st->setBool("UI/NoStartTip", true);
    QDialog::close();
}

void StartTipDialog::nextTip()
{
    if (current_tip < (tips.size() - 1))
    {
        current_tip++;
        showTip(current_tip);
    } else
        run();   // gets next tip, caches it and shows it
}

void StartTipDialog::prevTip()
{
    current_tip--;
    if (current_tip < 0) current_tip = 0;
    showTip(current_tip);
}
