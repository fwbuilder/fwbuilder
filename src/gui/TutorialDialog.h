#ifndef TUTORIALDIALOG_H
#define TUTORIALDIALOG_H

#include <QDialog>
#include "TutorialAnimator.h"


namespace Ui {
    class TutorialDialog_q;
}

class TutorialDialog : public QDialog {
    Q_OBJECT
public:
    TutorialDialog(QWidget *parent = 0);
    ~TutorialDialog();

    QString getScenarioForPage(int page);
    QString getUndoForPage(int page);
    QString getResetForPage(int page);
    void runScenario(QString scenario);

    void resizeEvent(QResizeEvent *);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TutorialDialog_q *ui;
    int currentPage;
    TutorialAnimator *animator;
    QMap<int, bool> prevEnabled;
    QMap<int, bool> requiresPrev;

public slots:
    void previous();
    void next();
    void reset();
    void demonstrate();
    void showPage(int page);
};

#endif // TUTORIALDIALOG_H
