/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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

#include "FWWindow.h"
#include "LibExportDialog.h"
#include "FWBSettings.h"
#include "longTextDialog.h"
#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qpixmapcache.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#ifndef _WIN32
#  include <unistd.h>     // for access(2)
#endif

#include <iostream>
#include <algorithm>

using namespace std;
using namespace libfwbuilder;

LibExportDialog::~LibExportDialog()
{
    delete m_dialog;
}

LibExportDialog::LibExportDialog( QWidget* parent, const char* name, bool  )
    : QDialog(parent)
{
    setWindowTitle(name);
    m_dialog = new Ui::LibExport_q;
    m_dialog->setupUi(this);

    init();
}

void LibExportDialog::init()
{
//    resize( QSize(500, 450) );
    m_dialog->exportRO->setChecked(true);

    m_dialog->libs->clear();
    list<FWObject*> ll = mw->activeProject()->db()->getByType(Library::TYPENAME);
    int n=0;
    string libicn;

    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++,n++)
    {
        /*if (libicn.empty())
            libicn=Resources::global_res->getObjResourceStr(*i,"icon-tree").c_str();*/

        mapOfLibs[n]= (*i);

        QPixmap pm;
        QString icn = (":/Icons/" + (*i)->getTypeName() + "/icon-tree").c_str();
        //QString icn = libicn.c_str();
        if ( ! QPixmapCache::find( icn, pm) )
        {
            pm.load( icn );
            QPixmapCache::insert( icn, pm);
        }

        QListWidgetItem *item = new QListWidgetItem(QString::fromUtf8((*i)->getName().c_str()));
        item->setIcon(QIcon(pm));
        /*m_dialog->libs->addItem(QIcon(pm),
                         QString::fromUtf8((*i)->getName().c_str()),
                         n);*/
        m_dialog->libs->addItem(item);
    }

}




