#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    ClickableLabel(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
#else
    ClickableLabel(QWidget *parent = 0, Qt::WindowFlags f = 0 );
#endif
    void mousePressEvent(QMouseEvent *);

signals:
    void clicked();
};

#endif // CLICKABLELABEL_H
