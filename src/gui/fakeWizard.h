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
                           QLabel         *_titleLabel = NULL);

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
