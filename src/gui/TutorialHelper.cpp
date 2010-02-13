#include "TutorialHelper.h"
#include <QTest>
#include <QDebug>
#include <QMenu>

TutorialHelper::TutorialHelper()
{
}

void TutorialHelper::clickWidget(QWidget *w)
{
    QTest::mouseClick(w, Qt::LeftButton, Qt::NoModifier);
}

void TutorialHelper::clickMenuItem(QMenu *menu, QPoint pos)
{
    QTest::mouseClick(menu, Qt::LeftButton, Qt::NoModifier, pos);
}

void TutorialHelper::typeWidget(QWidget *w, QString text)
{
    QTest::keyClicks(w, text);
}
