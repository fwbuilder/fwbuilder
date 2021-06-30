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

#ifndef FWOBJECTDATABASETEST_H
#define FWOBJECTDATABASETEST_H

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Library.h"
#include "FWBTree.h"

#include <QObject>

class UsageResolverTest : public QObject
{
    Q_OBJECT

    void addToLib(libfwbuilder::FWObject* obj);

    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::Library *lib;
    libfwbuilder::IPv4 *addr1;
    libfwbuilder::IPv4 *addr2;
    libfwbuilder::IPv4 *addr3;
    libfwbuilder::PolicyRule *r1, *r2, *r3, *r4;

private slots:
    void init();

    void findWhereObjectIsUsed();
    void findFirewallsForObject();
    void humanizeSearchResults();

};

#endif // FWOBJECTDATABASETEST_H
