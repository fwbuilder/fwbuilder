/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: UsageResolverTest.h 2161 2009-12-16 11:41:47Z a2k $

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

#ifndef IMPORTERTEST_H
#define IMPORTERTEST_H

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Logger.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class ImporterTest : public CppUnit::TestCase
{
    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::Library *lib;
    libfwbuilder::QueueLogger *logger;

public:
    void setUp();
    void IOSImporterTest();
    void IPTImporterTest();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "ImporterTest" );
      suiteOfTests->addTest( new CppUnit::TestCaller<ImporterTest>(
                                   "IOSImporterTest",
                                   &ImporterTest::IOSImporterTest ) );
      suiteOfTests->addTest( new CppUnit::TestCaller<ImporterTest>(
                                   "IPTImporterTest",
                                   &ImporterTest::IPTImporterTest ) );
      return suiteOfTests;
    }
};

#endif // IMPORTERTEST_H
