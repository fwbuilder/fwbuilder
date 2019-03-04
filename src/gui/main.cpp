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


#include "../../config.h"
#include "global.h"
#include "VERSION.h"
#include "../common/commoninit.h"

#include <getopt.h>

#include <QString>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#   include <QApplication>
#else
#   include <QtWidgets/QApplication>
#endif
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
FWBApplication *app = NULL;
FWWindow *mw = NULL;
FWBSettings *st = NULL;
int fwbdebug = 0;
bool safemode = false;
bool cli_print = false;
QString cli_print_fwname = "";
int sig = FWB_SIG;

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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setGraphicsSystem("native");
#endif
    app = new FWBApplication( argc, argv );
    app->setOrganizationName(QLatin1String("NetCitadel"));
    app->setApplicationName(QLatin1String("Firewall Builder"));

    // can not use "-p" for command line printing because
    // Mac OS X supplies switch "-psnXXXXX" when program is
    // started via Finder.

    int c;
    while ((c = getopt (argc , argv , "1hvf:o:P:dxgr")) != EOF )
	switch (c) {
	case 'h':
	    usage();
	    exit(0);

	case 'f':
	    filename = optarg;
	    break;

	case 'o':
	    print_output_file_name=optarg;
	    break;

        case 'r':
            auto_load_from_rcs_head_revision = true;
            break;

        case 'd':
            fwbdebug++;
            break;

	case 'v':
	    cout << VERSION << endl;
	    exit(0);

        case 'P':
            cli_print = true ;
            cli_print_fwname = optarg;
            break;

        case '1':
            force_first_time_run_flag = true;
            break;
	}

    if ( (argc-1)==optind)
        filename = strdup( argv[optind++] );

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


