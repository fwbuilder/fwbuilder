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


#include "config.h"
#include "global.h"

#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "FWObjectClipboard.h"
#include "FWWindow.h"


using namespace std;
using namespace libfwbuilder;

FWObjectClipboard* FWObjectClipboard::obj_clipboard=NULL;

FWObjectClipboard::FWObjectClipboard()
{
    assert(obj_clipboard==NULL);
    obj_clipboard=this;
}

FWObjectClipboard::~FWObjectClipboard()
{
    clear();
    obj_clipboard=NULL;
}

void FWObjectClipboard::clear()
{
    for (vector<string>::iterator i=ids.begin(); i!=ids.end(); ++i)
    {
        FWObject *obj= mw->db()->findInIndex(*i);
        if (obj) obj->unref();
    }
    ids.clear();
}

void FWObjectClipboard::add(FWObject *_obj)
{
    if (fwbdebug)
    {
	qDebug("FWObjectClipboard::add  adding _obj=%p (id=%s)",
               _obj,_obj->getId().c_str());
    }

    _obj->ref();
    ids.push_back(_obj->getId());
}

FWObject* FWObjectClipboard::getObject()
{
    if (ids.size()>0)
        return mw->db()->findInIndex( ids.back() );
    else
        return NULL;
}

