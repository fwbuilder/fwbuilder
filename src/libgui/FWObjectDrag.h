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


#ifndef  __FWOBJECTDRAG_H_
#define  __FWOBJECTDRAG_H_

//#include <q3dragobject.h>
//Added by qt3to4:
#include <QMimeData>
#include <QDropEvent>
#include <map>
#include <list>
#include <QDrag>

namespace libfwbuilder {
    class FWObject;
};

/*****************************************************************************
 *
 * Class FWObjectDrag
 *
 *****************************************************************************/

class FWObjectDrag : public QDrag {

private:

    std::list<libfwbuilder::FWObject*> objlist;

public:
    
    static QString FWB_MIME_TYPE;

    FWObjectDrag(std::list<libfwbuilder::FWObject*> ol,
                 QWidget *dragSource = 0, const char* name = 0);
    ~FWObjectDrag();

    virtual Qt::DropAction start(Qt::DropActions action = Qt::CopyAction);
    
    QByteArray encodedData() const;
    static bool decode( QDropEvent *ev, std::list<libfwbuilder::FWObject*> &ol);
    static bool decode( QDragEnterEvent *ev, std::list<libfwbuilder::FWObject*> &ol);

};

#endif

