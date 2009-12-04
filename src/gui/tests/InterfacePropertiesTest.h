/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: InterfaceEditorWidget.cpp 2006 2009-12-02 19:10:48Z a2k $

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

#ifndef INTERFACEPROPERTIESTEST_H
#define INTERFACEPROPERTIESTEST_H

#include <QtTest/QtTest>

class InterfacePropertiesTest : public QObject
{
Q_OBJECT
public:
    explicit InterfacePropertiesTest(QObject *parent = 0);

signals:

private slots:
    void validateInterface();
    void validateInterface_data();

};

#endif // INTERFACEPROPERTIESTEST_H
