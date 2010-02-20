#include "TutorialAnimator.h"
#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "platforms.h"
#include "FWWindow.h"
#include <QApplication>
#include <QDebug>
#include <QComboBox>
#include <QTest>
#include <QTimer>

Q_DECLARE_METATYPE(QMenu*);

using namespace libfwbuilder;
using namespace std;

TutorialAnimator::TutorialAnimator(QObject *parent, QString commands) :
    QThread(parent)
{
    qRegisterMetaType<QMenu*>("QMenu*");

//    widget = new QWidget(dynamic_cast<QWidget*>(this->parent()));
//    widget->setFocus(Qt::ActiveWindowFocusReason);
    //dynamic_cast<QWidget*>(parent)->grabKeyboard();
    //dynamic_cast<QWidget*>(parent)->grabMouse();
    dynamic_cast<QWidget*>(parent)->hide();
    currentCommand = 0;
    this->commands = commands.split('\n');

    helper = new TutorialHelper();
    connect(this, SIGNAL(finished()), this, SLOT(scenarioFinished()));
    this->speed = 50;
}

void TutorialAnimator::scenarioFinished()
{
    //dynamic_cast<QWidget*>(this->parent())->releaseKeyboard();
    //dynamic_cast<QWidget*>(this->parent())->releaseMouse();

    dynamic_cast<QWidget*>(this->parent())->show();
    /*
    dynamic_cast<QWidget*>(this->parent())->grabMouse();
    dynamic_cast<QWidget*>(this->parent())->grabKeyboard();
    dynamic_cast<QWidget*>(this->parent())->releaseKeyboard();
    dynamic_cast<QWidget*>(this->parent())->releaseMouse();
    */
    dynamic_cast<QWidget*>(this->parent())->raise();
}

void TutorialAnimator::run()
{
    for (int i=0; i<this->commands.count(); i++)
    {
        animate(i);
        QTest::qWait(speed*20);
    }
    //w->releaseKeyboard();
    //w->releaseMouse();
}

void TutorialAnimator::animate(int command)
{
    if (command == -1) command = currentCommand;
    QStringList input = commands.at(command).split(QRegExp("\\s+"));
    for(int i=0; i<input.size(); i++)
    {
        QString str = input.at(i);
        input[i] = str.replace(QRegExp("\\\\_"), " ");
    }
    QString baseCommand = input.first().trimmed();
    input.removeFirst();
    if (baseCommand == "moveMouse")
        moveMouse(input); // input: widget_tree OR x y
    if (baseCommand == "clickWidget")
        clickWidget(input); // input: widget_tree
    if (baseCommand == "typeWidget")
        typeWidget(input); // input: widget_tree text
    if (baseCommand == "hoverMenuItem")
        hoverMenuItem(input); // input: menu_name item_name
    if (baseCommand == "clickMenuItem")
        clickMenuItem(input); // input: menu_name item_name
    if (baseCommand == "selectComboItem")
        selectComboItem(input); // input: widget_tree item_index
    if (baseCommand == "selectListItem")
        selectListItem(input); // input: widget_tree item_index
    if (baseCommand == "selectTab")
        selectTab(input); // input: widget_tree item_index
    if (baseCommand == "wait")
        wait(input);
}

QObject* TutorialAnimator::findChild(QObject *parent, QString name)
{
    bool lookingForClass = false;
    if (name.startsWith("w#"))
    {
        name.remove(0, 2);
        lookingForClass = true;
        qDebug() << "looking for class" << name;
    }
    QObjectList queue;
    queue.append(parent);
    while(queue.size())
    {
        QObject *obj = queue.first();
        queue.removeFirst();
        if ( ( (!lookingForClass) && (obj->objectName() == name) ) ||
             ( (lookingForClass) && (obj->metaObject()->className() == name) ) )
        {
            return obj;
        }
        else
        {
            queue += obj->children();
        }
    }
    return NULL;
}

QWidget* TutorialAnimator::findWidget(QString name)
{
    bool lookingForClass = false;
    if (name.startsWith("w#"))
    {
        name.remove(0, 2);
        lookingForClass = true;
    }
    foreach(QWidget *w, app->allWidgets())
    {
        if ( ( (!lookingForClass) && (w->objectName() == name) ) ||
             ( (lookingForClass) && (w->metaObject()->className() == name) ) )
            return w;
    }
    return NULL;
}

QObject* TutorialAnimator::getWidget(QStringList input)
{
    QObject *obj = findChild(mw, input.first());
    if (obj == NULL)
    {
        obj = findWidget(input.first());
        if (obj == NULL)
        {
            qDebug() << "could not find such object";
            return NULL;
        }
    }
    input.removeFirst();
    if (!input.empty())
    foreach (QString name, input)
    {
        obj = findChild(obj, name);
        if (obj == NULL)
        {
            qDebug() << "could not find such object";
            return NULL;
        }
    }
    if (obj == NULL)
    {
        qDebug() << "could not find such object";
        return NULL;
    }
    return obj;
}


void TutorialAnimator::moveMouse(QStringList input)
{
    qDebug() << "input:" << input;
    // is first two arguments are ints, click mouse at this coordintates
    bool isInt1 = false, isInt2 = false;
    int x, y;
    if (input.size() == 2)
    {
        x = input.at(0).toInt(&isInt1, 10);
        y = input.at(1).toInt(&isInt2, 10);
    }
    if (isInt1 && isInt2)
    {
        qDebug() << "moving mouse to (" << x << "," << y << ")";
        helper->moveMouse(QPoint(x,y));
    }
    // if not, they should be widget names
    // find widget with name of last argument who's parent is previous argument recursively
    else
    {
        QObject *obj = getWidget(input);
        helper->moveMouse(dynamic_cast<QWidget*>(obj));
    }
}

QPoint TutorialAnimator::findMenuItemPos(QMenu *menu, QString item)
{
    int x = menu->width()/2;
    int top, bottom;
    for (int i=1; i<menu->height()-1; i++)
    {
        QAction *action = menu->actionAt(QPoint(x,i));
        if (action == NULL) continue;
        if (action->text() == item)
        {
            top = i;
            break;
        }
    }
    for (int i=menu->height()-1; i>1; i--)
    {
        QAction *action = menu->actionAt(QPoint(x,i));
        if (action == NULL) continue;
        if (action->text() == item)
        {
            bottom = i;
            break;
        }
    }

    if (top==bottom) return QPoint();
    return QPoint(x, (top+bottom)/2);
}

QWidget* TutorialAnimator::topLevelWindow(QString className)
{
    foreach (QWidget *w, app->topLevelWidgets())
    {
        if (w->metaObject()->className() == className)
            return w;
    }
    return NULL;
}

void TutorialAnimator::hoverMenuItem(QStringList input)
{
    QMenu *menu = dynamic_cast<QMenu*>(findWidget(input.first()));
    if (menu == NULL)
    {
        qDebug() << "could not find menu" << input.first();
        return;
    }
    QPoint pos = findMenuItemPos(menu, input.at(1));
    helper->moveMouse(dynamic_cast<QWidget*>(menu), pos);
}

void TutorialAnimator::clickMenuItem(QStringList input)
{
    QMenu *menu = dynamic_cast<QMenu*>(findWidget(input.first()));
    if (menu == NULL)
    {
        qDebug() << "could not find menu" << input.first();
        return;
    }
    QPoint pos = findMenuItemPos(menu, input.at(1));
    QTest::mouseMove(menu, pos);
    QMetaObject::invokeMethod(helper, "clickMenuItem", Qt::QueuedConnection,
                              Q_ARG(QMenu*, menu), Q_ARG(QPoint, pos));
}

void TutorialAnimator::wait(QStringList input)
{
    if (input.size() < 1)
    {
        qDebug() << "Wrong command";
        return;
    }
    bool isInt = false;
    int n = input.first().toInt(&isInt, 10);
    if (isInt == false)
    {
        qDebug() << "Wrong command";
        return;
    }
    QTest::qWait(n);
}

void TutorialAnimator::clickWidget(QStringList input)
{
    if (input.size() < 1)
    {
        qDebug() << "Wrong command";
        return;
    }
    QWidget *w = dynamic_cast<QWidget*>(getWidget(input));
    if (w == NULL)
    {
        qDebug() << "Could not find widget:" << input;
        return;
    }
    QTest::mouseMove(w);
    QMetaObject::invokeMethod(helper, "clickWidget", Qt::QueuedConnection, Q_ARG(QWidget*, w));
}

void TutorialAnimator::typeWidget(QStringList input)
{
    if (input.size() < 2)
    {
        qDebug() << "Wrong command";
        return;
    }
    QString text = input.last();
    input.pop_back();
    QWidget *w = dynamic_cast<QWidget*>(getWidget(input));
    if (w == NULL)
    {
        qDebug() << "Could not find widget:" << input;
        return;
    }
    QMetaObject::invokeMethod(helper, "typeWidget", Qt::QueuedConnection,
                              Q_ARG(QWidget*, w), Q_ARG(QString, text));
    QTest::qWait(text.length()*speed);
}

void TutorialAnimator::selectComboItem(QStringList input)
{
    QString idstr = input.last();
    bool isId;
    int id = idstr.toInt(&isId, 10);
    input.pop_back();
    QComboBox *combo = dynamic_cast<QComboBox*>(this->getWidget(input));
    if (isId)
        QMetaObject::invokeMethod(helper, "selectComboItem", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(int, id));
    else
        QMetaObject::invokeMethod(helper, "selectComboItem", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(QString, idstr));
}

void TutorialAnimator::selectListItem(QStringList input)
{
    QString idstr = input.last();
    bool isId;
    int id = idstr.toInt(&isId, 10);
    input.pop_back();
    qDebug() << "view:" << this->getWidget(input);
    QAbstractItemView *combo = dynamic_cast<QAbstractItemView*>(this->getWidget(input));
    qDebug() << "selecting list item:" << combo;
    if (isId)
        QMetaObject::invokeMethod(helper, "selectListItem", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(int, id));
    else
        QMetaObject::invokeMethod(helper, "selectListItem", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(QString, idstr));
}

void TutorialAnimator::selectTab(QStringList input)
{
    QString idstr = input.last();
    bool isId;
    int id = idstr.toInt(&isId, 10);
    input.pop_back();
    qDebug() << "view:" << this->getWidget(input);
    QTabWidget *combo = dynamic_cast<QTabWidget*>(this->getWidget(input));
    qDebug() << "selecting tab in widget:" << combo;
    if (isId)
        QMetaObject::invokeMethod(helper, "selectTab", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(int, id));
    else
        QMetaObject::invokeMethod(helper, "selectTab", Qt::BlockingQueuedConnection,
                                  Q_ARG(QWidget*, dynamic_cast<QWidget*>(combo)), Q_ARG(QString, idstr));
}

void TutorialAnimator::setSpeed(int speed)
{
    this->speed = speed;
    this->helper->speed = speed;
}
