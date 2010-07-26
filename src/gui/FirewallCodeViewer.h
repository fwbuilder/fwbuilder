/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#ifndef FIREWALLCODEVIEWER_H
#define FIREWALLCODEVIEWER_H

#include <QDialog>
#include <QFrame>
#include <QKeyEvent>
#include <QMap>

namespace Ui {
    class FirewallCodeViewer_q;
}

class FirewallCodeViewer : public QDialog {
    Q_OBJECT
    QStringList files;
    QMap<int, int> pages;
public:
    FirewallCodeViewer(QStringList files, QString path, QWidget *parent = 0);
    ~FirewallCodeViewer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FirewallCodeViewer_q *ui;
public slots:
    void fileSelected(int);
    void hideCloseButton();
    void keyPressEvent(QKeyEvent *event)
    {
        if (dynamic_cast<QFrame*>(this->parent()) == NULL)
            return QDialog::keyPressEvent(event);
        event->setAccepted(false);
    }

};

#endif // FIREWALLCODEVIEWER_H
