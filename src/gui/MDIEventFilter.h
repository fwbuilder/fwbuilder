#ifndef MDIEVENTFILTER_H
#define MDIEVENTFILTER_H

#include <QObject>
#include <QEvent>

class MDIEventFilter : public QObject
{
Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MDIEVENTFILTER_H
