#ifndef FIREWALLSELECTORWIDGET_H
#define FIREWALLSELECTORWIDGET_H

#include <list>

#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QMap>

#include <fwbuilder/Firewall.h>

struct TableRow
{
    libfwbuilder::Firewall* firewall;
    QTableWidgetItem* title;
    QCheckBox* use;
    QRadioButton* master;
};

class FirewallSelectorWidget : public QTableWidget
{
public:
    FirewallSelectorWidget(QWidget *parent = NULL);

    void setFirewallList(std::list<libfwbuilder::Firewall*>);

    //            pointer to firewall , master
    QList<QPair<libfwbuilder::Firewall*, bool> > getSelectedFirewalls();

private:
    QMap<QCheckBox*, int> boxrow;
    QMap<QRadioButton*, int> radiorow;
    QMap<int, TableRow> rows;

public slots:
    void usageChanged(bool);
};

#endif // FIREWALLSELECTORWIDGET_H
