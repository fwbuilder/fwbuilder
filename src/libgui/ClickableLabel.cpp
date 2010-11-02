#include "ClickableLabel.h"

ClickableLabel::ClickableLabel(QWidget * parent, Qt::WindowFlags)
{
    setParent(parent);
}

void ClickableLabel::mousePressEvent ( QMouseEvent *)
{
    emit clicked();
}
