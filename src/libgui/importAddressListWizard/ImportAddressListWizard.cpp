/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "ImportAddressListWizard.h"
#include "FileNamePage.h"
#include "ChooseObjectsPage.h"
#include "SelectLibraryPage.h"
#include "CreateObjectsPage.h"

#include <QtDebug>

using namespace std;
//using namespace libfwbuilder;


ImportAddressListWizard::ImportAddressListWizard(QWidget *parent) : QWizard(parent)
{
    QPixmap pm;
    pm.load(":/Images/fwbuilder3-72x72.png");
    setPixmap(QWizard::LogoPixmap, pm);

    setWindowTitle(tr("Import address objects from a text file in /etc/hosts format"));

    addPage(new FileNamePage(this));
    addPage(new ChooseObjectsPage(this));
    addPage(new SelectLibraryPage(this));
    addPage(new CreateObjectsPage(this));

    resize(700, 500);
}

