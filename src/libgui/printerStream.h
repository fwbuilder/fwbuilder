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


#ifndef  __PRINTERSTREAM_H_
#define  __PRINTERSTREAM_H_

#include <qstring.h>
#include <qpainter.h>

#include <QtWidgets/QTreeView>
#include <QtWidgets/QTableView>

#include <vector>

#include "PrintingProgressDialog.h"

class QPrinter;
class RuleSetView;

class printerStream {
    QPrinter *printer;
    QPainter  pr;
    QRect     pageBody;
    PrintingProgressDialog *ppd;

    int       yPos;
    float     margin;
    int       ymargin;
    int       xmargin;
    int       fromPage;
    int       toPage;
    bool      active;
    bool      printHeader;
    QString   headerTimeString;
    QString   headerText;
    QFont     headerFont;
    QRect     headerTextBox;
    QRect     headerBox;
    QFont     bodyFont;
    float     headerHeight;
    int       yHeaderHeight;
    int       yHeaderLine;
    int       pageNo;
    int       pageWidth;
    int       pageHeight;
    int       dpiy;
    float     pixmap_scaling_ratio;
    float     table_scaling;
    
    public:

    printerStream(QPrinter *p,
                  float table_scaling,
                  float margin, bool header,
                  const QString &headerText,
                  PrintingProgressDialog *ppd);

    bool begin();
    void end();
    bool isActive() { return active; }
    void setFromTo(int from, int to) { fromPage=from; toPage=to; }


    QPainter& painter() { return pr; }

    int  getYMargin() { return ymargin; }
    int  getXMargin() { return xmargin; }

    void printText(const QString &txt, bool newLine=true);
    void printPixmap(const QPixmap &pm, bool newLine=true);
    void printQTable(QTableView *tbl, bool left_margin=true, bool top_margin=true);
    void printRuleSetView(RuleSetView *tbl, bool top_margin=true);


    int  getTextHeight(const QString &txt);

    void beginPage();
    void flushPage();

    int getPageHeight() { return pageHeight; }
    int getPageWidth() { return pageWidth; }
    int getWorkspaceHeight();
    int getWorkspaceWidth();

    int getYPos() { return yPos; }
    int getYSpace() { return pageBody.height()-yPos; }

};

#endif
