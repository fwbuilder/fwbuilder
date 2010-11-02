#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = 0, Qt::WindowFlags f = 0 );
    void mousePressEvent(QMouseEvent *);

signals:
    void clicked();
};

#endif // CLICKABLELABEL_H
