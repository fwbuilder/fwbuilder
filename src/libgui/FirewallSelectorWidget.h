/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id$

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef FIREWALLSELECTORWIDGET_H
#define FIREWALLSELECTORWIDGET_H

#include <list>

#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QMessageBox>
#include <QMap>


#include <fwbuilder/Interface.h>
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
    Q_OBJECT
public:
    FirewallSelectorWidget(QWidget *parent = nullptr);
    ~FirewallSelectorWidget();

    void setFirewallList(std::list<libfwbuilder::Firewall*>, bool select = false);

    //            pointer to firewall , master
    QList<QPair<libfwbuilder::Firewall*, bool> > getSelectedFirewalls();
    bool isValid();

    virtual void resizeEvent (QResizeEvent*);

private:
    QMap<QCheckBox*, int> boxrow;
    QMap<QRadioButton*, int> radiorow;
    QMap<int, TableRow> rows;

public slots:
    void usageChanged(bool);
    void clear();
};

#endif // FIREWALLSELECTORWIDGET_H
