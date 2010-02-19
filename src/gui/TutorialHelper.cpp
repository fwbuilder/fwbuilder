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
#include "math.h"

TutorialHelper::TutorialHelper()
{
}

int vectorLength(QPoint a, QPoint b)
{
    return sqrt(pow(a.x() - b.x(),2) + pow(a.y() - b.y(),2));
}

QPoint getDirections(QPoint from, QPoint to, int step)
{
    int y = to.y() - from.y(), x = to.x() - from.x();
    double direction = fabs(atan(((double)(y)/x)));
    int vstep, hstep;
    vstep = step*sin(direction);
    hstep = step*cos(direction);
    if (x<0) hstep = -abs(hstep);
    if (y<0) vstep = -abs(vstep);

    return QPoint(hstep, vstep);
}

void TutorialHelper::moveMouse(QPoint end)
{
    qDebug() << end;
    QPoint start = QCursor::pos();
    int length = vectorLength(start, end);
    int maxlen = vectorLength(QPoint(0,0), QPoint(QApplication::desktop()->width(), QApplication::desktop()->height()));
    double time = 100*(((double)length)/maxlen);
    int step = ((double)length)/time;

    int timestep = time / (length/step);
    while (vectorLength(QCursor::pos(), end) > step)
    {
        QPoint newpos = QCursor::pos() + getDirections(QCursor::pos(), end, step);
        QCursor::setPos(newpos);
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
    QTest::mouseClick(w, Qt::LeftButton, Qt::NoModifier, QPoint(), 0);
}

void TutorialHelper::clickMenuItem(QMenu *menu, QPoint pos)
{
    QTest::mouseClick(menu, Qt::LeftButton, Qt::NoModifier, pos);
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
    QTest::qWait(200);
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

