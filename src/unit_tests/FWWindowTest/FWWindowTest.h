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

#ifndef FWWINDOWTEST_H
#define FWWINDOWTEST_H

#include <QMainWindow>
#include <QTest>
#include <QMessageBox>

class FWWindowTest : public QMainWindow
{
    Q_OBJECT;

public:
    FWWindowTest(QWidget *parent = 0);

    public slots:
    void findMessageBox();
    void findNoMessageBox();
    void prepareMW();
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void checkForUpgrade_1();
    void checkForUpgrade_2();
    void checkForUpgrade_3();
    void checkForUpgrade_4();
};

#endif // FWWINDOWTEST_H
