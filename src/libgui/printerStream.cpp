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

#include "printerStream.h"

#include "RuleSetModel.h"
#include "RuleSetView.h"

#include <QHeaderView>
#include <QtDebug>
#include <QScrollBar>

#include <qprinter.h>
#include <qdatetime.h>

printerStream::printerStream(QPrinter *p,
                             float ts,
                             float m,
                             bool h,
                             const QString &ht,
                             PrintingProgressDialog *pd) : pr()//,metrics(p)
{
    printer = p;
    table_scaling = ts;
    margin = m;
    printHeader = h;
    headerText = ht;
    ppd = pd;
    headerFont = QFont( "times", 10, QFont::Normal );
    bodyFont = QFont( "times", 14, QFont::Normal );
    headerHeight = 1.5;  // 1.5 cm for header
    pageNo = 0;
    active = false;
    fromPage = 1;
    toPage = 9999;

    headerTimeString = QDateTime::currentDateTime().toString();
}

bool printerStream::begin()
{
    if( !pr.begin(printer) )               // paint on printer
        return false;
    active=true;

    pageWidth = printer->width();
    pageHeight = printer->height();

    if (fwbdebug)
    {
        qDebug("printer dimensions: %dx%d",pageWidth,pageHeight);
        qDebug("Margin: %.1f", margin);
    }

    dpiy = printer->logicalDpiY();
    ymargin = (int) ( (margin/2.54)*dpiy );
// assuming printer's resolutions by X and Y axes are the same
    xmargin = ymargin;
    pageBody = QRect( xmargin,
                      ymargin,
                      printer->width() - 2 * xmargin,
                      printer->height() - 2 * ymargin );

    yHeaderHeight = int((headerHeight / 2.54) * dpiy);
    yHeaderLine = int(((headerHeight - 0.5) / 2.54) * dpiy);

    pr.setFont(headerFont);
    QFontMetrics fm = pr.fontMetrics();
    QRect br = fm.boundingRect("Page 999");

    headerTextBox = QRect(xmargin,
                          ymargin + yHeaderLine - fm.lineSpacing() - 1,
                          printer->width() - 2 * xmargin,
                          fm.lineSpacing() + 1);

    headerBox = QRect(xmargin,
                      ymargin,
                      printer->width() - 2 * xmargin,
                      yHeaderHeight);

    if (fwbdebug)
    {
        qDebug("dpiy=%d", dpiy);
        qDebug("yHeaderHeight=%d", yHeaderHeight);
        qDebug("yHeaderLine=%d", yHeaderLine);
        qDebug("fm.lineSpacing()=%d", fm.lineSpacing());
        qDebug("bounding rect for the header text: l=%d,t=%d,w=%d,h=%d",
               br.left(), br.top(), br.width(), br.height());
        qDebug("headerBox: l=%d,t=%d,w=%d,h=%d",
               headerBox.left(), headerBox.top(),
               headerBox.width(), headerBox.height());
        qDebug("headerTextBox: l=%d,t=%d,w=%d,h=%d",
               headerTextBox.left(), headerTextBox.top(),
               headerTextBox.width(), headerTextBox.height());
    }

    yPos = 0;
    pageNo = 1;

    QPaintDevice *dev = pr.device();
    // lets table with width 1000 pixels be drawn 80% of the page width
    pixmap_scaling_ratio = float(dev->width()) * 0.8 / 1000 * table_scaling;

    return true;
}

void printerStream::end()
{
    // according to QT docs, there is no need to explicitly call QPainter::end()
    // because its destructor does it
    //    pr.end();
}

int printerStream::getWorkspaceWidth()
{
    return pageBody.width();
}

int printerStream::getWorkspaceHeight()
{
    return (printHeader) ? (pageBody.height()-headerBox.height()) :
        pageBody.height();
}

void printerStream::beginPage()
{
    yPos=0;
    if (ppd!=nullptr) ppd->setCurrentPageNo(pageNo);

    if (printHeader)
    {
        if (fwbdebug) qDebug("Printing header for page %d (%d-%d)",
                             pageNo, fromPage, toPage);

        QString page = QObject::tr("Page %1").arg(pageNo);
        if (pageNo>=fromPage && pageNo<=toPage)
        {
            pr.setFont(headerFont);
            pr.setPen(Qt::black);
            pr.setPen(Qt::SolidLine);
            pr.drawText(headerTextBox,Qt::AlignLeft,page);
            pr.drawText(headerTextBox,Qt::AlignCenter,headerText);
            pr.drawText(headerTextBox,Qt::AlignRight,headerTimeString);

            pr.drawLine(headerTextBox.left(),headerTextBox.bottom(),
                        headerTextBox.right(),headerTextBox.bottom());
        }
        yPos = ymargin+headerBox.height();
    }
}

void printerStream::flushPage()
{
    if (pageNo>=fromPage && pageNo<=toPage)
        printer->newPage();
    pageNo++;
}

int printerStream::getTextHeight(const QString &txt)
{
    if (txt.isEmpty()) return 0;
    if (printer->printerState() == QPrinter::Aborted) return 0;

    pr.setFont( bodyFont );
    QFontMetrics fm = pr.fontMetrics();
    int nlines=1;
    int i=-1;
    while ( (i=txt.indexOf("\n",i+1))>=0 ) nlines++;
    return nlines*fm.lineSpacing();
}

void printerStream::printText(const QString &txt, bool newLine)
{
    if (fwbdebug)
    {       
        qDebug("printText -------");
        qDebug("pageBody.height(): %d", pageBody.height());
        qDebug("yPos: %d", yPos);
    }

    if (txt.isEmpty()) return;
    if (printer->printerState() == QPrinter::Aborted) return;

    pr.setFont( bodyFont );
    QFontMetrics fm = pr.fontMetrics();
    QRect br = fm.boundingRect(txt);

    if (getYSpace()<br.height())
    {
        flushPage();
        beginPage();   // resets yPos
    }

    if (pageNo>=fromPage && pageNo<=toPage)
    {
        pr.setPen(Qt::black);
        pr.drawText( xmargin, yPos, printer->width()-2*xmargin, br.height(),
                     Qt::TextExpandTabs | Qt::TextDontClip,
                     txt );
    }
    int nlines=1;
    int i=-1;
    while ( (i=txt.indexOf("\n",i+1))>=0 ) nlines++;
    if (newLine) yPos = yPos + nlines*fm.lineSpacing();
}

void printerStream::printPixmap(const QPixmap &pm, bool newLine)
{
#if 0
    QPaintDevice *dev = pr.device();
    if (fwbdebug)
    {
        qDebug("printPixmap: width=%d height=%d", pm.width(), pm.height());
        qDebug("printPixmap: printer->resolution()=%d", printer->resolution());
        if (dev)
        {
            qDebug("printPixmap: device parameters:");
            qDebug("             height=%d width=%d",
                   dev->height(), dev->width());
            qDebug("             logicalDpiY=%d logicalDpiX=%d",
                   dev->logicalDpiY(), dev->logicalDpiX());
            qDebug("             physicalDpiY=%d physicalDpiX=%d",
                   dev->physicalDpiY(), dev->physicalDpiX());
        }
    }
#endif

    int target_w = (int)(pm.width() * pixmap_scaling_ratio);
    int target_h = (int)(pm.height() * pixmap_scaling_ratio);

    int pmYOffset = 0;
    while ( getYSpace()<(pm.height()-pmYOffset) )
    {
        int yFrag = pageBody.height() - yPos;
        if (pageNo>=fromPage && pageNo<=toPage)
        {
            if (fwbdebug)
                qDebug("Print pixmap 1: yPos=%d pmYOffset=%d "
                       "yFrag=%d target_w=%d target_h=%d",
                       yPos, pmYOffset, yFrag, target_w, target_h);
            pr.drawPixmap(xmargin, yPos, target_w, target_h,
                          pm,
                          0, pmYOffset, -1, yFrag);
        }
        pmYOffset = pmYOffset + yFrag;
        flushPage();
        beginPage();   // resets yPos
    }
    if (pageNo>=fromPage && pageNo<=toPage)
    {
        if (fwbdebug)
            qDebug("Print pixmap 2: yPos=%d pmYOffset=%d target_w=%d target_h=%d",
                   yPos, pmYOffset, target_w, target_h);
        pr.drawPixmap(xmargin, yPos, target_w, target_h,
                      pm,
                      0, pmYOffset, -1, -1);
    }

    if (newLine) yPos = yPos + (target_h - pmYOffset);
}

void printerStream::printRuleSetView(RuleSetView *tbl, bool top_margin)
{
    if (fwbdebug)
    {
        qDebug("printQTable ----------------------------------------------");
        qDebug("Size: %dx%d", tbl->width(), tbl->height());
        qDebug("YSpace: %d", getYSpace());
        qDebug("yPos: %d", yPos);
    }

    int columnsWidth = 0;
    int i = 0;

    while (i < tbl->model()->columnCount())
    {
        columnsWidth += tbl->columnWidth(i);
        i++;
    }

    RuleSetModelIterator it = ((RuleSetModel*)tbl->model())->begin();
    RuleSetModelIterator end = ((RuleSetModel*)tbl->model())->end();

    RuleSetModelIterator bottomIt;

    while (it.isValid() && it != end)
    {
        // Pages iterations

        int tblHeight = (int)(
            (float)(tbl->header()->height()) * pixmap_scaling_ratio);

        /* ===================================================================
         * Row height is screen pixels, getYSpace returns remaining
         * space in printer resolution units. Keep track of both to
         * resize pixmap
         * ===================================================================
         */
        int pixMapHeight = tbl->header()->height();


        RuleSetModelIterator pit = it;
        while (pit != end)
        {

            // Check if current index is collapsed
            QModelIndex index = pit.index();
            QModelIndex parent = index.parent();
            if (!parent.isValid() || tbl->isExpanded(parent))
            {

                int nth = tblHeight +
                    (int)((float)(tbl->rowHeight(index)) * pixmap_scaling_ratio);

                if ( nth==getYSpace() )  break;

                if ( nth>getYSpace() )
                {
                    // if it == pit then even single row does not fit on the page
                    if (it == pit)
                    {
                        pixMapHeight = tbl->rowHeight(index);
                    } else
                    {
                        --pit;
                    }
                    break;
                }

                tblHeight = nth;
                pixMapHeight += tbl->rowHeight(index);
            }

            ++pit;
        }

        bottomIt = pit;

        int left_hdr_w = 0;

        int top_hdr_h = 0;
        Q_UNUSED(top_hdr_h);
        if (top_margin && tbl->header() != nullptr)
            top_hdr_h = tbl->header()->height();

        int tblWidth = columnsWidth + left_hdr_w;


            qDebug("Page %d -- tblWidth: %d   tblHeight: %d",
                   pageNo, tblWidth, tblHeight);

        tbl->resize(tblWidth, pixMapHeight);
        tbl->updateWidget();

        tbl->scrollTo(it.index(), QAbstractItemView::PositionAtTop);

        printPixmap(tbl->grab());

        if (bottomIt == end) break;

        flushPage();
        beginPage();

        it = bottomIt;
        ++it;
    }
}


void printerStream::printQTable(QTableView *tbl, bool left_margin,
                                bool top_margin)
{
    if (fwbdebug)
    {
        qDebug("printQTable ----------------------------------------------");
        qDebug("Size: %dx%d", tbl->width(), tbl->height());
//        qDebug("Visible: %dx%d",
//               tbl->contentsRect().width(), tbl->contentsRect().height());
//        qDebug("Viewport: %dx%d",
//               tbl->viewport()->width(), tbl->viewport()->height());
//        qDebug("pageBody.height(): %d", pageBody.height());
        qDebug("YSpace: %d", getYSpace());
        qDebug("yPos: %d", yPos);
    }

    int top_row = 0;
    int bottom_row = 1;

    int columnsWidth = 0;
    int i = 0;
    while (i < tbl->model()->columnCount())
    {
        columnsWidth += tbl->columnWidth(i);
        i++;
    }

    int rowCount = tbl->model()->rowCount();
    while (top_row <= (rowCount-1))
    {
        int row = 0;

        int tblHeight = (int)(
            (float)(tbl->horizontalHeader()->height()) * pixmap_scaling_ratio);

        /* ===================================================================
         * Row height is screen pixels, getYSpace returns remaining
         * space in printer resolution units. Keep track of both to
         * resize pixmap
         * ===================================================================
         */
        int pixMapHeight = tbl->horizontalHeader()->height();
        for (row=top_row; row < rowCount; ++row)
        {
            if (tbl->isRowHidden(row))
            {
                // hidden rows count but do not contribute to table height
                continue;
            }

            int nth = tblHeight +
                (int)((float)(tbl->rowHeight(row)) * pixmap_scaling_ratio);

            if ( nth==getYSpace() )  break;

            if ( nth>getYSpace() ) 
            { 
                row--; 
                break; 
            }

            tblHeight = nth;
            pixMapHeight += tbl->rowHeight(row);
        }
        
        // if row < top_row then even single row does not fit on the page
        if (row < top_row)
        {
            row = top_row;
            pixMapHeight = tbl->rowHeight(top_row);
        }

        if (row == rowCount) row--;

        bottom_row = row;

        int left_hdr_w = 0;
        if (left_margin && tbl->verticalHeader() != nullptr)
            left_hdr_w = tbl->verticalHeader()->width();

        int top_hdr_h = 0;
        Q_UNUSED(top_hdr_h);
        if (top_margin && tbl->horizontalHeader() != nullptr)
            top_hdr_h = tbl->horizontalHeader()->height();

        int tblWidth = columnsWidth + left_hdr_w;

        if (fwbdebug)
            qDebug("Page %d -- (%d-%d of %d rows) tblWidth: %d   tblHeight: %d",
                   pageNo, top_row, bottom_row, rowCount, tblWidth, tblHeight);

        tbl->resize(tblWidth, pixMapHeight);

        tbl->verticalHeader()->resize(
            tbl->verticalHeader()->width(),
            tbl->height() - tbl->horizontalHeader()->height());
        tbl->horizontalHeader()->resize(
            tbl->width() - tbl->verticalHeader()->width(),
            tbl->horizontalHeader()->height());


// QTableView::scrollTo() makes row visible, but if there are not enough
// rows below it, it appears in the middle of the table. This means the table
// shows few rows that belong on the previous page, which is bad.
//
//        tbl->scrollTo(tbl->model()->index(top_row, 0),
//                      QAbstractItemView::PositionAtTop);

        int top_row_position = tbl->verticalHeader()->sectionPosition(top_row);
        tbl->verticalHeader()->setOffset(top_row_position);

        tbl->update();

        printPixmap(tbl->grab());  //,0,0,-1,pixMapHeight));

        if (bottom_row>=(rowCount-1)) break;

        flushPage();
        beginPage();

        top_row = bottom_row + 1;

    }
}



