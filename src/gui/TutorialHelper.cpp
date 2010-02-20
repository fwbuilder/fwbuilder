#include "TutorialHelper.h"
#include <QTest>
#include <QDebug>
#include <QMenu>
#include <QAbstractItemView>
#include <QDesktopWidget>
#include <QApplication>
#include <QCursor>
#include "global.h"
#include "FWWindow.h"
#include "TutorialAnimator.h"

TutorialHelper::TutorialHelper()
{
    speed = 50;
}

void TutorialHelper::moveMouse(QPoint end)
{
    qreal distance = QLineF(QCursor::pos(), end).length();
    qreal screenSize = QLineF(QPoint(0,0), QApplication::desktop()->geometry().bottomRight()).length();
    qreal time = (speed*2) * distance / screenSize;
    qreal step = distance / time;

    int timestep = time / (distance/step);
    while (QLineF(QCursor::pos(), end).length() > step)
    {
        QLineF line(QCursor::pos(), end);
        line.setLength(step);
        QCursor::setPos(line.p2().toPoint());
        QTest::qWait(timestep);
    }
    QCursor::setPos(end);
}

void TutorialHelper::moveMouse(QWidget *w, QPoint userpoint)
{
    QPoint pos;
    if (userpoint == QPoint(-1,-1))
        pos = w->mapToGlobal(w->rect().center());
    else
        pos = w->mapToGlobal(userpoint);
    moveMouse(pos);
}

void TutorialHelper::clickWidget(QWidget *w)
{
    QTest::mouseClick(w, Qt::LeftButton, Qt::NoModifier, QPoint(), speed*4);
}

void TutorialHelper::clickMenuItem(QMenu *menu, QPoint pos)
{
    QTest::mouseClick(menu, Qt::LeftButton, Qt::NoModifier, pos, speed);
}

void TutorialHelper::typeWidget(QWidget *w, QString text)
{
    QTest::keyClicks(w, text, Qt::NoModifier, 50);
}

QPoint TutorialHelper::findViewItem(QAbstractItemView *view, int id)
{
    int x = view->width() / 5;
    int top = -1, bottom = -1;
    for (top=0; top<view->height(); top++)
    {
        if (view->indexAt(QPoint(x, top)).row() == id)
            break;
    }
    for (bottom=view->height(); bottom != 0; bottom--)
    {
        if (view->indexAt(QPoint(x, bottom)).row() == id)
            break;
    }
    return QPoint(x, (top+bottom)/2);
}

void TutorialHelper::selectComboItem(QWidget *widget, int id)
{
    QComboBox * combo = dynamic_cast<QComboBox*>(widget);
    moveMouse(combo);
    QTest::mouseClick(combo, Qt::LeftButton);
    QPoint itemPos = this->findViewItem(combo->view(), id);
    QTest::qWait(speed*4);
    moveMouse(combo->view(), itemPos);
    QTest::mouseClick(combo->view(), Qt::LeftButton, 0, itemPos);
}

void TutorialHelper::selectComboItem(QWidget *widget, QString name)
{
    QComboBox * combo = dynamic_cast<QComboBox*>(widget);
    int id = combo->findText(name);
    selectComboItem(widget, id);
}

void TutorialHelper::selectListItem(QWidget *widget, int id)
{
    QAbstractItemView *view = dynamic_cast<QAbstractItemView*>(widget);
    QPoint itemPos = findViewItem(view, id);
    view->scrollTo(view->indexAt(itemPos));
    itemPos = findViewItem(view, id);
    moveMouse(view, itemPos);
    view->setCurrentIndex(view->indexAt(itemPos));
    QTest::mouseClick(view, Qt::LeftButton, 0, itemPos);
}

void TutorialHelper::selectListItem(QWidget *widget, QString name)
{
    QAbstractItemView * view = dynamic_cast<QAbstractItemView*>(widget);
    int id = 0;
    for (; id < view->model()->rowCount(); id++)
        if (view->model()->data(view->model()->index(id, 0, QModelIndex())).toString() == name)
            break;
    selectListItem(widget, id);
}

class PublicTabBar : public QTabWidget { public: using QTabWidget::tabBar; };

QPoint findTab(QTabBar *bar, int id)
{
    int x = bar->width()/2;
    int top, bottom, left, right;
    for (top=0; top<bar->height(); top++)
        if (bar->tabAt(QPoint(x, top)) == id)
            break;
    for (bottom = bar->height(); bottom != 0; bottom--)
        if (bar->tabAt(QPoint(x, bottom)) == id)
            break;
    int y = bar->height()/2;
    for (left = 0; left < bar->width(); left++)
        if (bar->tabAt(QPoint(left, y)) == id)
            break;
    for (right = bar->width(); right!=0; right--)
        if (bar->tabAt(QPoint(right, y)) == id)
            break;
    qDebug() << left << right << top << bottom;
    return QPoint((left+right)/2, (top+bottom)/2);
}

void TutorialHelper::selectTab(QWidget *widget, int id)
{
    QTabBar *bar = dynamic_cast<QTabBar*>(dynamic_cast<TutorialAnimator*>(parent())->findChild(widget, "w#QTabBar"));
    QPoint pos = findTab(bar, id);
    moveMouse(bar, pos);
    QTest::qWait(speed*4);
    QTest::mouseClick(bar, Qt::LeftButton, 0, pos);

}

void TutorialHelper::selectTab(QWidget *widget, QString name)
{
    QTabWidget * view = dynamic_cast<QTabWidget*>(widget);
    int id = 0;
    for (; id < view->count(); id++)
        if (view->tabText(id) == name)
            break;
    selectTab(widget, id);
}


