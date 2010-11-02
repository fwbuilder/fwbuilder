
/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: WorkflowIcons.h 2786 2010-04-01 14:05:36Z a2k $

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

public slots:
    void libraryAccessChanged(bool writable);
    void openTutorial();
};

#endif // WORKFLOWICONS_H
