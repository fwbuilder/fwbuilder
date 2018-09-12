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
#include "utils.h"

#include "debugDialog.h"
#include "RCS.h"
#include "ProjectPanel.h"
#include "FWWindow.h"

#include <qtextbrowser.h>
#include <qtextcodec.h>
#include <qglobal.h>
#include <qpixmapcache.h>
#include <qlocale.h>

#include "fwbuilder/Constants.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;


debugDialog::debugDialog(QWidget *parent) : QDialog(parent)
{
    m_dialog = new Ui::debugDialog_q;
    m_dialog->setupUi(this);

/*
 * some variables used for remote debugging (so I can ask the user to
 * send me a screenshot of the "about" dialog and get the idea about
 * their environment etc.)
 */
    m_dialog->debugText->append( QString("Path to executable: %1")
        .arg(argv0.c_str()) );
    m_dialog->debugText->append( QString("Path to resources: %1")
        .arg(Constants::getResourcesDirectory().c_str()) );
    m_dialog->debugText->append( QString("Path to locale: %1")
        .arg(Constants::getLocaleDirectory().c_str()) );
    m_dialog->debugText->append( QString("Path to libfwbuilder data directory: %1")
        .arg(Constants::getDTDDirectory().c_str()) );
    m_dialog->debugText->append( QString("appRootDir: %1")
        .arg(appRootDir.c_str()) );
    m_dialog->debugText->append( "\n" );
    m_dialog->debugText->append( QString("standard objects library: %1")
                                 .arg(Constants::getStandardObjectsFilePath().c_str()));
    m_dialog->debugText->append( QString("standard templates library: %1")
                                 .arg(Constants::getTemplatesObjectsFilePath().c_str()));
    m_dialog->debugText->append( QString("user name: %1").arg(user_name) );
    m_dialog->debugText->append( "\n" );
    m_dialog->debugText->append( QString("Path to rcs: %1").arg(RCS::getRcsFileName()));
    m_dialog->debugText->append( QString("Path to rcsdiff: %1").arg(RCS::getRcsdiffFileName()));
    m_dialog->debugText->append( QString("Path to rlog: %1").arg(RCS::getRlogFileName()));
    m_dialog->debugText->append( QString("Path to ci: %1").arg(RCS::getCiFileName()));
    m_dialog->debugText->append( QString("Path to co: %1").arg(RCS::getCoFileName()));
    m_dialog->debugText->append( "\n" );

    m_dialog->debugText->append( "RCS timezone setting:" );
    m_dialog->debugText->append( RCS::getRCSEnvFix()->getTZOffset() );
    m_dialog->debugText->append( "\n" );
    m_dialog->debugText->append( "RCS environment:" );
    m_dialog->debugText->append( RCS::getEnv()->join("\n").toLatin1() );
    m_dialog->debugText->append( "\n" );

    m_dialog->debugText->append( QString("Current locale: %1")
        .arg(QLocale::system().name()) );
    m_dialog->debugText->append( "\n" );
    m_dialog->debugText->append( QString("Versions:") );
    m_dialog->debugText->append( QString("  Firewall Builder %1").arg(VERSION));
    m_dialog->debugText->append( QString("  Data format version %1")
        .arg(libfwbuilder::Constants::getDataFormatVersion().c_str() ) );

    m_dialog->debugText->append( QString("  Built with QT %1").arg(QT_VERSION_STR));
    m_dialog->debugText->append( QString("  Using QT %1").arg( qVersion() ) );
    m_dialog->debugText->append( QString("  Built with libxml2 %1")
        .arg(LIBXML_DOTTED_VERSION) );
#if !defined(Q_OS_MACX)
    m_dialog->debugText->append( QString("  Using libxml2 %1").arg(xmlParserVersion));
#endif
    m_dialog->debugText->append( "\n" );

    m_dialog->debugText->append( QString("FWObjectDatabase index statistics:"));

    if (mw->activeProject())
    {
        int s,h,m;
        mw->activeProject()->db()->getIndexStats(s,h,m);
        m_dialog->debugText->append( QString("  index size: %1 records").arg(s) );
        m_dialog->debugText->append( QString("  hits: %1").arg(h) );
        m_dialog->debugText->append( QString("  misses: %1").arg(m) );
        m_dialog->debugText->append( "\n" );
    }

    m_dialog->debugText->append( QString("QPixmapCache limit: %1 kb")
        .arg(QPixmapCache::cacheLimit()));
}

debugDialog::~debugDialog()
{
    delete m_dialog;
}
