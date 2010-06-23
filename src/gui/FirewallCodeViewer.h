#ifndef FIREWALLCODEVIEWER_H
#define FIREWALLCODEVIEWER_H

#include <QDialog>

namespace Ui {
    class FirewallCodeViewer_q;
}

class FirewallCodeViewer : public QDialog {
    Q_OBJECT
    QStringList files;
public:
    FirewallCodeViewer(QStringList files, QString path, QWidget *parent = 0);
    ~FirewallCodeViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FirewallCodeViewer_q *ui;
public slots:
    void fileSelected(int);
};

#endif // FIREWALLCODEVIEWER_H
