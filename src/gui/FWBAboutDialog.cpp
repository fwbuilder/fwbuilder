/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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
#include "FWBAboutDialog.h"

#include "fwbuilder/Constants.h"

#include "../../VERSION.h"

FWBAboutDialog::FWBAboutDialog() 
{
    m_aboutDialog = new Ui::AboutDialog_q;
    m_aboutDialog->setupUi(this);
    m_aboutDialog->titleLbl->setText(
        QString("Firewall Builder v%1").arg(VERSION) );
    m_aboutDialog->revLbl->setText(tr("Build: %1").arg(build_num));
    m_aboutDialog->apiLbl->setText(
        tr("Using Firewall Builder API %1").arg(
            libfwbuilder::Constants::getLibraryVersion().c_str() ) );

    ABOUT_DLG_BLANKS;

    setWindowTitle(QString("Firewall Builder: About..."));
    adjustSize();
};

FWBAboutDialog::~FWBAboutDialog() { delete m_aboutDialog; };

