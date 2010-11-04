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

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include "FWBTreeTest.h"

int fwbdebug;


int main(int /*UNUSED argc*/, char ** /*UNUSED argv[]*/)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( FWBTreeTest::suite() );
    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
                                                         std::cerr ) );

    runner.run();
    return 0;
}
