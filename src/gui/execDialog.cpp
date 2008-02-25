/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: execDialog.cpp,v 1.10 2006/02/18 05:26:30 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"

#include "execDialog.h"

#include <qdir.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>

#include "FWBSettings.h"

#if defined(Q_OS_WIN32)
#  include <stdio.h>
#  include <sys/timeb.h>
#  include <time.h>
#endif

#include <iostream>

using namespace std;

execDialog::~execDialog()
{
    delete m_dialog;
}

execDialog::execDialog(QWidget *parent,const QStringList &args, const QString &closeButtonText)
    : QDialog(parent), proc(parent)
{
    m_dialog = new Ui::execDialog_q;
    m_dialog->setupUi(this);

    m_dialog->output->setWordWrapMode( QTextOption::NoWrap );
    res=-1;

    if (! closeButtonText.isEmpty()) m_dialog->buttonOk->setText(closeButtonText);

    proc.setProcessChannelMode(QProcess::MergedChannels);

    connect(&proc, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()) );
    connect(&proc, SIGNAL(processExited()),   this, SLOT(processExited()) );

    arguments = args;

    m_dialog->stopButton->hide();
    m_dialog->saveLogButton->hide();
}

void execDialog::readFromStdout()
{
//    output->append( proc.readStdout() );
    m_dialog->output->moveCursor( QTextCursor::End );
    m_dialog->output->insertPlainText( proc.readAllStandardOutput() );
}

void execDialog::stopProcess()
{
    proc.terminate();
    QTimer::singleShot( 1000, &proc, SLOT( kill() ) );
}

void execDialog::processExited()
{
    m_dialog->stopButton->hide();
    m_dialog->saveLogButton->show();
    m_dialog->buttonOk->setEnabled( true );
    m_dialog->buttonOk->setFocus();
    res=proc.exitStatus();
}

int execDialog::run()
{
    m_dialog->output->append( arguments.join(" ") );
    m_dialog->output->append("\n");

    QStringList args = arguments;

    assert(!args.empty());
    QString command = args[0];
    args.pop_front();

    proc.start(command, args);

    if ( !proc.waitForStarted() )
    {
        m_dialog->output->append( tr("Error: Failed to start program") );
        return exec();
    }
    m_dialog->saveLogButton->hide();
    m_dialog->stopButton->show();
    m_dialog->stopButton->setFocus();
    m_dialog->buttonOk->setEnabled( false );
    exec();
    return res;
}
void execDialog::saveLog()
{
    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";

    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a file",
                    dir,
                    "Text file (*.txt)");


    if (!s.isEmpty())
    {
        if (!s.endsWith(".txt"))
        {
            s+=".txt";
        }
        QFile f(s);
        if (f.open(QIODevice::WriteOnly))
        {
            QTextStream (&f) << m_dialog->output->toPlainText();
            f.close();
        }
    }
}
