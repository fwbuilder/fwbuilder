/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author: alek@codeminders.com

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

#ifndef __FAKEWIZARD_H__
#define __FAKEWIZARD_H__

#include <vector>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qlabel.h>
#include <qstring.h>

class FakeWizard
{
protected:
    QWidget        *mainWidget;
    QStackedWidget *stackedWidget;
    QPushButton    *nextButton;
    QPushButton    *backButton;
    QPushButton    *finishButton;
    QPushButton    *cancelButton;
    QLabel         *titleLabel;
    
    int m_currentPage;
    int m_pageCount;
    
    std::vector<bool> nextEnabled;
    std::vector<bool> backEnabled;
    std::vector<bool> appropriates;
    std::vector<bool> finishEnabled;
    std::vector<QString> pageTitles;
    
    int nextRelevant(const int page) const;
    int previousRelevant(const int page) const;
    
public :
    
    FakeWizard();
    virtual ~FakeWizard();
    
    void setControlWidgets(QWidget        *_mainWidget,
                           QStackedWidget *_stackedWidget, 
                           QPushButton    *_nextButton,
                           QPushButton    *_finishButton,
                           QPushButton    *_backButton,
                           QPushButton    *_cancelButton,
                           QLabel         *_titleLabel = nullptr);

    int  pageCount() const;
    int  currentPage() const;
    void showPage(const int page);
    void setCurrentPage(const int page);
    void setNextEnabled(const int page, const bool enabled);
    void setBackEnabled(const int page, const bool enabled);
    void setAppropriate(const int page, const bool value);
    void setFinishEnabled(const int page, const bool enabled);
    void setTitle(const int page, const QString title);
    
    virtual bool appropriate(const int) const
    { return true; }
    
    /*virtual void backClicked();
    virtual void nextClicked();*/
};

#endif
