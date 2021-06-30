/*

                          Firewall Builder

                 Copyright (C) 2000-2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "FWObjectClipboard.h"
#include "FWWindow.h"
#include "ProjectPanel.h"

using namespace std;
using namespace libfwbuilder;

FWObjectClipboard* FWObjectClipboard::obj_clipboard=nullptr;

FWObjectClipboard::FWObjectClipboard()
{
    assert(obj_clipboard==nullptr);
    obj_clipboard=this;
}

FWObjectClipboard::~FWObjectClipboard()
{
    clear();
    obj_clipboard=nullptr;
}

void FWObjectClipboard::clear()
{
    if (fwbdebug) qDebug("FWObjectClipboard::clear  size()=%d",
                         int(size()));

    for (vector<std::pair<int,ProjectPanel*> >::iterator i=ids.begin();
         i!=ids.end(); ++i)
    {
        int obj_id = i->first;
        ProjectPanel *proj_p = i->second;
        assert(proj_p);

        FWObject *obj = proj_p->db()->findInIndex(obj_id);
        if (obj)
        {
            if (fwbdebug)
                qDebug("FWObjectClipboard::clear  unref obj=%p (%s)",
                       obj, obj->getName().c_str());

            obj->unref();
        }
    }
    ids.clear();
}

void FWObjectClipboard::add(FWObject *obj, ProjectPanel * proj_p)
{
    if (fwbdebug)
    {
	qDebug("FWObjectClipboard::add  obj=%p (id=%d %s) proj_p=%p",
               obj,
               obj->getId(),
               FWObjectDatabase::getStringId(obj->getId()).c_str(),
               proj_p
        );
    }

    obj->ref();
	
    ids.push_back( pair<int,ProjectPanel*>(obj->getId(), proj_p) );
}

void FWObjectClipboard::remove(FWObject* obj)
{
    int id = obj->getId();
    for (vector<std::pair<int,ProjectPanel*> >::iterator i=ids.begin();
         i!=ids.end(); ++i)
    {
        int obj_id = i->first;
        if (obj_id == id)
        {
            ids.erase(i);
            break;
        }
    }
}

FWObject* FWObjectClipboard::getObject()
{
    if (ids.size()>0)
    {
        pair<int,ProjectPanel*> p = ids.back();
        return p.second->db()->findInIndex( p.first );
    } else
        return nullptr;
}

FWObject* FWObjectClipboard::getObjectByIdx (int idx)
{
    if (idx < int(ids.size()))
    {
        pair<int,ProjectPanel*> p = ids[idx];
        return p.second->db()->findInIndex( p.first );
    } else
        return nullptr;
}

