/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: RCSFileDialog.cpp,v 1.16 2006/10/22 00:09:08 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"

#include "FWBSettings.h"
#include "RCSFileDialog.h"
#include "RCSFilePreview.h"
#include "RCS.h"

#include <qsplitter.h>

#include <iostream>

using namespace std;

RCSFileDialog::RCSFileDialog( const QString& dirName, const QString& filter,
                              QWidget* parent, const char* name, bool modal )
    : QFileDialog( parent, name, dirName, filter )
{
    if (fwbdebug) qDebug("RCSFileDialog: constructor 1");

    QStringList qsl;

    qsl << "Firewall Builder 4 (2) files (*.fwb)"
        << "Firewall Builder 4 (2) library files (*.fwl)"
        << "Old Firewall Builder files (*.xml)";

    setFilters(qsl);

    setFileMode( QFileDialog::ExistingFile );

    resize( QSize(700, 350) );

    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (!dir.isEmpty()) setDirectory( dir );

    if (fwbdebug) qDebug("RCSFileDialog: checkpoint 1");

/*************************************/
    if (fwbdebug) qDebug("RCSFileDialog: constructor done");
}

RCSFileDialog::RCSFileDialog( QWidget* parent, const char* name, bool modal )
    : QFileDialog(parent, "Open file")// name
{
    if (fwbdebug) qDebug("RCSFileDialog: constructor 2");

    QStringList qsl;

    qsl << "Firewall Builder 4 (2) files (*.fwb)"
        << "Firewall Builder 4 (2) library files (*.fwl)"
        << "Old Firewall Builder files (*.xml)";

    setFilters(qsl);

    resize( QSize(700, 350) );

    QString dir;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir=userDataDir.c_str();
    if (!dir.isEmpty()) setDirectory( dir );

    if (fwbdebug) qDebug("RCSFileDialog: constructor done");
}

void RCSFileDialog::accept()
{
    st->setOpenFileDir( directory().absolutePath() );

    QFileDialog::accept();
}

RCS*  RCSFileDialog::getSelectedRev()
{
    /*RCS *preview_rcs = preview->getSelectedRev();
    if (preview_rcs!=NULL && preview_rcs->getFileName()==selectedFile())
        return preview_rcs; */

    RCS *rcs = new RCS(selectedFiles()[0]);
    return rcs;
}
