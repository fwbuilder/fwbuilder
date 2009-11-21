#include "FirewallSelectorWidget.h"

using namespace std;
using namespace libfwbuilder;

FirewallSelectorWidget::FirewallSelectorWidget(QWidget *parent):
        QTableWidget(parent)
{
}

void FirewallSelectorWidget::setFirewallList(list<Firewall*> firewalls)
{
    this->clear();
    Firewall *fw;
    for (list<Firewall*>::iterator it = firewalls.begin(); it != firewalls.end(); it++)
    {
        fw = *it;
        QTableWidgetItem *title = new QTableWidgetItem(QIcon(":/Icons/Firewall/icon"), QString(fw->getName().c_str()));
        QCheckBox *box = new QCheckBox(this);
        connect(box, SIGNAL(toggled(bool)), this, SLOT(usageChanged(bool)));
        QRadioButton *radio = new QRadioButton(this);
        radio->setEnabled(false);
        TableRow rw;
        rw.firewall = fw;
        rw.master = false;
        rw.title = title;
        rw.use = false;
        int rowNumber = this->rowCount();
        this->insertRow(rowNumber);
        rows[rowNumber] = rw;
        this->setItem(rowNumber, 0, title);
        this->setCellWidget(rowNumber, 1, box);
        this->setCellWidget(rowNumber, 2, radio);
    }
}


void FirewallSelectorWidget::usageChanged(bool st)
{
    int row = this->boxrow[dynamic_cast<QCheckBox*>(sender())];
    if (!st) rows[row].master->setChecked(false);
    rows[row].master->setEnabled(st);
    bool isMaster = false, isSelected = false;
    foreach(TableRow row, this->rows)
    {
        if (row.use->isChecked()) isSelected = true;
        if (row.master->isChecked()) isMaster = true;
    }
    if (isSelected && !isMaster)
    {
        foreach(TableRow row, this->rows)
        {
            if (row.use->isChecked())
            {
                row.master->setChecked(true);
                break;
            }
        }
    }
}

QList<QPair<Firewall*, bool> > FirewallSelectorWidget::getSelectedFirewalls()
{
    QList<QPair<Firewall*, bool> > res;
    foreach(TableRow row, this->rows)
    {
        if (row.use->isChecked())
            res.append(qMakePair(row.firewall, row.master->isChecked()));
    }
    return res;
}
