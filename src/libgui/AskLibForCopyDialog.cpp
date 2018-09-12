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

#include "AskLibForCopyDialog.h"
#include "FWBSettings.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"

#include <qpixmapcache.h>

using namespace std;
using namespace libfwbuilder;

FWObject *AskLibForCopyDialog::askLibForCopyDialog( QWidget *parent, 
    libfwbuilder::FWObjectDatabase *db, libfwbuilder::FWObject *curr)
{
    AskLibForCopyDialog dlg(parent, db, curr);
    if ( dlg.exec() == QDialog::Accepted )
        return dlg.getChoosenLib();
    return nullptr;
}

AskLibForCopyDialog::~AskLibForCopyDialog()
{
    delete m_dialog;
}

AskLibForCopyDialog::AskLibForCopyDialog( QWidget *parent, FWObjectDatabase *db, 
    libfwbuilder::FWObject *curr):
  QDialog(parent), m_db(db), m_curr(curr)
{
    m_dialog = new Ui::asklibforcopydialog_q;
    m_dialog->setupUi(this);
    loadObjects();
}

void AskLibForCopyDialog::loadObjects()
{
    list<FWObject*> ll = m_db->getByType( Library::TYPENAME );
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib = (*i);

        if ((lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID &&
             ! st->getBool("UI/ShowDeletedObjects"))||
            lib->getId() == FWObjectDatabase::STANDARD_LIB_ID ||
            lib->getId() == FWObjectDatabase::TEMPLATE_LIB_ID) 
             continue;

        int ind = addLib( lib );

        if (m_curr == lib)
            m_dialog->libs->setCurrentIndex(ind);
    }
}

int AskLibForCopyDialog::addLib( FWObject *lib)
{
    QString newlibname = QString::fromUtf8(lib->getName().c_str());
    int              N = m_dialog->libs->count();
    int            idx = 0;
    vector<FWObject*>::iterator  i1=idxToLibs.begin();
    //vector<QTreeWidget*>::iterator i2=idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1)
        if ( m_dialog->libs->itemText(idx) > newlibname ) break;

    string icn=":/Icons/"+lib->getTypeName()+"/icon-tree";
            //Resources::global_res->getObjResourceStr(lib,"icon-tree").c_str();
    QPixmap pm;
    if ( ! QPixmapCache::find( icn.c_str(), pm) )
    {
        pm.load( icn.c_str() );
        QPixmapCache::insert( icn.c_str(), pm);
    }
    m_dialog->libs->insertItem( idx, pm, newlibname);
//    idx=libs->count()-1;

    m_dialog->libs->setCurrentIndex(idx);

    idxToLibs.insert(i1,lib);
    return idx;
}

FWObject *AskLibForCopyDialog::getChoosenLib()
{
    int ind = m_dialog->libs->currentIndex();
    if (0 <= ind)
        return idxToLibs[ind];
    return nullptr;
}
