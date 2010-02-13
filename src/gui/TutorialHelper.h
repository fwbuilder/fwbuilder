#ifndef TUTORIALHELPER_H
#define TUTORIALHELPER_H

#include <QObject>
#include <QMenu>

class TutorialHelper : public QObject
{
    Q_OBJECT
    QWidget *widget;
public:
    TutorialHelper();
public slots:
    void clickWidget(QWidget*);
    void clickMenuItem(QMenu *menu, QPoint pos);
    void typeWidget(QWidget *w, QString text);
};

#endif // TUTORIALHELPER_H
