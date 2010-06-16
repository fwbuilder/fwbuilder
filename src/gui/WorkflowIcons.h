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
    ~WorkflowIcons();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WorkflowIcons_q *ui;

signals:
    void newFirewall();
    void importConfig();
    void watchTutorial(QString);

public slots:
    void itemClicked();
};

#endif // WORKFLOWICONS_H
