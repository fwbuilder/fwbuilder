/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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


#include "global.h"
#include "../common/commoninit.h"

#include <QCommandLineParser>

#include <QString>
#include <QtWidgets/QApplication>
#include <QTimer>
#include <QPixmapCache>
#include <QTextCodec>
#include <QLocale>
#include <QDir>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtDebug>

#include "FWBApplication.h"
#include "FWBSettings.h"
#include "RCS.h"
#include "FWWindow.h"
#include "FWObjectClipboard.h"
#include "FWBTree.h"
#include "platforms.h"
#include "ObjectEditor.h"
#include "findDialog.h"
#include "ProjectPanel.h"
#include "RCS.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Tools.h"
#include "fwbuilder/dns.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Constants.h"


#if defined(Q_WS_MAC)
extern void connectOdocHandler();
#endif

#ifndef INIT2
#define INIT2
#endif

extern void extras();
extern void ssh_wrapper(int argc, char *argv[]);


using namespace libfwbuilder;
using namespace std;

static QString filename;
static QString print_output_file_name;
bool auto_load_from_rcs_head_revision = false;
FWBApplication *app = nullptr;
FWWindow *mw = nullptr;
FWBSettings *st = nullptr;
int fwbdebug = 0;
bool safemode = false;
bool cli_print = false;
QString cli_print_fwname = "";

void usage()
{
    cerr << "Usage: fwbuilder [-hv] [-P object_name] [-o file_name] [-r] [ [-f] filename]\n";
    cerr << endl;
}

int main( int argc, char *argv[] )
{
    cout << "Firewall Builder GUI " VERSION << endl << flush;

    filename = "";
    print_output_file_name = "";
    fwbdebug = 0;
    safemode = false;

    bool force_first_time_run_flag = false;

    ssh_wrapper(argc, argv);

    //QApplication::setDesktopSettingsAware(desktopaware);
 
    Q_INIT_RESOURCE(MainRes);
    app = new FWBApplication( argc, argv );
    app->setOrganizationName(QLatin1String("NetCitadel"));
    app->setApplicationName(QLatin1String("Firewall Builder"));

    // can not use "-p" for command line printing because
    // Mac OS X supplies switch "-psnXXXXX" when program is
    // started via Finder.

    QCommandLineParser parser;
    parser.addOption({"h", "help"});
    parser.addOption({"v", "version"});
    parser.addOption({"f", "filename"});

    QCommandLineOption fileNameOption("f", "file_name", "file_name");
    parser.addOption(fileNameOption);

    QCommandLineOption outputFileNameOption("o", "file_name", "output_file_name");
    parser.addOption(outputFileNameOption);

    QCommandLineOption autoLoadFromRcsHeadRevisionOption("r");
    parser.addOption(autoLoadFromRcsHeadRevisionOption);

    QCommandLineOption debugOption("d");
    parser.addOption(debugOption);

    QCommandLineOption objectNameOption("P", "object_name", "object_name");
    parser.addOption(objectNameOption);

    QCommandLineOption forceFirstRunFlagOption("1");
    parser.addOption(forceFirstRunFlagOption);

    parser.process(*app);

    if (parser.isSet("h")) {
        usage();
        exit(0);
    }

    if (parser.isSet("v")) {
        cout << VERSION << "\n";
        exit(0);
    }

    if (parser.isSet(fileNameOption)) {
        filename = parser.value(fileNameOption);
    }

    if (parser.isSet(objectNameOption)) {
        cli_print = true;
        cli_print_fwname = parser.value(objectNameOption);
    }

    if (parser.isSet(outputFileNameOption)) {
        print_output_file_name = parser.value(outputFileNameOption);
    }

    if (parser.isSet(autoLoadFromRcsHeadRevisionOption)) {
        auto_load_from_rcs_head_revision = true;
    }

    if (parser.isSet(debugOption)) {
        fwbdebug++;
    }

    if (parser.isSet(forceFirstRunFlagOption)) {
        force_first_time_run_flag = true;
    }

    auto positionalArguments = parser.positionalArguments();
    if (positionalArguments.size()) {
        filename = positionalArguments.at(0);
    }

    if (fwbdebug) qDebug("Initializing ...");


/* need to initialize in order to be able to use FWBSettings */
    init(argv);
    init_platforms();
    RCS::init();

    if (fwbdebug) qDebug("Reading settings ...");
    st = new FWBSettings();
    st->init(force_first_time_run_flag);
    FWObject::setDataDir(st->getDataDir().toUtf8().constData());
    if (fwbdebug) qDebug("done");

    QPixmapCache::setCacheLimit(4096);

//    INIT2;

    string full_res_path = Constants::getResourcesFilePath();

    if (fwbdebug)
    {
        qDebug("reading resources from '%s' ...", full_res_path.c_str());
    }

    QFileInfo fi(full_res_path.c_str());
    if (!fi.exists())
    {
        qDebug() << QString("Resource file %1 does not exist").arg(fi.filePath());
        exit(1);
    }

    Resources res(full_res_path);

    if (fwbdebug) qDebug("done");

    vector<std::string> platforms = Resources::getListOfPlatforms();
    if (platforms.empty() || (
            platforms.size()==1 && platforms.front()=="unknown" ))
    {
        qDebug("Failed to load list of supported platforms");
        exit(1);
    }

    if (cli_print)
    {
        if (fwbdebug) qDebug("Print from command line");
        FWWindow::printFirewallFromFile(filename,
                                        cli_print_fwname,
                                        print_output_file_name);
        return 0;
    }
    

    if (fwbdebug) qDebug("creating widgets ...");

    //new FWObjectDatabase();
    new FWObjectClipboard();

    if (fwbdebug) qDebug("loading translation for the current locale ...");

    QString local = QLocale::system().name();//"en_US";//
    QTranslator translator(0);
    translator.load(QLatin1String("fwbuilder_") +
                    QString(local), Constants::getLocaleDirectory().c_str());
    app->installTranslator (&translator);

    QString qt_resource_dir =
        QLibraryInfo::location(QLibraryInfo::TranslationsPath);


    QTranslator qt_translator(0);
    qt_translator.load(QLatin1String("qt_") + QLocale::system().name(),
                       qt_resource_dir);
    app->installTranslator (&qt_translator);

    mw  = new FWWindow();
    //mw->setSafeMode(safemode);
    if (filename != "")
        mw->registerAutoOpenDocFile(filename, auto_load_from_rcs_head_revision);

    mw->show();

    // it is connected to QApplication's slot, not to FWBApplication
    app->setQuitOnLastWindowClosed(false);
    // connecting to right slot
    app->connect(app, SIGNAL(lastWindowClosed()), app, SLOT(quit()));

#if defined(Q_WS_MAC)
    connectOdocHandler();
#endif

    // setup single shot timer to call startupLoad()
    QTimer::singleShot(500, mw, SLOT(startupLoad()));

    try
    {
        app->exec();
    } catch (const libfwbuilder::FWException &ex)
    {
        qDebug() << "Caught FWException: " << ex.toString().c_str();
    }

    mw->hide();  // must do this before settings object is destroyed

    st->save();
    delete st;

    res.clear();

    XMLTools::close();

    // We need to call FWWindow::~FWWindow() to remove temporary directory
    delete mw;

    return 0;
}


