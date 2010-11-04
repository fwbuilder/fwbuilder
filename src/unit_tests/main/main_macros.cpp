/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: main_IPDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "FWWindow.h"
#include "FWBSettings.h"
#include "common/commoninit.h"
#include "fwbuilder/Resources.h"
#include <QTest>
#include "FWBApplication.h"
#include "UserWorkflow.h"
#include <QFile>

  using namespace std;
  using namespace libfwbuilder;

#define RUN_TEST(x) int fwbdebug = 0;\
FWWindow *mw = NULL; \
FWBSettings *st = NULL; \
FWBApplication *app = NULL; \
UserWorkflow *wfl; \
int sig = FWB_SIG; \
int main( int argc, char** argv) \
{ \
    app = new FWBApplication(argc, argv); \
    app->setOrganizationName(QLatin1String("NetCitadel")); \
    app->setApplicationName(QLatin1String("Firewall Builder")); \
    Q_INIT_RESOURCE(MainRes); \
    st = new FWBSettings(true); \
    st->init(); \
    st->setCheckUpdates(false); \
    st->setBool("UI/NoStartTip", true);  \
    st->setIntroDialogEnabled(false); \
    st->suppressReminderAboutStandardLib(true); \
    init(argv); \
    wfl = new UserWorkflow(); \
    string full_res_path = respath + FS_SEPARATOR + "resources.xml"; \
    try \
    { \
        new Resources(full_res_path); \
    } \
    catch (FWException &e) \
    { \
        cerr << "Can not read resource file from " << full_res_path << endl; \
    } \
    QTest::qExec(x); \
    if (QFile::exists("test_work.fwb")) \
	QFile::remove("test_work.fwb"); \
}
