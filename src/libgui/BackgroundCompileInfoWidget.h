/*

                          Firewall Builder

                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#ifndef BACKGROUNDCOMPILEINFOWIDGET_H
#define BACKGROUNDCOMPILEINFOWIDGET_H

#include <QWidget>

class instDialog;
class FWWindow;
class QProgressBar;
class QLabel;

class BackgroundCompileInfoWidget : public QWidget
{
    Q_OBJECT
public:
    BackgroundCompileInfoWidget(QWidget *parent, instDialog *instDialog, FWWindow *fwwindow);
private:
    FWWindow *m_fwwindow;
    QProgressBar *m_progressBar;
    QLabel *m_label;
public slots:
    void autoCompileIsDone();
    
};

#endif // BACKGROUNDCOMPILEINFOWIDGET_H
