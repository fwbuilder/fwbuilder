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

#ifndef DNSTEST_H
#define DNSTEST_H

#include <QObject>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"

class DNSTest : public QObject
{
    Q_OBJECT

    libfwbuilder::FWObjectDatabase *objdb = nullptr;
    libfwbuilder::FWObject *root = nullptr;

    bool testDNSNameObject(libfwbuilder::FWObjectDatabase *objdb,
                           libfwbuilder::FWObject *root,
                           const std::string &dnsrec,
                           const char* results[]);
private slots:
    void init();
    void cleanup();
    void testLocalhost();
    void testSingleIP();
    void testMultipleIPs();
};

#endif // DNSTEST_H
