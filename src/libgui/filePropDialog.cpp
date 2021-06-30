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


#include "global.h"
#include "utils.h"

#include "fwbuilder/FWObjectDatabase.h"

#include "filePropDialog.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "PrintingProgressDialog.h"
#include "printerStream.h"

#include "RCS.h"

#include <qlabel.h>
#include <qtextbrowser.h>
#include <qprintdialog.h>

#include <time.h>

#ifndef _WIN32
#  include <unistd.h>     // for access(2)
#endif


#include <iostream>

using namespace std;

filePropDialog::~filePropDialog()
{
    delete m_dialog;
}

filePropDialog::filePropDialog(QWidget *parent, RCS *rcs) :
    QDialog(parent)
{
    m_dialog = new Ui::filePropDialog_q;
    m_dialog->setupUi(this);

    // we have to get a reference to the printer
    // or we can create a new one ...

    m_dialog->fileLocation->setText( rcs->getFileName() );
    if (rcs->isRO()) m_dialog->fileRO->setText( tr("Opened read-only") );
    else             m_dialog->fileRO->setText("");

    time_t lm = mw->activeProject()->db()->getTimeLastModified();
    QString s = ctime( &lm );
    s.truncate( s.length()-1 );   // chop newline

    m_dialog->lastModified->setText( QString("%1 (%2)").arg(s).arg(lm) );

    if (rcs->isInRCS())
    {
        m_dialog->fileRev->setText( rcs->getSelectedRev() );
        m_dialog->fileLockedBy->setText( rcs->getLockedBy() );

        QList<Revision>::iterator i;
        for (i=rcs->begin(); i!=rcs->end(); ++i)
        {
            Revision r= *i;
            m_dialog->fileRevHistory->append( tr("Revision %1").arg(r.rev) );
            m_dialog->fileRevHistory->append( r.log );
            m_dialog->fileRevHistory->append("\n");
            m_dialog->fileRevHistory->moveCursor(QTextCursor::Start);
        }
    } else
    {
        m_dialog->fileRev->setText("");
        m_dialog->fileLockedBy->setText("");
        m_dialog->fileRevHistory->setText("");
    }

}
void filePropDialog::setPrinter(QPrinter *p)
{
    printer=p;
}

void filePropDialog::printRevHistory()
{
    // Revision history printing:
    //
    // 1. setup printer properties
    // 2. create a stream
    // 3. split text from the QTextBrowser (fileRevHistory) into the lines
    // 4. send lines to the stream
    //

    //int pageWidth = 0;
    //int pageHeight = 0;
    bool  fullPage = false;

    float margin;
#if defined(Q_OS_MACX) || defined(Q_OS_WIN32)
    margin=1.5;
#else
    margin=0;
#endif
    int   resolution = 150;
    bool  printHeader = true;
    //bool  printLegend = true;
    //bool  printObjects = true;
    //bool  newPageForSection = false;
    //int   tableResolution = 2;   // 50%, 75%, 100%, 150%, 200%, default 100%

    QPrintDialog printDialog(printer, this);

    printDialog.addEnabledOption(QAbstractPrintDialog::PrintPageRange);
    printDialog.setPrintRange(QAbstractPrintDialog::AllPages);
    printDialog.setMinMax(1,9999);

    printer->setResolution(resolution);
    printer->setFullPage(fullPage);

    if (printDialog.exec() == QDialog::Accepted)
    {
	int fromPage = printer->fromPage();
	int toPage = printer->toPage();
	if (fromPage==0) fromPage=1;
	if (toPage==0) toPage=9999;

	PrintingProgressDialog *ppd = new PrintingProgressDialog(this,printer,0,false);
	QString headerText = "Revision History:";

#if defined(Q_OS_MACX)
	printerStream pr(printer,1.0,margin,printHeader,headerText,nullptr);
#else
	printerStream pr(printer,1.0,margin,printHeader,headerText,ppd);
	ppd->show();
#endif
	pr.setFromTo(fromPage,toPage);
	if ( !pr.begin())
	{
	    ppd->hide();
	    delete ppd;
	    return;
	}

	//QSize margins = printer->margins();

	pr.beginPage();   // resets yPos
	QStringList sl;
        sl=m_dialog->fileRevHistory->toPlainText().split('\n');

	for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it )
	{
	    pr.printText(*it);
	}

	ppd->hide();
	delete ppd;

	pr.end();


    }
}
