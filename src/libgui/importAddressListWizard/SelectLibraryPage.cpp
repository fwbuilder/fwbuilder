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
#include "utils.h"
#include "FWWindow.h"

#include "SelectLibraryPage.h"

#include <QtDebug>


using namespace std;
//using namespace libfwbuilder;


SelectLibraryPage::SelectLibraryPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::SelectLibraryPage_q;
    m_dialog->setupUi(this);

    registerField("libIndex*", m_dialog->libs);
    registerField("libraries", this, "libraries");

    setCommitPage(true);
}

void SelectLibraryPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "SelectLibraryPage::initializePage()";


    fillLibraries(m_dialog->libs, mw->activeProject()->db());

    for (int i=0; i < m_dialog->libs->count(); ++i)
        libraries << m_dialog->libs->itemText(i);
}
