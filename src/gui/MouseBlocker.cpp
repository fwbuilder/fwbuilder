#include "MouseBlocker.h"
#include <QCursor>
#include <QTest>

MouseBlocker::MouseBlocker(QObject *parent) :
    QThread(parent)
{
    this->alive = true;
}

void MouseBlocker::run()
{
    while (this->alive)
    {
        QCursor::setPos(QCursor::pos());
        QTest::qSleep(100);
    }
}

void MouseBlocker::die()
{
    this->alive = false;
    while (this->isRunning()) QTest::qSleep(10);
}
