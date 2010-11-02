/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: FirewallDialogTest.h 2723 2010-03-16 17:32:18Z a2k $

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

#ifndef FirewallDialogTest_H
#define FirewallDialogTest_H

#include <QObject>
#include "ObjectManipulator.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Cluster.h"

class FirewallDialogTest : public QObject
{
    Q_OBJECT
    libfwbuilder::Library* findUserLibrary();
    ObjectManipulator *om;
    libfwbuilder::Firewall *firewall;
    bool dialog_rejected;
private slots:
    void initTestCase();
    void testDialog();

public slots:
    void rejectDialog();
};

#endif // FirewallDialogTest_H
