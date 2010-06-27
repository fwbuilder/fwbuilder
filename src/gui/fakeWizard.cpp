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

#include "global.h"
#include "fakeWizard.h"

#include <QtDebug>

FakeWizard::FakeWizard()
{
}

FakeWizard::~FakeWizard()
{
}

void FakeWizard::setControlWidgets(QWidget *_mainWidget,
                       QStackedWidget *_stackedWidget,
                       QPushButton    *_nextButton,
                       QPushButton    *_finishButton,
                       QPushButton    *_backButton,
                       QPushButton    *_cancelButton,
                       QLabel         *_titleLabel)
{
    mainWidget = _mainWidget;
    stackedWidget = _stackedWidget;
    nextButton = _nextButton;
    finishButton = _finishButton;
    backButton =_backButton;
    titleLabel = _titleLabel;
    cancelButton = _cancelButton;

    m_pageCount = stackedWidget->count();

    QObject::connect( nextButton, SIGNAL( clicked() ),
             mainWidget, SLOT( nextClicked() ));
    QObject::connect( backButton, SIGNAL( clicked() ),
             mainWidget, SLOT( backClicked() ));
    QObject::connect( finishButton, SIGNAL( clicked() ),
             mainWidget, SLOT( finishClicked() ));
    QObject::connect( cancelButton, SIGNAL( clicked() ),
             mainWidget, SLOT( cancelClicked() ));

    for (int i = 0; i < m_pageCount; i++)
    {
        appropriates.push_back(true);
        backEnabled.push_back(true);
        nextEnabled.push_back(true);
        finishEnabled.push_back(false);
        pageTitles.push_back(QString());
    }
    backEnabled[0] = false;
    nextEnabled[m_pageCount-1] = false;

    m_currentPage = 0;
}

void FakeWizard::setAppropriate(const int page, const bool value)
{
    if (page >= m_pageCount) return;
    appropriates[page] = value;
}

void FakeWizard::setNextEnabled(const int page, const bool enabled)
{
    if (page >= m_pageCount) return;
    nextEnabled[page] = enabled;
    if (page == currentPage())
        nextButton->setEnabled(enabled);
}

void FakeWizard::setBackEnabled(const int page, const bool enabled)
{
    if (page >= m_pageCount) return;
    backEnabled[page] = enabled;
    if (page == currentPage())
        backButton->setEnabled(enabled);
}

void FakeWizard::setFinishEnabled(const int page, const bool enabled)
{
    if (page >= m_pageCount) return;
    finishEnabled[page] = enabled;
    if (page == currentPage())
        finishButton->setEnabled(enabled);
}

void FakeWizard::setTitle(const int page, const QString title)
{
    if (page >= m_pageCount) return;
    pageTitles[page] = title;
    if (page == currentPage())
        if (titleLabel)
            titleLabel->setText(title);
}

int  FakeWizard::pageCount() const
{
    return stackedWidget->count();
}

int FakeWizard::previousRelevant(const int page) const
{
    if (fwbdebug)
        qDebug() << "FakeWizard::previousRelevant(" << page << ")";
    int prev_p = -1;
    for (int i = page-1; i >= 0; i--)
        if (appropriates[i] && appropriate(i))
        {
            prev_p = i;
            break;
        }
    if (fwbdebug)
        qDebug() << "FakeWizard::previousRelevant: previous page" << prev_p;
    return prev_p;
}

int FakeWizard::nextRelevant(const int page) const
{
    if (fwbdebug)
        qDebug() << "FakeWizard::nextRelevant(" << page << ")";
    int next_p = -1;
    for (int i = page+1; i < m_pageCount; i++)
        if (appropriates[i] && appropriate(i))
        {
            next_p = i;
            break;
        }
    if (fwbdebug)
        qDebug() << "FakeWizard::nextRelevant: next page" << next_p;
    return next_p;
}

void FakeWizard::showPage(const int page)
{
    if (page >= m_pageCount) return;

    nextButton->setEnabled(nextEnabled[page] && (nextRelevant(page) > -1) );
    backButton->setEnabled(backEnabled[page] && (previousRelevant(page) > -1) );
    finishButton->setEnabled(finishEnabled[page]);

    if (titleLabel)
    {
        if (!pageTitles[page].isEmpty())
        {
            titleLabel->setText(pageTitles[page]);
            titleLabel->show();
        }
        else
            titleLabel->hide();
    }
    setCurrentPage(page);

    stackedWidget->setCurrentIndex(page);
}

int  FakeWizard::currentPage() const
{
    return m_currentPage;
}

void FakeWizard::setCurrentPage(const int page)
{
    m_currentPage = page;
}
