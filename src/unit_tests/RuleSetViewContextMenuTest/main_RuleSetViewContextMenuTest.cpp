/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: main_instDialogTest.cpp 2707 2010-03-10 18:22:19Z a2k $

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

#include "RuleSetViewContextMenuTest.h"


#include <QTest>
#include <QFile>

#include "FWWindow.h"
#include "FWBSettings.h"
#include "FWBApplication.h"


using namespace std;
using namespace libfwbuilder;

int fwbdebug = 0;
FWWindow *mw = nullptr; 
FWBSettings *st = nullptr; 
FWBApplication *app = nullptr; 
int sig = FWB_SIG; 


extern void build_app(int argc, char** argv,
                      FWBApplication** app,
                      FWBSettings** st);

int main(int argc, char** argv)
{ 
    app = new FWBApplication(argc, argv); 
    app->setOrganizationName(QLatin1String("NetCitadel")); 
    app->setApplicationName(QLatin1String("Firewall Builder")); 

    build_app(argc, argv, &app, &st);


    QTest::qExec(new RuleSetViewContextMenuTest()); 

    if (QFile::exists("test_work.fwb")) 
        QFile::remove("test_work.fwb"); 
}
