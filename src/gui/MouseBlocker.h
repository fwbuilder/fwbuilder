#ifndef MOUSEBLOCKER_H
#define MOUSEBLOCKER_H

#include <QThread>

class MouseBlocker : public QThread
{
Q_OBJECT
    bool alive;
public:
    explicit MouseBlocker(QObject *parent = 0);
    void run();
    void die();
signals:

public slots:

};

#endif // MOUSEBLOCKER_H
