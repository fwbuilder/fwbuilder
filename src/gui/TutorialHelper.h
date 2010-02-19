#ifndef TUTORIALHELPER_H
#define TUTORIALHELPER_H

#include <QObject>
#include <QMenu>
#include <QComboBox>

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
    void selectComboItem(QWidget *box, int id);
    void selectComboItem(QWidget *box, QString name);
    void selectListItem(QWidget *box, int id);
    void selectListItem(QWidget *box, QString name);
    void moveMouse(QPoint end);
    void moveMouse(QWidget *w, QPoint userpoint = QPoint(-1,-1));
    void selectTab(QWidget *widget, QString name);
    void selectTab(QWidget *widget, int id);

    QPoint findViewItem(QAbstractItemView *view, int id);
};

#endif // TUTORIALHELPER_H
