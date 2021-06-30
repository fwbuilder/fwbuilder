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



  This class is a simple wrapper for QPixmapCache, it automatically
  creates pixmap if it is not found in the cache

*/



#include "global.h"
#include "utils.h"

#include "PixmapFactory.h"

#include <qpixmap.h>
#include <qpixmapcache.h>

#include <iostream>

using namespace std;


QPixmap PixmapFactory::getPixmap(const std::string &icn_filename)
{
    QPixmap pm;
    if ( ! QPixmapCache::find( icn_filename.c_str(), pm) ) {
        pm.load( icn_filename.c_str() );
        QPixmapCache::insert( icn_filename.c_str(), pm);
        if (fwbdebug)
            qDebug("Created new pixmap from file %s: isNull=%d w=%d h=%d hasAlpha=%d",
                   icn_filename.c_str(),
                   pm.isNull(),
                   pm.width(),
                   pm.height(),
                   pm.hasAlpha()
        );
    }
    return pm;
}

