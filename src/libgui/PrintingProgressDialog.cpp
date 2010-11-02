/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#include "PrintingProgressDialog.h"

#include <qstring.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qpushbutton.h>

PrintingProgressDialog::PrintingProgressDialog(QWidget *parent,QPrinter *p,int nPages,bool disableCancel) :
    QDialog(parent)
{
    m_dialog = new Ui::printingProgressDialog_q;
    m_dialog->setupUi(this);

    connect( m_dialog->Cancel, SIGNAL( clicked() ),
             this,             SLOT( abortPrinting() ) );

    printer=p;

    setNPages(nPages);
    m_dialog->text->setText("");
    if (disableCancel) m_dialog->Cancel->hide();
}

PrintingProgressDialog::~PrintingProgressDialog()
{
    delete m_dialog;
}

void PrintingProgressDialog::setCurrentPageNo(int n)
{
    if (totalPages)
        m_dialog->text->setText(tr( "Printing (page %1/%2)" ).arg(n).arg(totalPages));
    else
        m_dialog->text->setText(tr( "Printing page %1" ).arg(n) );

    m_dialog->progressBar->setValue(n);

    QApplication::processEvents(QEventLoop::AllEvents,50);
}

void PrintingProgressDialog::genericProgressIndicator(int n,const QString &txt)
{
    m_dialog->text->setText(txt);
    m_dialog->progressBar->setValue(n);

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,10);
}

void PrintingProgressDialog::abortPrinting()
{
    if (printer->abort())
        m_dialog->text->setText( tr("Aborting print operation") );
    else
        m_dialog->text->setText( tr("Cannot abort printing") );
}

void PrintingProgressDialog::setNPages(int n)
{
    totalPages=n;
    m_dialog->progressBar->setMinimum(0);
    m_dialog->progressBar->setMaximum(totalPages);
    m_dialog->progressBar->reset();
    //m_dialog->progressBar->setTotalSteps(totalPages);
}

