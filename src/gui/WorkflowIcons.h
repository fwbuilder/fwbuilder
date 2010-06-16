#ifndef WORKFLOWICONS_H
#define WORKFLOWICONS_H

#include <QWidget>

namespace Ui {
    class WorkflowIcons_q;
}

class WorkflowIcons : public QWidget {
    Q_OBJECT
public:
    WorkflowIcons(QWidget *parent = 0);
    void setUpSignals(QWidget *panel);
    ~WorkflowIcons();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WorkflowIcons_q *ui;
};

#endif // WORKFLOWICONS_H
