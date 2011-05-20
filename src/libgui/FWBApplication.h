/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __FWBAPPLICATION_H_
#define __FWBAPPLICATION_H_

#include <QApplication>


class FWBApplication : public QApplication
{

    Q_OBJECT;
    int timeout;
    
public:
    FWBApplication(int &argc, char **argv) : QApplication(argc, argv) {}

    virtual bool notify(QObject *receiver, QEvent *event);
    
public slots:

    void quit();
    void delayedQuit();

};

#endif

