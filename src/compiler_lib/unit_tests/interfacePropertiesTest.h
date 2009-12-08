/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: interfaceProperties.h 2043 2009-12-06 01:10:10Z a2k $

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


#ifndef INTERFACEPROPERTIESTEST
#define INTERFACEPROPERTIESTEST

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "../interfaceProperties.h"
#include "../interfacePropertiesObjectFactory.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <list>
#include <string>



class interfacePropertiesTest: public CppUnit::TestCase
{
    libfwbuilder::FWObjectDatabase *db;
public:
  //interfaceProperties( std::string name ) : CppUnit::TestCase( name ) {}

  void validateInterface();
  void isEligibleForCluster();
  void setUp();

  static CppUnit::Test *suite()
  {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "interfaceProperties" );
    suiteOfTests->addTest( new CppUnit::TestCaller<interfacePropertiesTest>(
                                 "validateInterface",
                                 &interfacePropertiesTest::validateInterface ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<interfacePropertiesTest>(
                                 "validateInterface",
                                 &interfacePropertiesTest::isEligibleForCluster ) );
    return suiteOfTests;
  }

};

#endif

