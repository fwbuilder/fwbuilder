/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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
#include "FWBApplication.h"
#include "FWObjectClipboard.h"

#include "common/commoninit.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/Constants.h"

#include <QString>
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QTest>
#include <QFile>


using namespace std;
using namespace libfwbuilder;

extern void init_platforms();  // defined in platforms.cpp


void build_app(int argc, char** argv,
               FWBApplication** app,
               FWBSettings** st)

{ 
    Q_INIT_RESOURCE(MainRes); 

    init(argv); 
    init_platforms();

    *st = new FWBSettings(true); 
    (*st)->init(false); 
    (*st)->setCheckUpdates(false); 
    (*st)->setBool("UI/NoStartTip", true);  
    (*st)->setIntroDialogEnabled(false); 
    (*st)->suppressReminderAboutStandardLib(true); 

    string full_res_path = Constants::getResourcesFilePath(); 

    new Resources(full_res_path);

    QString qt_resource_dir =
        QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    QTranslator qt_translator(0);
    qt_translator.load(QLatin1String("qt_") + QLocale::system().name(),
                       qt_resource_dir);
    (*app)->installTranslator (&qt_translator);
}
