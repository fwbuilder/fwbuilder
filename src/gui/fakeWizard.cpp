#include "fwbuilder_ph.h"

#include "fakeWizard.h"

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
    {
        if (enabled)
            finishButton->show();
        else
            finishButton->hide();
    }
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
    for (int i = page-1; i >= 0; i--)
        if (appropriates[i] && appropriate(i))
            return i;

    return -1;
}

int FakeWizard::nextRelevant(const int page) const
{
    for (int i = page+1; i < m_pageCount; i++)
        if (appropriates[i] && appropriate(i))
            return i;

    return -1;
}

void FakeWizard::showPage(const int page)
{
    if (page >= m_pageCount) return;

    if (finishEnabled[page])
        finishButton->show();
    else
        finishButton->hide();

    nextButton->setEnabled( nextEnabled[page] &&
                           (nextRelevant(page) > -1) );
    backButton->setEnabled( backEnabled[page] &&
                           (previousRelevant(page) > -1) );

    if (titleLabel)
        if (!pageTitles[page].isEmpty())
        {
            titleLabel->setText(pageTitles[page]);
            titleLabel->show();
        }
        else
            titleLabel->hide();

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
